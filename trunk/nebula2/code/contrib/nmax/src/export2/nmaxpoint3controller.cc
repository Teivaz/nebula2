//-----------------------------------------------------------------------------
//
//
//
//-----------------------------------------------------------------------------
#include "export2/nmax.h"
#include "export2/nmaxcontroller.h"
#include "export2/nmaxpoint3controller.h"
#include "export2/nmaxinterface.h"
#include "pluginlibs/nmaxdlg.h"
#include "pluginlibs/nmaxlogdlg.h"

//-----------------------------------------------------------------------------
/**
*/
nMaxPoint3Controller::nMaxPoint3Controller()
{
}

//-----------------------------------------------------------------------------
/**
*/
nMaxPoint3Controller::~nMaxPoint3Controller()
{
}

//-----------------------------------------------------------------------------
/**
*/
void nMaxPoint3Controller::Export(Control *control)
{
    if (control)
    {
        IKeyControl* keyControl = GetKeyControlInterface(control);

        if (keyControl)
        {

            if (0 == keyControl->GetNumKeys())
            {
                //n_maxlog(Error, "%s node's key control does not have any keys.", inode->GetName());
                return;
            }

            switch(GetType(control))
            {
            case TCBPoint3:
                ExportTCBPoint3(control);
                break;
            case HybridPoint3:
                ExportHybridPoint3(control);
                break;
            default:
                ExportDefaultPoint3(control);
                break;
            }
        }
        else
        {
            ExportDefaultPoint3(control);
        }
    }
    else
    {
        ExportDefaultPoint3(control);
    }
}

//-----------------------------------------------------------------------------
/**
*/
void nMaxPoint3Controller::ExportTCBPoint3(Control* control)
{
    n_maxlog(Error, "TCB Point3 controller is not supported.");
}

//-----------------------------------------------------------------------------
/**
*/
void nMaxPoint3Controller::ExportHybridPoint3(Control* control)
{
    n_maxlog(Error, "Hybrid Point3 controller is not supported.");
}

//-----------------------------------------------------------------------------
/**
*/
void nMaxPoint3Controller::ExportDefaultPoint3(Control* control)
{
    int numFrames = nMaxInterface::Instance()->GetNumFrames();
    if (0 == numFrames)
    {
        n_maxlog(Error, "ExportDefaultPoint3: Number of frames 0.");
        return;
    }

    TimeValue animStart = nMaxInterface::Instance()->GetAnimStartTime();

    int i;
    TimeValue t;
    Point3 value;
    Interval valid;

    // retrieves sample keys then add it to array.
    for (i=0, t=animStart; i<numFrames; i++, t+=GetTicksPerFrame())
    {
        control->GetValue(t, &value, valid);

        SampleKey sample;

        sample.time = (t * SECONDSPERTICK);
        sample.key.set(value.x, value.y, value.z);

        this->keyArray.Append(sample);
    }
}