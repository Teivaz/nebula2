//----------------------------------------------------------------------------
// (c) 2006    Vadim Macagon
//----------------------------------------------------------------------------
#include "lwexporter/nlwanimexporthandler.h"
#include "lwwrapper/nlwcmdexec.h"
#include "lwexporter/nlwskinanimexport.h"
#include "lwexporter/nlwexportersettings.h"
#include "tools/nanimbuilder.h"
#include "mathlib/quaternion.h"
#include "lwwrapper/nlwglobals.h"

const char nLWAnimExportHandler::HANDLER_NAME[] = "N2_ExporterAnimExpHandler";

//----------------------------------------------------------------------------
/**
*/
nLWAnimExportHandler::nLWAnimExportHandler(LWItemID boneId) :
    boneId(boneId)
{
    // empty
}

//----------------------------------------------------------------------------
/**
*/
bool
nLWAnimExportHandler::AttachToBone(LWItemID boneId, nLWCmdExec* cmdExec)
{
    nString cmdStr;

    cmdStr.Format("SelectItem %x", boneId);

    if (!cmdExec->ExecuteCmd(cmdStr))
    {
        return false;
    }

    cmdStr.Format("ApplyServer %s %s",
                  LWITEMMOTION_HCLASS,
                  nLWAnimExportHandler::HANDLER_NAME);

    if (!cmdExec->ExecuteCmd(cmdStr))
    {
        return false;
    }

    return true;
}

//----------------------------------------------------------------------------
/**
*/
bool
nLWAnimExportHandler::DetachFromBone(LWItemID boneId, nLWCmdExec* cmdExec)
{
    nString cmdStr;

    cmdStr.Format("SelectItem %x", boneId);

    if (!cmdExec->ExecuteCmd(cmdStr))
    {
        return false;
    }

    nLWGlobals::ItemInfo itemInfo;
    n_assert(itemInfo.IsValid());
    if (!itemInfo.IsValid())
    {
        return false;
    }

    for (int serverIdx = 1; ; serverIdx++)
    {
        const char* serverName = itemInfo.Get()->server(boneId, LWITEMMOTION_HCLASS, serverIdx);
        if (!serverName)
        {
            break;
        }
        if (strcmp(serverName, nLWAnimExportHandler::HANDLER_NAME) == 0)
        {
            cmdStr.Format("RemoveServer %s %d", LWITEMMOTION_HCLASS, serverIdx);

            if (!cmdExec->ExecuteCmd(cmdStr))
            {
                return false;
            }
        }
    }

    return true;
}

//----------------------------------------------------------------------------
/**
*/
XCALL_(int)
nLWAnimExportHandler::Activate_Handler(long version,
                                       GlobalFunc* global,
                                       LWItemMotionHandler* local,
                                       void* serverData)
{
    if (version != LWITEMMOTION_VERSION)
        return AFUNC_BADVERSION;

    if (!local)
        return AFUNC_BADLOCAL;

    if (!local->inst)
        return AFUNC_BADLOCAL;

    local->inst->create  = nLWAnimExportHandler::Create;
    local->inst->destroy = nLWAnimExportHandler::Destroy;
    local->inst->load    = nLWAnimExportHandler::Load;
    local->inst->save    = nLWAnimExportHandler::Save;
    local->inst->copy    = nLWAnimExportHandler::Copy;
    local->inst->descln  = nLWAnimExportHandler::Describe;
    local->evaluate      = nLWAnimExportHandler::Evaluate;
    local->flags         = nLWAnimExportHandler::Flags;

    return AFUNC_OK;
}

//----------------------------------------------------------------------------
/**
*/
XCALL_(LWInstance)
nLWAnimExportHandler::Create(void* priv, void* boxedItemId, LWError*)
{
    return (LWInstance)(n_new(nLWAnimExportHandler((LWItemID)boxedItemId)));
}

//----------------------------------------------------------------------------
/**
*/
XCALL_(void)
nLWAnimExportHandler::Destroy(LWInstance boxedHandler)
{
    if (boxedHandler)
    {
        n_delete((nLWAnimExportHandler*)boxedHandler);
    }
}

//----------------------------------------------------------------------------
/**
*/
XCALL_(LWError)
nLWAnimExportHandler::Copy(LWInstance boxedDest, LWInstance boxedSrc)
{
    if (!boxedDest || !boxedSrc)
        return "Copy failed!";

    nLWAnimExportHandler* dest = (nLWAnimExportHandler*)boxedDest;
    nLWAnimExportHandler* src = (nLWAnimExportHandler*)boxedSrc;
    *dest = *src;

    // no errors
    return 0;
}

//----------------------------------------------------------------------------
/**
*/
XCALL_(LWError)
nLWAnimExportHandler::Load(LWInstance, const LWLoadState*)
{
    // plugin doesn't need to load any data
    // no errors
    return 0;
}

//----------------------------------------------------------------------------
/**
*/
XCALL_(LWError)
nLWAnimExportHandler::Save(LWInstance, const LWSaveState*)
{
    // plugin doesn't need to save any data
    // no errors
    return 0;
}

//----------------------------------------------------------------------------
/**
*/
XCALL_(const char*)
nLWAnimExportHandler::Describe(LWInstance)
{
    return nLWAnimExportHandler::HANDLER_NAME;
}

//----------------------------------------------------------------------------
/**
*/
XCALL_(void)
nLWAnimExportHandler::Evaluate(LWInstance lwInst,
                               const LWItemMotionAccess* access)
{
    n_assert(lwInst);
    if (!lwInst)
    {
        return;
    }

    n_assert(access);
    if (!access)
    {
        return;
    }

    nLWAnimExportHandler* self = (nLWAnimExportHandler*)lwInst;

    if (LWITEM_NULL == self->boneId)
    {
        return;
    }

    int startFrame = 0;
    nAnimBuilder::Curve* transCurve = 0;
    nAnimBuilder::Curve* rotCurve = 0;
    nAnimBuilder::Curve* scaleCurve = 0;
    if (nLWSkinAnimExport::GetAnimCurvesForBone(self->boneId,
                                                startFrame,
                                                &transCurve,
                                                &rotCurve,
                                                &scaleCurve))
    {
        nAnimBuilder::Key animKey;
        LWDVector lwVec;
        int animKeyIdx = access->frame - startFrame;
        n_assert(animKeyIdx >= 0);

        n_assert(transCurve);
        if (transCurve)
        {
            access->getParam(LWIP_POSITION, access->time, lwVec);
            animKey.Set(vector4((float)lwVec[0], (float)lwVec[1], (float)lwVec[2], 0.0f));
            transCurve->SetKey(animKeyIdx, animKey);
        }

        n_assert(rotCurve);
        if (rotCurve)
        {
            // get the HPB (heading, pitch, banking) (yxz)
            access->getParam(LWIP_ROTATION, access->time, lwVec);
            quaternion rotSample;
            // re-arrange components to get (xyz)
            rotSample.set_rotate_xyz((float)lwVec[1], (float)lwVec[0], (float)lwVec[2]);
            quaternion rotOther;
            rotOther.set_rotate_xyz(0.0f, 0.0f, 0.0f);
            rotSample = rotOther * rotSample;
            animKey.Set(vector4(rotSample.x, rotSample.y, rotSample.z, rotSample.w));
            rotCurve->SetKey(animKeyIdx, animKey);
        }

        n_assert(scaleCurve);
        if (scaleCurve)
        {
            access->getParam(LWIP_SCALING, access->time, lwVec);
            vector3 scaleSample((float)lwVec[0], (float)lwVec[1], (float)lwVec[2]);
            nLWExporterSettings* exporterSettings = nLWExporterSettings::Instance();
            if (exporterSettings)
            {
                if (exporterSettings->GetGeometryScale() != 1.0f)
                {
                    // take into account the global scale
                    scaleSample *= exporterSettings->GetGeometryScale();
                }
            }
            animKey.Set(vector4(scaleSample.x, scaleSample.y, scaleSample.z, 0.0f));
            scaleCurve->SetKey(animKeyIdx, animKey);
        }
    }
}

//----------------------------------------------------------------------------
/**
*/
XCALL_(unsigned int)
nLWAnimExportHandler::Flags(LWInstance)
{
    return LWIMF_AFTERIK;
}

//----------------------------------------------------------------------------
// EOF
//----------------------------------------------------------------------------
