//-----------------------------------------------------------------------------
//
//
//
//-----------------------------------------------------------------------------
#include "export2/nmax.h"
#include "export2/nmaxcontroller.h"
#include "export2/nmaxfloatcontroller.h"
#include "export2/nmaxinterface.h"
#include "pluginlibs/nmaxdlg.h"
#include "pluginlibs/nmaxlogdlg.h"

//-----------------------------------------------------------------------------
/**
*/
nMaxFloatController::nMaxFloatController()
{
}

//-----------------------------------------------------------------------------
/**
*/
nMaxFloatController::~nMaxFloatController()
{
}

//-----------------------------------------------------------------------------
/**
*/
void nMaxFloatController::Export(Control *control)
{
    if (control)
    {
        IKeyControl* keyControl = GetKeyControlInterface(control);

        if (keyControl)
        {
            if (0 == keyControl->GetNumKeys())
            {
                return;
            }

            switch(GetType(control))
            {
            case TCBFloat:
                ExportTCBFloat(keyControl);
                break;
            case LinearFloat:
                ExportLinearFloat(keyControl);
                break;
            case HybridFloat:
                ExportHybridFloat(keyControl);
                break;
            default:
                ExportDefaultFloat(control);
                break;
            }
        }
        else
        {
            ExportDefaultFloat(control);
        }
    }
    else
    {
        ExportDefaultFloat(control);
    }

}

//-----------------------------------------------------------------------------
/**
*/
void nMaxFloatController::ExportTCBFloat(IKeyControl* keyControl)
{
    n_maxlog(Error, "TCB Float controller is not supported");
}

//-----------------------------------------------------------------------------
/**
*/
void nMaxFloatController::ExportHybridFloat(IKeyControl* keyControl)
{
    n_maxlog(Error, "Hybrid Float controller is not supported");
}

//-----------------------------------------------------------------------------
/**
*/
void nMaxFloatController::ExportLinearFloat(IKeyControl* keyControl)
{
    int numKeys = keyControl->GetNumKeys();

    for (int i=0; i<numKeys; i++)
    {
        ILinFloatKey key;
        keyControl->GetKey(i, &key);

        SampleKey sample;

        sample.time = (key.time * SECONDSPERTICK);
        sample.key  = key.val;

        this->keyArray.Append(sample);
    }
}

//-----------------------------------------------------------------------------
/**
*/
void nMaxFloatController::ExportDefaultFloat(Control* control)
{
    int numFrames = nMaxInterface::Instance()->GetNumFrames();
    if (0 == numFrames)
    {
        n_maxlog(Error, "ExportDefaultFloat: Number of frames 0.");
        return;
    }

    TimeValue animStart = nMaxInterface::Instance()->GetAnimStartTime();

    int i;
    TimeValue t;
    Interval valid;

    for (i=0, t=animStart; i<numFrames; i++, t+=GetTicksPerFrame())
    {
        float value;
        control->GetValue(t, (void*)&value, valid, CTRL_ABSOLUTE);

        SampleKey sample;

        sample.time = t*SECONDSPERTICK;
        sample.key = value;

        this->keyArray.Append(sample);
    }
}
