//-----------------------------------------------------------------------------
//
//
//
//-----------------------------------------------------------------------------
#include "export2/nmax.h"
#include "export2/nmaxcontroller.h"
#include "export2/nmaxprscontroller.h"
#include "export2/nmaxikcontroller.h"
#include "export2/nmaxpathcontroller.h"
#include "export2/nmaxanim.h"
#include "pluginlibs/nmaxdlg.h"
#include "pluginlibs/nmaxlogdlg.h"

#include "kernel/ntypes.h"

//-----------------------------------------------------------------------------
/**
*/
nMaxAnim::nMaxAnim()
{
}

//-----------------------------------------------------------------------------
/**
*/
nMaxAnim::~nMaxAnim()
{
}

//-----------------------------------------------------------------------------
/**
*/
void nMaxAnim::Export(INode* inode)
{
    nMaxController* controller;
    controller = CreateController(inode);
    if (controller)
    {
        //controller->Export(inode);
        n_delete(controller);
    }
    else
    {
        n_maxlog(High, "%s node does not have any controller.", inode->GetName());
    }
}

//-----------------------------------------------------------------------------
/**
*/
nMaxController* nMaxAnim::CreateController(INode* inode)
{
    n_assert(inode);

    Control* control = inode->GetTMController();
    if (!control)
    {
        return NULL;
    }

    nMaxController* newControl = 0;

    Control *posControl, *rotControl, *scaleControl;

    posControl   = control->GetPositionController();
    rotControl   = control->GetRotationController();
    scaleControl = control->GetScaleController();

    if (posControl || rotControl || scaleControl)
    {

        if (control->ClassID() == IKSLAVE_CLASSID || 
            posControl->ClassID() == IKSLAVE_CLASSID ||
            rotControl->ClassID() == IKSLAVE_CLASSID)
        {
            nMaxIKController* ikControl = n_new(nMaxIKController);
            ikControl->Export(inode);
            newControl = ikControl;
        }
        else
        if (posControl->ClassID() == Class_ID(PATH_CONTROL_CLASS_ID, 0))
        {
            // path control
            nMaxPathController* pathController = n_new(nMaxPathController);
            pathController->Export(control);
            newControl = pathController;

        }
        else
        {
            nMaxPRSController* prsController = n_new(nMaxPRSController);
            prsController->Export(inode);
            newControl = prsController;
        }
    }
    else
    {
        n_maxlog(High, "%s node has Control but it does not contain have any PRS controller.", inode->GetName());
    }

    return newControl;
}

//-----------------------------------------------------------------------------
/**
    float
    linear interpolation
    
  
*/
/*
nMaxAnim::Type nMaxAnim::GetType(Control* control)
{
    ulong id;
    id = control->ClassID().PartA();

    // check controller sub classes type
    switch(id)
    {
    case LININTERP_POSITION_CLASS_ID:
    case LININTERP_ROTATION_CLASS_ID:
    case LININTERP_SCALE_CLASS_ID:
    case HYBRIDINTERP_POSITION_CLASS_ID:
    case HYBRIDINTERP_ROTATION_CLASS_ID:
    case HYBRIDINTERP_SCALE_CLASS_ID:
    case TCBINTERP_POSITION_CLASS_ID:
    case TCBINTERP_ROTATION_CLASS_ID:
    case TCBINTERP_SCALE_CLASS_ID:
        return nMaxAnim::PRS;

    case LININTERP_FLOAT_CLASS_ID:
    case HYBRIDINTERP_FLOAT_CLASS_ID:
    case TCBINTERP_FLOAT_CLASS_ID:
        return nMaxAnim::Float;

    case HYBRIDINTERP_POINT3_CLASS_ID:
    case HYBRIDINTERP_COLOR_CLASS_ID:
    case TCBINTERP_POINT3_CLASS_ID:
        return nMaxAnim::Point3;

    case PRS_CONTROL_CLASS_ID:
    case LOOKAT_CONTROL_CLASS_ID:
        break;

    case HYBRIDINTERP_POINT4_CLASS_ID:
    case HYBRIDINTERP_FRGBA_CLASS_ID:
        break;

    case TCBINTERP_POINT4_CLASS_ID:
    case MASTERPOINTCONT_CLASS_ID:
    default:
        break;
    }
}
*/

//-----------------------------------------------------------------------------
/*
int nMaxAnim::Vecotr3Anim(Control* control)
{
    int ret = 0;
    IKeyControl *pIKeys;

    *ppKeys = NULL;
    *pNumKeys = 0;

    if (control)
    {
        Class_ID id;

        id = control->ClassID(); 

        pIKeys = GetKeyControlInterface(control);
        if (pIKeys != NULL)
        {
            if (pIKeys->GetNumKeys() == 0)
                return 0;
            else 
            if (id == Class_ID(TCBINTERP_POINT3_CLASS_ID, 0))
                ret = ConvertTCBPoint3(control, pIKeys, pNumKeys, ppKeys, pKeyType);
            else 
            if ((id == Class_ID(HYBRIDINTERP_COLOR_CLASS_ID, 0)) ||(id == Class_ID(HYBRIDINTERP_POINT3_CLASS_ID, 0)))
                ret = ConvertBezPoint3(control, pIKeys,pNumKeys, ppKeys, pKeyType);
            else
                ret = ConvertDumbPoint3(control, pNumKeys, ppKeys, pKeyType);
        }
        else
            ret = ConvertDumbPoint3(control, pNumKeys, ppKeys, pKeyType);
    }
    else
    {
        ret = ConvertDumbPoint3(control, pNumKeys, ppKeys, pKeyType);
    }

    return ret;
}

//-----------------------------------------------------------------------------
int nMaxAnim::ConvertTCBPoint3(Control *pCC, 
                                              IKeyControl *pIKeys, 
                                              unsigned int *pNumKeys,
                                              NiPosKey **ppKeys,
                                              NiAnimationKey::KeyType *pKeyType)
{
    NiTCBPosKey *pKeys;

    //pKeys = new NiTCBPosKey[uiNumKeys];
    //if (pKeys == NULL)
    //    return(W3D_STAT_NO_MEMORY);
    nKernelServer* ks = nKernelServer::Instance();
    nVectorAnimator* animator = (nVectorAnimator*)ks->New("nvectoranimator", "");
    animator->SetVectorName();


    uint numKeys = pIKeys->GetNumKeys();
    for (uint i = 0; i<numKeys; i++)
    {
        ITCBPoint3Key tcb;
        
        pIKeys->GetKey(i, &tcb);

        vector4 val;
        val.x = tcb.val.x;
        val.y = tcb.val.y;
        val.z = tcb.val.z;
        val.w = 0.0f;

        //pKeys[i].SetTime(tcb.time * SECONDSPERTICK);
        //pKeys[i].SetPos(pos);
        //pKeys[i].SetTension(tcb.tens);
        //pKeys[i].SetContinuity(tcb.cont);
        //pKeys[i].SetBias(tcb.bias);
        animator->AddKey(tcb.time * SECONDSPERTICK, val);

    }

    *pNumKeys = uiNumKeys;
    *ppKeys = (NiPosKey *) pKeys;
    *pKeyType = NiPosKey::TCBKEY;

    return(W3D_STAT_OK);
}

//-----------------------------------------------------------------------------
int nMaxAnim::ConvertDumbPoint3(Control *pCol,
                                               unsigned int *pNumKeys, 
                                               NiPosKey **ppKeys,
                                               NiAnimationKey::KeyType *pKeyType)
{
    NiLinPosKey *pKeys;
    TimeValue t;
    unsigned int i;
    Point3 val;
    Interval range;

    if (m_uiNumFrames == 0)
    {
        *pNumKeys = 0;
        *ppKeys = NULL;
        *pKeyType = NiAnimationKey::NOINTERP;
        return(W3D_STAT_OK);
    }
    
    pKeys = new NiLinPosKey[m_uiNumFrames];
    if (pKeys == NULL)
        return(W3D_STAT_NO_MEMORY);

    for (i = 0, t = m_animStart; 
         i < m_uiNumFrames; 
         i++, t += GetTicksPerFrame())
    {
        pCol->GetValue(t, &val, range);

        pKeys[i].SetTime(t * SECONDSPERTICK);
        pKeys[i].SetPos(NiPoint3(val.x, val.y, val.z));
    }

    *pNumKeys = m_uiNumFrames;
    *ppKeys = (NiPosKey*) pKeys;
    *pKeyType = NiPosKey::LINKEY;

    return(W3D_STAT_OK);
}
*/
