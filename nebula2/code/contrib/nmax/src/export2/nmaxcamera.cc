//---------------------------------------------------------------------------
//  nmaxcamera.cc
//
//  (c)2004 Kim, Hyoun Woo
//---------------------------------------------------------------------------
#include "export2/nmax.h"
#include "export2/nmaxinterface.h"
#include "export2/nmaxcamera.h"
#include "kernel/ntypes.h"

//---------------------------------------------------------------------------
/**
*/
nMaxCamera::nMaxCamera()
{
}

//---------------------------------------------------------------------------
/**
*/
nMaxCamera::~nMaxCamera()
{
}

//---------------------------------------------------------------------------
/**
*/
void nMaxCamera::Export(INode* inode, Object* obj)
{
    n_assert(inode);
    n_assert(obj);

    TimeValue animStart = nMaxInterface::Instance()->GetAnimStartTime();

    // get camera object.
    CameraObject* camObj = static_cast<CameraObject*>(obj);

    // get camera state.
    Interval interval;
    CameraState camState;
    camObj->EvalCameraState(animStart, interval, &camState);

    bool fixedCam = false;
    if (camObj->GetManualClip())
        fixedCam = true;

    // get field of view.
    float fov    = camState.fov;
    // view mode (true: orthogonal, false:perspective)
    BOOL isOrtho = camState.isOrtho;

    // inverse aspect ratio.
    Interface* intf = nMaxInterface::Instance()->GetInterface();
    float invAspectRatio = 1.0f / intf->GetRendImageAspect();

    if (isOrtho)
    {
        float width  = 640.0f / 4.0f;
        float height = 480.0f / 4.0f;
        float scalar = camObj->GetTDist(0) / 200.0f;

    }
    else
    {
        ;//FIXME: calc perspective view settting should be here.
    }

    if (fixedCam)
    {
        float nearPlane = camState.hither;
        float farPlane  = camState.yon;
    }
    else
    {
        float nearPlane = 1.0f;
        float farPlane  = 6000.0f;
    }

    // check the camera is used for current viewport.
    ViewExp* viewport = intf->GetActiveViewport();
    if (viewport)
    {
        INode* viewCam = viewport->GetViewCamera();

        if (inode == viewCam)
        {
            // this camera is viewport camera
        }
        intf->ReleaseViewport(viewport);
    }
}