//----------------------------------------------------------------------------
// (c) 2006    Vadim Macagon
//----------------------------------------------------------------------------
#include "lwexporter/nlwskinanimexport.h"
#include "lwexporter/nlwobjectexportsettings.h"
#include "lwexporter/nlwexportersettings.h"
#include "lwexporter/nlwanimexporthandler.h"
#include "lwwrapper/nlwlayoutmonitor.h"
#include "kernel/nkernelserver.h"
#include "scene/nskinanimator.h"
#include "lwwrapper/nlwcmdexec.h"

const nLWBoneJointMap* nLWSkinAnimExport::curBoneJointMap = 0;
int nLWSkinAnimExport::curAnimGroupStartFrame = 0;
nAnimBuilder::Group* nLWSkinAnimExport::curAnimGroup = 0;

//----------------------------------------------------------------------------
/**
*/
nLWSkinAnimExport::nLWSkinAnimExport(nLWObjectExportSettings* objSettings,
                                     nLWCmdExec* cmdExec) :
    objSettings(objSettings),
    cmdExec(cmdExec)
{
    // empty
}

//----------------------------------------------------------------------------
/**
*/
nLWSkinAnimExport::~nLWSkinAnimExport()
{
    // empty
}

//----------------------------------------------------------------------------
/**
    @brief Export animation curves and create a skin animator node.
    @return false if some sort of major error occurred, true otherwise.
*/
bool 
nLWSkinAnimExport::Run(nLWLayoutMonitor* monitor)
{
    nKernelServer* ks = nKernelServer::Instance();
    nObject* object = ks->NewNoFail("nskinanimator", "skinanimator");
    nSkinAnimator* skinAnimator = static_cast<nSkinAnimator*>(object);
    
    if (!skinAnimator)
        return false;

    // set skin animator skeleton
    if (!this->CollectJoints(skinAnimator, monitor))
        return false;

    // set skin animator animation states
    if (!this->CollectAnimStates(skinAnimator, monitor))
        return false;

    nString animFileName;
    if (!this->ConstructAnimFileName(animFileName, monitor))
        return false;

    // export animation curves
    if (!this->ExportAnimation(animFileName, monitor))
        return false;

    skinAnimator->SetAnim(animFileName);
    skinAnimator->SetChannel("time");
    skinAnimator->SetLoopType(nAnimLoopType::Loop);
    skinAnimator->SetStateChannel(this->objSettings->GetAnimationVarName().Get());

    return true;
}

namespace
{
    //------------------------------------------------------------------------
    /**
    */
    bool 
    ValidateBones(LWItemID boneId, LWBoneInfo* boneInfo, 
                  LWItemInfo* itemInfo, nLWLayoutMonitor* monitor)
    {
        unsigned int boneFlags = boneInfo->flags(boneId);
        if ((boneFlags & LWBONEF_WEIGHT_MAP_ONLY) == 0)
        {
            nString monitorMsg("[ERROR] Deformation must be based solely on the weight map!\n");
            if (monitor)
            {
                monitor->Step(0, monitorMsg);
            }
            n_printf(monitorMsg.Get());
            return false;
        }
        
        LWItemID childId = itemInfo->firstChild(boneId);
        while (childId != LWITEM_NULL)
        {
            if (itemInfo->type(childId) == LWI_BONE)
            {
                if (!ValidateBones(childId, boneInfo, itemInfo, monitor))
                {
                    return false;
                }
            }
            childId = itemInfo->nextChild(boneId, childId);
        }

        return true;
    }

    //------------------------------------------------------------------------
    /**
    */
    int
    GetNumJoints(LWItemID boneId, LWItemInfo* itemInfo)
    {
        int numJoints = 1;
        LWItemID childId = itemInfo->firstChild(boneId);
        while (childId != LWITEM_NULL)
        {
            if (itemInfo->type(childId) == LWI_BONE)
            {
                numJoints += GetNumJoints(childId, itemInfo);
            }
            childId = itemInfo->nextChild(boneId, childId);
        }
        return numJoints;
    }

    //----------------------------------------------------------------------------
    /**
    */
    LWItemID
    GetRootBoneId(LWItemID itemId, LWObjectInfo* objectInfo, LWItemInfo* itemInfo)
    {
        // the bones that deform the object may actually belong to a different
        // object so need to find the rightful owner
        LWItemID boneOwnerId = objectInfo->boneSource(itemId);
        return itemInfo->first(LWI_BONE, boneOwnerId);
    }

} // anonymous namespace

//------------------------------------------------------------------------
/**
*/
bool
nLWSkinAnimExport::AddJointsToSkinAnimator(nSkinAnimator* skinAnimator, 
                                           LWItemID boneId,
                                           int parentJointIdx, 
                                           LWBoneInfo* boneInfo, 
                                           LWItemInfo* itemInfo, 
                                           float scale)
{
    LWDVector lwPos;
    boneInfo->restParam(boneId, LWIP_POSITION, lwPos);
    vector3 poseTranslate((float)lwPos[0], (float)lwPos[1], (float)lwPos[2]);

    LWDVector lwRot;
    boneInfo->restParam(boneId, LWIP_ROTATION, lwRot);
    quaternion poseRotate;
    // HPB (yxz) -> (xyz)
    poseRotate.set_rotate_xyz((float)lwRot[1], (float)lwRot[0], (float)lwRot[2]);

    LWDVector lwScale;
    boneInfo->restParam(boneId, LWIP_SCALING, lwScale);
    vector3 poseScale((float)lwScale[0], (float)lwScale[1], (float)lwScale[2]);
    if (scale != 1.0f)
    {
        // take into account the global scale
        poseScale *= scale;
    }

    nString jointName(itemInfo->name(boneId));
    jointName.ReplaceIllegalFilenameChars('_');

    int jointIdx = this->boneJointMap.GetJointIndex(boneId);
    n_assert(jointIdx != -1);
    if (jointIdx != -1)
    {
        skinAnimator->SetJoint(jointIdx, parentJointIdx, poseTranslate, 
                               poseRotate, poseScale, jointName);
    }
    
    LWItemID childId = itemInfo->firstChild(boneId);
    while (childId != LWITEM_NULL)
    {
        if (itemInfo->type(childId) == LWI_BONE)
        {
            if (!this->AddJointsToSkinAnimator(skinAnimator, childId, 
                                               jointIdx, boneInfo, 
                                               itemInfo, scale))
            {
                return false;
            }
        }
        childId = itemInfo->nextChild(boneId, childId);
    }

    return true;
}

//------------------------------------------------------------------------
/**
*/
bool
nLWSkinAnimExport::AddJointsToSkinAnimator(nSkinAnimator* skinAnimator, 
                                           LWItemID rootBoneId,
                                           LWBoneInfo* boneInfo, 
                                           LWItemInfo* itemInfo)
{
    nLWExporterSettings* exporterSettings = nLWExporterSettings::Instance();
    n_assert(exporterSettings);
    if (!exporterSettings)
        return false;

    return this->AddJointsToSkinAnimator(skinAnimator, rootBoneId, -1, 
                                         boneInfo, itemInfo,
                                         exporterSettings->GetGeometryScale());
}

//----------------------------------------------------------------------------
/**
*/
bool 
nLWSkinAnimExport::CollectJoints(nSkinAnimator* skinAnimator,
                                 nLWLayoutMonitor* monitor)
{
    nLWGlobals::ObjectInfo objectInfo;
    if (!objectInfo.IsValid())
        return false;

    nLWGlobals::BoneInfo boneInfo;
    if (!boneInfo.IsValid())
        return false;

    nLWGlobals::ItemInfo itemInfo;
    if (!itemInfo.IsValid())
        return false;

    LWItemID rootBoneId = GetRootBoneId(this->objSettings->GetItemId(),
                                        objectInfo.Get(), itemInfo.Get());
    if (LWITEM_NULL == rootBoneId)
        return false;
    
    ValidateBones(rootBoneId, boneInfo.Get(), itemInfo.Get(), monitor);

    this->boneJointMap.Fill(rootBoneId);

    // populate the skin animator joint list
    skinAnimator->BeginJoints(GetNumJoints(rootBoneId, itemInfo.Get()));
    if (!this->AddJointsToSkinAnimator(skinAnimator, rootBoneId, 
                                       boneInfo.Get(), itemInfo.Get()))
    {
        return false;
    }
    skinAnimator->EndJoints();

    return true;
}

//----------------------------------------------------------------------------
/**
*/
bool 
nLWSkinAnimExport::CollectAnimStates(nSkinAnimator* skinAnimator, 
                                     nLWLayoutMonitor* monitor)
{
    const nArray<nLWAnimationState>& stateArray = this->objSettings->GetAnimationStates();
    skinAnimator->BeginStates(stateArray.Size());
    for (int i = 0; i < stateArray.Size(); i++)
    {
        skinAnimator->SetState(i, i, stateArray[i].fadeInTime);
        skinAnimator->SetStateName(i, stateArray[i].name);
        skinAnimator->BeginClips(i, 1);
        skinAnimator->SetClip(i, 0, "one");
        skinAnimator->EndClips(i);
    }
    skinAnimator->EndStates();
    return true;
}

//----------------------------------------------------------------------------
/**
*/
bool
nLWSkinAnimExport::ConstructAnimFileName(nString& animFileName, 
                                         nLWLayoutMonitor* monitor) const
{
    n_assert(this->objSettings);
    if (!this->objSettings)
        return false;

    nLWExporterSettings* exporterSettings = nLWExporterSettings::Instance();
    n_assert(exporterSettings);
    if (!exporterSettings)
        return false;

    nLWGlobals::ObjectInfo objectInfo;
    n_assert(objectInfo.IsValid());
    if (!objectInfo.IsValid())
        return false;

    nLWGlobals::ItemInfo itemInfo;
    n_assert(itemInfo.IsValid());
    if (!itemInfo.IsValid())
        return false;

    // the absolute path to the object file
    nString objFile = objectInfo.Get()->filename(this->objSettings->GetItemId());
    nString objFileDir = objFile.ExtractToLastSlash();
    objFileDir.ToLower();
    objFileDir.ConvertBackslashes();
    objFileDir.StripTrailingSlash();
    objFileDir += "/";

    // the absolute path to the current Lightwave Objects directory.
    nLWGlobals::DirInfoFunc dirInfo;
    nString lwObjectsDir = dirInfo.GetObjectsDir();
    lwObjectsDir.ToLower();
    lwObjectsDir.ConvertBackslashes();
    lwObjectsDir.StripTrailingSlash();
    lwObjectsDir += "/";

    animFileName = "anims:";
    
    // figure out what the category dir is and append it to the filename
    if (objFileDir.Length() > lwObjectsDir.Length())
    {
        if (strncmp(objFileDir.Get(), lwObjectsDir.Get(), lwObjectsDir.Length()) == 0)
        {
            animFileName += &(objFileDir.Get()[lwObjectsDir.Length()]);
        }
        else
        {
            nString msg;
            msg.Format("[ERROR] Object file %s was not found in the current "
                       "Lightwave Content directory.\n", objFile.Get());
            if (monitor)
            {
                monitor->Step(0, msg);
            }
            n_printf(msg.Get());
        }
    }
    
    nString shortName = itemInfo.Get()->name(this->objSettings->GetItemId());
    shortName.ReplaceIllegalFilenameChars('_');
    animFileName += shortName;

    if (exporterSettings->GetOutputBinaryAnimation())
    {
        animFileName += ".nax2";
    }
    else
    {
        animFileName += ".nanim2";
    }

    return true;
}

//----------------------------------------------------------------------------
/**
    @brief Recursively attach the animation export handler to the given bone
           and it's children.
    @return true if the animation export handler was successfully attached,
            false otherwise.
*/
bool 
nLWSkinAnimExport::AttachFrameHandler(LWItemID boneId, LWItemInfo* itemInfo)
{
    if (!nLWAnimExportHandler::AttachToBone(boneId, this->cmdExec))
    {
        return false;
    }
    
    LWItemID childId = itemInfo->firstChild(boneId);
    while (childId != LWITEM_NULL)
    {
        if (itemInfo->type(childId) == LWI_BONE)
        {
            if (!this->AttachFrameHandler(childId, itemInfo))
            {
                return false;
            }
        }
        childId = itemInfo->nextChild(boneId, childId);
    }

    return true;
}

//----------------------------------------------------------------------------
/**
    @brief Recursively detach the animation export handler from the given bone
           and it's children.
    @return true if the animation export handler was successfully detached,
            false otherwise.
*/
bool 
nLWSkinAnimExport::DetachFrameHandler(LWItemID boneId, LWItemInfo* itemInfo)
{
    if (!nLWAnimExportHandler::DetachFromBone(boneId, this->cmdExec))
    {
        return false;
    }
    
    LWItemID childId = itemInfo->firstChild(boneId);
    while (childId != LWITEM_NULL)
    {
        if (itemInfo->type(childId) == LWI_BONE)
        {
            if (!this->DetachFrameHandler(childId, itemInfo))
            {
                return false;
            }
        }
        childId = itemInfo->nextChild(boneId, childId);
    }

    return true;
}

//----------------------------------------------------------------------------
/**
*/
void 
nLWSkinAnimExport::BeginFrameGrab(int startFrame, nAnimBuilder::Group* animGroup)
{
    nLWSkinAnimExport::curBoneJointMap = &this->boneJointMap;
    nLWSkinAnimExport::curAnimGroupStartFrame = startFrame;
    nLWSkinAnimExport::curAnimGroup = animGroup;
}

//----------------------------------------------------------------------------
/**
*/
bool 
nLWSkinAnimExport::GrabFrame(int frameNum)
{
    nString cmdStr("GoToFrame ");
    cmdStr.AppendInt(frameNum);
    return this->cmdExec->ExecuteCmd(cmdStr);
}

//----------------------------------------------------------------------------
/**
*/
void 
nLWSkinAnimExport::EndFrameGrab()
{
    nLWSkinAnimExport::curBoneJointMap = 0;
    nLWSkinAnimExport::curAnimGroupStartFrame = 0;
    nLWSkinAnimExport::curAnimGroup = 0;
}

//----------------------------------------------------------------------------
/**
*/
bool 
nLWSkinAnimExport::GetAnimCurvesForBone(LWItemID boneId,
                                        int& startFrame,
                                        nAnimBuilder::Curve** transCurve, 
                                        nAnimBuilder::Curve** rotCurve, 
                                        nAnimBuilder::Curve** scaleCurve)
{
    n_assert(nLWSkinAnimExport::curBoneJointMap);
    if (!nLWSkinAnimExport::curBoneJointMap)
        return false;

    n_assert(nLWSkinAnimExport::curAnimGroup);
    if (!nLWSkinAnimExport::curAnimGroup)
        return false;

    int jointIdx = nLWSkinAnimExport::curBoneJointMap->GetJointIndex(boneId);
    if (jointIdx != -1)
    {
        startFrame = nLWSkinAnimExport::curAnimGroupStartFrame;
        *transCurve = &(nLWSkinAnimExport::curAnimGroup->GetCurveAt(jointIdx * 3));
        *rotCurve = &(nLWSkinAnimExport::curAnimGroup->GetCurveAt(jointIdx * 3 + 1));
        *scaleCurve = &(nLWSkinAnimExport::curAnimGroup->GetCurveAt(jointIdx * 3 + 2));
        return true;
    }
    return false;
}

//----------------------------------------------------------------------------
/**
*/
bool 
nLWSkinAnimExport::ExportAnimation(const nString& fileName,
                                   nLWLayoutMonitor* monitor)
{
    nKernelServer* ks = nKernelServer::Instance();
    n_assert(ks);
    if (!ks)
        return false;

    nLWGlobals::ObjectInfo objectInfo;
    if (!objectInfo.IsValid())
        return false;

    nLWGlobals::ItemInfo itemInfo;
    if (!itemInfo.IsValid())
        return false;

    nLWGlobals::SceneInfo sceneInfo;
    if (!sceneInfo.IsValid())
        return false;

    nString monitorMsg;
    nAnimBuilder animBuilder;
    const nArray<nLWAnimationState>& states = this->objSettings->GetAnimationStates();
    LWItemID rootBoneId = GetRootBoneId(this->objSettings->GetItemId(),
                                        objectInfo.Get(), itemInfo.Get());
    
    if (!this->AttachFrameHandler(rootBoneId, itemInfo.Get()))
        return false;

    const float keyDuration = (float)(1.0 / sceneInfo.Get()->framesPerSecond);

    int numJoints = GetNumJoints(rootBoneId, itemInfo.Get());

    for (int stateIdx = 0; stateIdx < states.Size(); stateIdx++)
    {
        const nLWAnimationState& state = states[stateIdx];
        int startFrame = state.startFrame;
        int endFrame = state.endFrame;
        int numStateKeys = endFrame - startFrame + 1;

        nAnimBuilder::Group animGroup;
        animGroup.SetLoopType(state.repeat ? nAnimBuilder::Group::REPEAT 
                                           : nAnimBuilder::Group::CLAMP);
        animGroup.SetKeyTime(keyDuration);
        animGroup.SetNumKeys(numStateKeys);

        nAnimBuilder::Curve transCurve;
        transCurve.SetIpolType(nAnimBuilder::Curve::LINEAR);

        nAnimBuilder::Curve rotCurve;
        rotCurve.SetIpolType(nAnimBuilder::Curve::QUAT);
        
        nAnimBuilder::Curve scaleCurve;
        scaleCurve.SetIpolType(nAnimBuilder::Curve::LINEAR);

        // add all the empty curves into the group
        for (int jointIdx = 0; jointIdx < numJoints; jointIdx++)
        {
            animGroup.AddCurve(transCurve);
            animGroup.AddCurve(rotCurve);
            animGroup.AddCurve(scaleCurve);    
        }

        // fill in the curves
        this->BeginFrameGrab(startFrame, &animGroup);
        for (int curFrame = startFrame; curFrame <= endFrame; curFrame++)
        {
            if (!this->GrabFrame(curFrame))
            {
                monitorMsg.Format("Failed to grab frame %d\n", curFrame);
                if (monitor)
                {
                    monitor->Step(0, monitorMsg);
                }
                n_printf(monitorMsg.Get());
            }
        }
        this->EndFrameGrab();

        animBuilder.AddGroup(animGroup);
    }

    this->DetachFrameHandler(rootBoneId, itemInfo.Get());

    monitorMsg = "Optimizing animation curves...\n";
    if (monitor)
    {
        monitor->Step(0, monitorMsg);
    }
    n_printf(monitorMsg.Get());

    int numOptimizedCurves = animBuilder.Optimize();

    monitorMsg.Format("Optimized %d curve(s).\n", numOptimizedCurves);
    if (monitor)
    {
        monitor->Step(0, monitorMsg);
    }
    n_printf(monitorMsg.Get());

    animBuilder.FixKeyOffsets();

    if (animBuilder.Save(ks->GetFileServer(), fileName.Get()))
    {
        monitorMsg.Format("[INFO] Saved %s\n", fileName.Get());
        if (monitor)
        {
            monitor->Step(0, monitorMsg);
        }
        n_printf(monitorMsg.Get());
    }
    else
    {
        monitorMsg.Format("[ERROR] Failed to save %s\n", fileName.Get());
        if (monitor)
        {
            monitor->Step(0, monitorMsg);
        }
        n_printf(monitorMsg.Get());
        return false;
    }
    
    return true;
}

//----------------------------------------------------------------------------
// EOF
//----------------------------------------------------------------------------
