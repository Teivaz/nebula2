#ifndef N_LW_CALLBACK_TEMPLATES_H
#define N_LW_CALLBACK_TEMPLATES_H
//----------------------------------------------------------------------------
/**
    This file contains a bunch of function templates that can be used to
    reduce the hassle of mapping Lightwave C-callbacks to C++ class methods.
*/

extern "C" 
{
#include <lwsdk/lwserver.h>
#include <lwsdk/lwmeshes.h>
#include <lwsdk/lwio.h>
#include <lwsdk/lwtypes.h>
#include <lwsdk/lwmaster.h>
#include <lwsdk/lwshader.h>
#include <lwsdk/lwcustobj.h>
}

#include "kernel/ntypes.h"

namespace nLWCallbacks
{
    //----------------------------------------------------------------------------
    /**
    */
    namespace MeshInfo
    {
        template<class T>
        XCALL_(int) PointScan(void* boxedObject, LWPntID pntId)
        {
            n_assert(boxedObject);
            if (!boxedObject)
                return 0;

            return ((T*)boxedObject)->OnPointScan(pntId);
        }

        template<class T>
        XCALL_(int) PolygonScan(void* boxedObject, LWPolID polId)
        {
            n_assert(boxedObject);
            if (!boxedObject)
                return 0;

            return ((T*)boxedObject)->OnPolygonScan(polId);
        }
    }

    //----------------------------------------------------------------------------
    /**
    */
    namespace Instance
    {
        template<class T>
        XCALL_(void) Destroy(LWInstance instance)
        {
            n_delete((T*)instance);
        }

        template<class T>
        XCALL_(LWError) Load(LWInstance instance, const LWLoadState* state)
        {
            n_assert(instance);
            if (!instance)
                return "NULL instance!";

            return ((T*)instance)->OnLoad(state);
        }

        template<class T>
        XCALL_(LWError) Save(LWInstance instance, const LWSaveState* state)
        {
            n_assert(instance);
            if (!instance)
                return "NULL instance!";

            return ((T*)instance)->OnSave(state);
        }

        template<class T>
        XCALL_(const char*) GetDescription(LWInstance instance)
        {
            n_assert(instance);
            if (!instance)
                return "NULL instance!";

            return ((T*)instance)->OnGetDescription();
        }
    }

    //----------------------------------------------------------------------------
    /**
    */
    namespace Item
    {
        template<class T>
        XCALL_(const LWItemID*) UseItems(LWInstance instance)
        {
            n_assert(instance);
            if (!instance)
                return 0;

            return ((T*)instance)->OnUseItems();
        }

        template<class T>
        XCALL_(void) ChangeID(LWInstance instance, const LWItemID* idList)
        {
            n_assert(instance);
            if (!instance)
                return;

            ((T*)instance)->OnChangeID(idList);
        }
    }

    //----------------------------------------------------------------------------
    /**
    */
    namespace Interface
    {
        template<class T>
        XCALL_(LWError) DisplayUI(LWInstance instance)
        {
            n_assert(instance);
            if (!instance)
                return "NULL instance!";

            return ((T*)instance)->OnDisplayUI();
        }
    }

    //----------------------------------------------------------------------------
    /**
    */
    namespace Render
    {
        template<class T>
        XCALL_(LWError) BeginRender(LWInstance instance, int renderMode)
        {
            n_assert(instance);
            if (!instance)
                return "NULL instance!";

            return ((T*)instance)->OnBeginRender(renderMode);
        }

        template<class T>
        XCALL_(LWError) RenderFrame(LWInstance instance, LWFrame frame, LWTime time)
        {
            n_assert(instance);
            if (!instance)
                return "NULL instance!";

            return ((T*)instance)->OnRenderFrame(frame, time);
        }

        template<class T>
        XCALL_(void) EndRender(LWInstance instance)
        {
            n_assert(instance);
            if (!instance)
                return;

            ((T*)instance)->OnEndRender();
        }
    }

    //----------------------------------------------------------------------------
    /**
    */
    namespace MasterHandler
    {
        template<class T>
        XCALL_(double) Event(LWInstance instance, const LWMasterAccess* access)
        {
            n_assert(instance);
            if (!instance)
                return 0.0;

            return ((T*)instance)->OnEvent(access);
        }

        template<class T>
        XCALL_(unsigned int) Flags(LWInstance instance)
        {
            n_assert(instance);
            if (!instance)
                return 0;

            return ((T*)instance)->OnFlags();
        }
    }

    //----------------------------------------------------------------------------
    /**
    */
    namespace CustomObjHandler
    {
        template<class T>
        XCALL_(void) Evaluate(LWInstance instance, const LWCustomObjAccess* access)
        {
            n_assert(instance);
            if (!instance)
                return;

            ((T*)instance)->OnEvaluate(access);
        }

        template<class T>
        XCALL_(unsigned int) Flags(LWInstance instance)
        {
            n_assert(instance);
            if (!instance)
                return 0;

            return ((T*)instance)->OnFlags();
        }
    }

    //----------------------------------------------------------------------------
    /**
    */
    namespace ShaderHandler
    {
        //----------------------------------------------------------------------------
        /**
            @brief Handler callback. Called to draw a single pixel.

            The evaluation callback is called for every visible spot on a surface 
            and is passed a shader access structure describing the spot to be shaded.
        */
        template<class T>
        XCALL_(void) Evaluate(LWInstance instance, LWShaderAccess* access)
        {
            n_assert(instance);
            if (!instance)
                return;

            ((T*)instance)->OnEvaluate(access);
        }

        //----------------------------------------------------------------------------
        /**
            @brief Handler callback.
        */
        template<class T>
        XCALL_(unsigned int) Flags(LWInstance instance)
        {
            n_assert(instance);
            if (!instance)
                return 0;

            return ((T*)instance)->OnFlags();
        }
    }

} // namespace nLWCallbacks

//----------------------------------------------------------------------------
#endif // N_LW_CALLBACK_TEMPLATES_H
