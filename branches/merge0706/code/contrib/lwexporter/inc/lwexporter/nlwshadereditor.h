#ifndef N_LW_SHADER_EDITOR_H
#define N_LW_SHADER_EDITOR_H
//----------------------------------------------------------------------------
extern "C" 
{
#include <lwsdk/lwserver.h>
#include <lwsdk/lwhandler.h>
#include <lwsdk/lwshader.h>
#include <lwsdk/lwsurf.h>
}

#include "kernel/ntypes.h"
#include "util/nstring.h"

class nLWShaderSelectionPanel;
class nLWShaderExportSettings;

//----------------------------------------------------------------------------
/**
    @class nLWShaderEditor
    @brief A ShaderHandler Lightwave plugin that associates a Nebula 2 shader
           with a Lightwave surface.
    
    Once an instance of this plugin is associated with a surface the user can
    pick the actual Nebula 2 shader to use for the surface and set the shader
    parameters via the UI. Note that Lightwave stores the data for 
    ShaderHandler plugins in the object file, not the scene file!
*/
class nLWShaderEditor
{
public:
    nLWShaderEditor(LWSurfaceID);
    ~nLWShaderEditor();

    // these need to be public so the callback template functions can call them
    LWError OnLoad(const LWLoadState* loadState);
    LWError OnSave(const LWSaveState* saveState);
    const char* OnGetDescription();
    LWError OnBeginRender(int renderMode);
    LWError OnRenderFrame(LWFrame frame, LWTime time);
    void OnEndRender();
    void OnEvaluate(LWShaderAccess* access);
    unsigned int OnFlags();
    LWError OnDisplayUI();

    XCALL_(static int) Activate_Handler(long version, 
                                        GlobalFunc* global,
                                        LWShaderHandler* local, 
                                        void* serverData);

    XCALL_(static int) Activate_Interface(long version, 
                                          GlobalFunc* global,
                                          LWInterface* local, 
                                          void* serverData);

    static const char HANDLER_NAME[];

private:
    XCALL_(static LWInstance) OnCreate(void* priv, void* context, LWError* error);
    XCALL_(static LWError) OnCopy(LWInstance dest, LWInstance source);

    LWSurfaceID surfaceId;
    nLWShaderExportSettings* shaderSettings;
    nString description;
};

//----------------------------------------------------------------------------
#endif // N_LW_SHADER_EDITOR_H
