#include "export/nmaxexport.h"
//------------------------------------------------------------------------------
/**
    3ds max to Nebula2 exporter using the IGame lib.

    This file is licensed under the terms of the Nebula License.
    (C) 2004 Oleg Kreptul (Haron)
*/

//------------------------------------------------------------------------------
/**
*/
nAnimBuilder::Group
nMaxExport::createAnimGroup(int numFrames, nAnimBuilder::Group::LoopType loopType)
{ // when we build the curves, we renormalize them to go from 0 to numFrames
    nAnimBuilder::Group animGroup;
    animGroup.SetStartKey(0);
    animGroup.SetNumKeys(numFrames);
    animGroup.SetLoopType(loopType);
    animGroup.SetKeyTime((float)this->ticksPerFrame / (float)4800.0);
    return animGroup;
}

//------------------------------------------------------------------------------
/**
    Looking for bone index in boneIDs array. If id not in the array and flag 'insert'
    is set put id into the array and return its index otherwise return -1
*/
int
nMaxExport::GetBoneByID(int id, bool insert)
{
	int res;
	res = this->boneIDs.FindIndex(id);
	if (res == -1 && insert)
	{
		res = this->boneIDs.Size();
		this->boneIDs.Append(id);
	}
	return res;
}

//------------------------------------------------------------------------------
/**
*/
int
nMaxExport::getCurve(IGameControl *igControl, IGameControlType type, nAnimBuilder::Curve &curve)
{
    IGameKeyTab igKey;
    AffineParts ap;
    int i;
    vector4 v;
    Point3 p;
    Quat q;

    switch (igControl->GetControlType(type))
    {
        case IGameControl::IGAME_UNKNOWN:
        case IGameControl::IGAME_MAXSTD:
            {
                if (igControl->GetBezierKeys(igKey,type))
                {
                    for (i = 0; i < igKey.Count(); i++)
                    {
                        switch (type)
                        {
                        case IGAME_POS:
                            p = igKey[i].bezierKey.pval;
                            v.set(p.x, p.y, p.z, 0.0);
                            break;
                        case IGAME_ROT:
                            q = igKey[i].bezierKey.qval;
                            v.set(q.x, q.y, q.z, q.w);
                            break;
                        case IGAME_SCALE:
                            p = igKey[i].bezierKey.sval.s;
                            v.set(p.x, p.y, p.z, 1.0);
                            break;
                        }
                        curve.SetKey(i, nAnimBuilder::Key(v));
                    }
                }
                else //if (igControl->GetFullSampledKeys(igKey, this->task->sampleRate, type))
                {
                    igControl->GetFullSampledKeys(igKey, this->task->sampleRate, type);
                    for (i = 0; i < igKey.Count(); i++)
                    {
                        switch (type)
                        {
                        case IGAME_POS:
                            p = igKey[i].sampleKey.pval;
                            v.set(p.x, p.y, p.z, 0.0);
                            break;
                        case IGAME_ROT:
                            q = igKey[i].sampleKey.qval;
                            v.set(q.x, q.y, q.z, q.w);
                            break;
                        case IGAME_SCALE:
                            p = igKey[i].sampleKey.sval.s;
                            v.set(p.x, p.y, p.z, 1.0);
                            break;
                        }
                        curve.SetKey(i, nAnimBuilder::Key(v));
                    }
                }
/*
                else
                {
                    n_printf("ERROR(pos): IGAME_MAXSTD - unsupported position key type.!\n");
                    return 0;
                }
*/
            }
            break;
        default:
            n_printf("ERROR: Unsupported Contol type!\n");
            return 0;
    }

    return igKey.Count();
}

//------------------------------------------------------------------------------
/**
*/
void
nMaxExport::getBipedCurves(IGameNode *igNode, nAnimBuilder::Curve &posCurve, nAnimBuilder::Curve &rotCurve, nAnimBuilder::Curve &scaleCurve)
{
    int i, keyFrame;
    for (keyFrame = this->startFrame, i = 0; keyFrame <= this->endFrame; keyFrame++, i++)
    {
        vector4 p, s;
        quaternion q;

        this->GetPRS(igNode, keyFrame * this->ticksPerFrame, p, q, s, 1);

        vector4 r(q.x, q.y, q.z, q.w);

        posCurve.SetKey(i, nAnimBuilder::Key(p));
        rotCurve.SetKey(i, nAnimBuilder::Key(r));
        scaleCurve.SetKey(i, nAnimBuilder::Key(s));
    }
}

//------------------------------------------------------------------------------
/**
    Add 3 curves for current bone (position, rotation and scale curves)
*/
bool
nMaxExport::writeCurves(IGameNode *igNode, nAnimBuilder::Group& animGroup, int& numUncompressedCurves)
{
    nAnimBuilder::Curve posCurve, rotCurve, scaleCurve;

    posCurve.SetIpolType(nAnimBuilder::Curve::LINEAR);  
    rotCurve.SetIpolType(nAnimBuilder::Curve::QUAT); 
    scaleCurve.SetIpolType(nAnimBuilder::Curve::LINEAR);

    IGameControl *igControl;
    igControl = igNode->GetIGameControl();

    if (!igControl ||
        igControl->GetControlType(IGAME_POS) == IGameControl::IGAME_BIPED ||
        igControl->GetControlType(IGAME_ROT) == IGameControl::IGAME_BIPED ||
        igControl->GetControlType(IGAME_SCALE) == IGameControl::IGAME_BIPED)
    {
        this->getBipedCurves(igNode, posCurve, rotCurve, scaleCurve);
    }
    else
    {
        int pN = this->getCurve(igControl, IGAME_POS, posCurve);
        int rN = this->getCurve(igControl, IGAME_ROT, rotCurve);
        int sN = this->getCurve(igControl, IGAME_SCALE, scaleCurve);
        if (pN != rN || pN != sN || pN == 0)
        {
            n_printf("nMaxExport::writeCurves(): ERROR - can't get keys from node %d", igNode->GetNodeID());
            return false;
        }
    }

    if(!posCurve.Optimize())
    {
        posCurve.SetFirstKeyIndex(numUncompressedCurves++);
        posCurve.SetCollapsedKey(posCurve.GetKeyAt(0).Get());
    }

    if(!rotCurve.Optimize())
    {
        rotCurve.SetFirstKeyIndex(numUncompressedCurves++);
        rotCurve.SetCollapsedKey(rotCurve.GetKeyAt(0).Get());
    }

    if(!scaleCurve.Optimize())
    {
        scaleCurve.SetFirstKeyIndex(numUncompressedCurves++);
        scaleCurve.SetCollapsedKey(scaleCurve.GetKeyAt(0).Get());
    }

    animGroup.AddCurve(posCurve);
    animGroup.AddCurve(rotCurve);
    animGroup.AddCurve(scaleCurve);
    //animGroup.SetKeyTime((float)this->ticksPerFrame / (float)4800.0);

    return true;
}

//------------------------------------------------------------------------------
/**
    Add animation curves for each bone and make animation group
*/
void
nMaxExport::exportSkinnedAnim(IGameNode *igNode, nString nodeName, nAnimBuilder *animBuilder)
{
    n_assert(animBuilder);
	nAnimBuilder::Group animGroup = this->createAnimGroup(this->endFrame - this->startFrame + 1, nAnimBuilder::Group::REPEAT);
	IGameNode *igBone;
	int i, boneNum;

    int numUncompressedCurves = 0; // in this group
	boneNum = this->boneIDs.Size();

    for (i = 0; i < boneNum; i++)
	{
		igBone = this->iGameScene->GetIGameNode(this->boneIDs[i]);
        
        //initialize IGameObject temporarily
        igBone->GetIGameObject()->InitializeData();
		
        this->writeCurves(igBone, animGroup, numUncompressedCurves);
        
        //release IGameObject
        igBone->ReleaseIGameObject();
	}

    n_assert(animGroup.Validate());
    animGroup.SetKeyStride(numUncompressedCurves);

    animBuilder->AddGroup(animGroup);
}

//------------------------------------------------------------------------------
/**
    Fill in the boneIDs array
*/
void
nMaxExport::findBones(IGameSkin *igSkin, int vertNum)
{
    int i, n;

    // find all bones num
    for (i = 0; i < vertNum; i++)
    {
        if (igSkin->GetNumberOfBones(i) > 0)
        {
            n = igSkin->GetBoneID(i, 0);
            break;
        }
    }

    // find root bone
    IGameNode *igBone;
    igBone = this->iGameScene->GetIGameNode(n);
    while (igBone->GetNodeParent())
    {
        igBone = igBone->GetNodeParent();
    }

    this->boneIDs.Clear();
    this->traceBonesTree(igBone, this->boneIDs);
}

//------------------------------------------------------------------------------
/**
    Recursive trace of bones tree
*/
void
nMaxExport::traceBonesTree(IGameNode *igBone, nArray<int>& tmpIDs)
{
    int id, i, n;

    id = igBone->GetNodeID();

    tmpIDs.PushBack(id);

    n = igBone->GetChildCount();
    for (i = 0; i < n; i++)
    {
        this->traceBonesTree(igBone->GetNodeChild(i), tmpIDs);
    }
}

