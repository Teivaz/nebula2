//----------------------------------------------------------------------------
// (c) 2006    Vadim Macagon
//----------------------------------------------------------------------------
#include "lwexporter/nlwshadereditor.h"
#include "lwwrapper/nlwglobals.h"
#include "lwexporter/nlwshaderpanel.h"
#include "lwexporter/nlwshaderpanelfactory.h"
#include "lwexporter/nlwshaderexportsettings.h"
#include "lwexporter/nlwsettingsregistry.h"
#include "lwwrapper/nlwcallbacktemplates.h"
#include "lwexporter/nlwexportermaster.h"
#include "lwwrapper/nlwsavestate.h"
#include "lwwrapper/nlwloadstate.h"

const char nLWShaderEditor::HANDLER_NAME[] = "N2_ExporterShaderEditor";

//----------------------------------------------------------------------------
/**
    Block ids used to save and load plugin data in the Lightwave file.
*/
namespace FileBlockId
{
    // top level block
    const LWBlockIdent SHADER       = { LWID_('S','H','D','R'), "Shader" };
    const LWBlockIdent SHADER_PARAM = { LWID_('P','A','R','M'), "Param" };
}

//----------------------------------------------------------------------------
/**
*/
nLWShaderEditor::nLWShaderEditor(LWSurfaceID surfId) :
    surfaceId(surfId),
    shaderSettings(0)
{
    nLWExporterMaster::PluginCreated();

    // make the shader settings we store accessible to other plugins by
    // sticking them in the settings registry
    this->shaderSettings = n_new(nLWShaderExportSettings);
    n_assert(this->shaderSettings);
    if (this->shaderSettings)
    {
        nLWSettingsRegistry* registry = nLWSettingsRegistry::Instance();
        n_assert(registry);
        if (registry)
        {
            registry->Add(this->surfaceId, this->shaderSettings);
        }
    }
}

//----------------------------------------------------------------------------
/**
*/
nLWShaderEditor::~nLWShaderEditor()
{
    if (this->shaderSettings)
    {
        nLWSettingsRegistry* registry = nLWSettingsRegistry::Instance();
        n_assert(registry);
        if (registry)
        {
            registry->Remove(this->surfaceId);
        }
        n_delete(this->shaderSettings);
        this->shaderSettings = 0;
    }

    nLWExporterMaster::PluginDestroyed();
}

//----------------------------------------------------------------------------
/**
    @brief Handler callback. Allocate and initialize instance data.
*/
XCALL_(LWInstance) 
nLWShaderEditor::OnCreate(void* priv, void* context, LWError* error)
{
    if (!priv)
    {
        *error = "GlobalFunc pointer not supplied to nLWExporterMaster::OnCreate()";
        return 0;
    }

    LWSurfaceID lwSurfId = (LWSurfaceID)context;
    return n_new(nLWShaderEditor(lwSurfId));
}

//----------------------------------------------------------------------------
/**
    @brief Handler callback. Copy instance data.
*/
XCALL_(LWError) 
nLWShaderEditor::OnCopy(LWInstance dest, LWInstance source)
{
    // TODO: Copy all plug-in data.
    nLWShaderEditor* destShader = (nLWShaderEditor*)dest;
    nLWShaderEditor* sourceShader = (nLWShaderEditor*)source;
    if (destShader->surfaceId == sourceShader->surfaceId)
    {
        return "Only one Nebula 2 Shader can be assigned per surface!";
    }
    return 0;
}

//----------------------------------------------------------------------------
/**
    @brief Handler callback. Read instance data.
    
    Shader instance data is stored in the SURF chunks of object files, 
    but it isn't necessary to know that to read the data.
*/
LWError
nLWShaderEditor::OnLoad(const LWLoadState* loadState)
{
    if (!this->shaderSettings)
        return 0;

    this->shaderSettings->Clear();
    
    nLWLoadState state(loadState);
    if (state.BeginBlock(FileBlockId::SHADER))
    {
        nString tempStr;
        
        if (state.ReadString(tempStr))
        {
            this->shaderSettings->SetShaderName(tempStr);
        }
        
        if (state.ReadString(tempStr))
        {
            this->shaderSettings->SetShaderFile(tempStr);
        }

        while (state.BeginBlock(FileBlockId::SHADER_PARAM))
        {
            // param name
            nShaderState::Param param = nShaderState::InvalidParameter;
            nString paramNameStr;
            if (state.ReadString(paramNameStr))
            {
                param = nShaderState::StringToParam(paramNameStr.Get());
            }
            // param type
            nShaderState::Type paramType = nShaderState::InvalidType;
            nString paramTypeStr;
            if (state.ReadString(paramTypeStr))
            {
                paramType = nShaderState::StringToType(paramTypeStr.Get());
            }
            // param value
            switch (paramType)
            {
                case nShaderState::Bool:
                {
                    bool val = false;
                    if (state.ReadBool(val))
                    {
                        this->shaderSettings->SetArg(param, val);
                    }
                    break;
                }

                case nShaderState::Int:
                {
                    int val = 0;
                    if (state.ReadInt(val))
                    {
                        this->shaderSettings->SetArg(param, val);
                    }
                    break;
                }
                
                case nShaderState::Float:
                {
                    float val = 0.0f;
                    if (state.ReadFloat(val))
                    {
                        this->shaderSettings->SetArg(param, val);
                    }
                    break;
                }

                case nShaderState::Float4:
                {
                    nFloat4 val = { 0.0f };
                    if (state.ReadFloat4(val))
                    {
                        this->shaderSettings->SetArg(param, val);
                    }
                    break;
                }

                case nShaderState::Texture:
                {
                    if (state.ReadString(tempStr))
                    {
                        this->shaderSettings->SetTexture(param, tempStr);
                    }
                    break;
                }

                default:
                    n_assert2(false, "Unhandled shader arg type!");
                    break;
            }
            state.EndBlock(/*FileBlockId::SHADER_PARAM*/);
        }
        state.EndBlock(/*FileBlockId::SHADER*/);
    }

    // no errors
    return 0;
}

//----------------------------------------------------------------------------
/**
    @brief Handler callback. Write instance data.
    
    Shader instance data is stored in the SURF chunks of object files, 
    but it isn't necessary to know that to write the data.
*/
LWError
nLWShaderEditor::OnSave(const LWSaveState* saveState)
{
    if (!this->shaderSettings)
        return 0;
    
    if (!this->shaderSettings->Valid())
        return 0;

    static const nString boolTypeStr(nShaderState::TypeToString(nShaderState::Bool));
    static const nString intTypeStr(nShaderState::TypeToString(nShaderState::Int));
    static const nString floatTypeStr(nShaderState::TypeToString(nShaderState::Float));
    static const nString float4TypeStr(nShaderState::TypeToString(nShaderState::Float4));
    static const nString textureTypeStr(nShaderState::TypeToString(nShaderState::Texture));

    nLWSaveState state(saveState);
    state.BeginBlock(FileBlockId::SHADER, false);
    state.WriteString(this->shaderSettings->GetShaderName());
    state.WriteString(this->shaderSettings->GetShaderFile());

    // non-texture params
    for (int i = 0; i < this->shaderSettings->GetNumArgs(); i++)
    {
        state.BeginBlock(FileBlockId::SHADER_PARAM, true);
        // param name
        nShaderState::Param param = this->shaderSettings->GetParamAt(i);
        nString paramStr = nShaderState::ParamToString(param);
        state.WriteString(paramStr);
        // param type and value
        const nShaderArg& arg = this->shaderSettings->GetArgAt(i);
        switch (arg.GetType())
        {
            case nShaderState::Bool:
                state.WriteString(boolTypeStr);
                state.WriteBool(arg.GetBool());
                break;

            case nShaderState::Int:
                state.WriteString(intTypeStr);
                state.WriteInt(arg.GetInt());
                break;

            case nShaderState::Float:
                state.WriteString(floatTypeStr);
                state.WriteFloat(arg.GetFloat());
                break;

            case nShaderState::Float4:
                state.WriteString(float4TypeStr);
                state.WriteFloat4(arg.GetFloat4());
                break;

            default:
                n_assert2(false, "Unhandled shader arg type!");
                break;
        }
        state.EndBlock(/*FileBlockId::SHADER_PARAM*/);
    }
    
    // texture params
    for (int i = 0; i < this->shaderSettings->GetNumTextures(); i++)
    {
        state.BeginBlock(FileBlockId::SHADER_PARAM, true);
        // param name
        nShaderState::Param param = this->shaderSettings->GetTextureParamAt(i);
        nString paramStr = nShaderState::ParamToString(param);
        state.WriteString(paramStr);
        // param type
        state.WriteString(textureTypeStr);
        // param value
        state.WriteString(this->shaderSettings->GetTextureAt(i));
        state.EndBlock(/*FileBlockId::SHADER_PARAM*/);
    }
    
    state.EndBlock(/*FileBlockId::SHADER*/);
    
    // no errors;
    return 0;
}

//----------------------------------------------------------------------------
/**
    @brief Handler callback. Get a one-line text description of the instance.

    The string will persist after this method is called (until it's called
    again).
*/
const char*
nLWShaderEditor::OnGetDescription()
{
    // should return the name of the shader
    this->description = "Nebula 2 - ";
    if (this->surfaceId)
    {
        nLWSettingsRegistry* registry = nLWSettingsRegistry::Instance();
        n_assert(registry);
        if (registry)
        {
            nLWShaderExportSettings* settings = registry->Get(this->surfaceId);
            this->description += settings->GetShaderName();
        }
    }
    return this->description.Get();
}

//----------------------------------------------------------------------------
/**
    @brief Handler callback. Called at the start of rendering.
*/
LWError
nLWShaderEditor::OnBeginRender(int /*renderMode*/)
{
    // nothing to do
    // no errors
    return 0;    
}

//----------------------------------------------------------------------------
/**
    @brief Handler callback. Called at the start of each sampling pass.
*/
LWError
nLWShaderEditor::OnRenderFrame(LWFrame /*frame*/, LWTime /*time*/)
{
    // nothing to do
    // no errors
    return 0;
}

//----------------------------------------------------------------------------
/**
    @brief Handler callback. Called at the end of rendering.
*/
void
nLWShaderEditor::OnEndRender()
{
    // nothing to do
    return;
}

//----------------------------------------------------------------------------
/**
    @brief Handler callback. Called to draw a single pixel.

    The evaluation callback is called for every visible spot on a surface 
    and is passed a shader access structure describing the spot to be shaded.
*/
void
nLWShaderEditor::OnEvaluate(LWShaderAccess* /*access*/)
{
    // nothing to do since we don't render anything
    return;
}

//----------------------------------------------------------------------------
/**
    @brief Handler callback.
*/
unsigned int
nLWShaderEditor::OnFlags()
{
    // just return something, we don't render anything anyway
    return LWSHF_COLOR;
}

//----------------------------------------------------------------------------
/**
    @brief Handler callback. Called to display the user interface.
*/
LWError
nLWShaderEditor::OnDisplayUI()
{
    nLWShaderPanelFactory* factory = nLWShaderPanelFactory::Instance();
    n_assert(factory);
    if (!factory)
        return "Failed to obtain Shader Panel Factory instance!";

    factory->DisplayPanel(this->surfaceId);

    // no errors
    return 0;
}

//----------------------------------------------------------------------------
/**
*/
XCALL_(int) 
nLWShaderEditor::Activate_Handler(long version, 
                                  GlobalFunc* global,
                                  LWShaderHandler* local, 
                                  void* /*serverData*/)
{
    if (version != LWSHADER_VERSION) 
        return AFUNC_BADVERSION;

    if (!local)
        return AFUNC_BADLOCAL;

    if (!local->inst)
        return AFUNC_BADLOCAL;

    if (!nLWGlobals::IsGlobalFuncSet())
        nLWGlobals::SetGlobalFunc(global);

    // right now we can only run in Layout
    if (!nLWGlobals::SystemID::IsLayout())
        return AFUNC_BADAPP;

    if (!nLWExporterMaster::Instance())
        return AFUNC_BADAPP_SILENT;

    local->inst->priv = global;
    local->inst->create = nLWShaderEditor::OnCreate;
    local->inst->destroy = nLWCallbacks::Instance::Destroy<nLWShaderEditor>;
    local->inst->copy = nLWShaderEditor::OnCopy;
    local->inst->load = nLWCallbacks::Instance::Load<nLWShaderEditor>;
    local->inst->save = nLWCallbacks::Instance::Save<nLWShaderEditor>;
    local->inst->descln = nLWCallbacks::Instance::GetDescription<nLWShaderEditor>;

    if (local->item) // will be NULL when called from Modeler
    {
        local->item->useItems = 0;
        local->item->changeID = 0;
    }

    if (!local->rend)
        return AFUNC_BADLOCAL;

    local->rend->init = nLWCallbacks::Render::BeginRender<nLWShaderEditor>;
    local->rend->cleanup = nLWCallbacks::Render::EndRender<nLWShaderEditor>;
    local->rend->newTime = nLWCallbacks::Render::RenderFrame<nLWShaderEditor>;

    local->evaluate = nLWCallbacks::ShaderHandler::Evaluate<nLWShaderEditor>;
    local->flags = nLWCallbacks::ShaderHandler::Flags<nLWShaderEditor>;

    return AFUNC_OK;
}

//----------------------------------------------------------------------------
/**
*/
XCALL_(int) 
nLWShaderEditor::Activate_Interface(long version, 
                                    GlobalFunc* global,
                                    LWInterface* local, 
                                    void* /*serverData*/)
{
    if (version != LWINTERFACE_VERSION) 
        return AFUNC_BADVERSION;

    if (!local)
        return AFUNC_BADLOCAL;

    // right now we can only run in Layout
    if (!nLWGlobals::IsGlobalFuncSet())
        nLWGlobals::SetGlobalFunc(global);

    if (!nLWGlobals::SystemID::IsLayout())
        return AFUNC_BADAPP;

    if (!nLWExporterMaster::Instance())
        return AFUNC_BADAPP_SILENT;

    local->panel = 0;
    local->options = nLWCallbacks::Interface::DisplayUI<nLWShaderEditor>;
    local->command = 0;

    return AFUNC_OK;
}

//----------------------------------------------------------------------------
// EOF
//----------------------------------------------------------------------------
