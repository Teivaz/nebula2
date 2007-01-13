//------------------------------------------------------------------------------
//  nglslshader_state.cc
//
//  (C) 2006 Oleg Khryptul (Haron)
//------------------------------------------------------------------------------

#include "opengl/nglslshader.h"

#include "opengl/nglserver2.h"
#include "opengl/npassstate.h"
#include "opengl/nglextensionserver.h"

#define nGLSL_STATE_NOT_IMPLEMENTED n_printf("nGLSLShader::ParsePassParam(): WARNING! There is no implementation for parameter <%s>.\n", name)
#define nGL_ENABLE_DISABLE(param) if (arg.GetBool()) glEnable(param); else glDisable(param);

//------------------------------------------------------------------------------
/**
*/
bool
GetGLShadeMode(const nString& val, GLenum& func)
{
    if      (val == nString("smooth")) func = GL_SMOOTH;
    else if (val == nString("flat"))   func = GL_FLAT;
    else return false;
    return true;
}

//------------------------------------------------------------------------------
/**
*/
bool
GetGLPolygonMode(const nString& val, GLenum& func)
{
    if      (val == nString("point")) func = GL_POINT;
    else if (val == nString("line"))  func = GL_LINE;
    else if (val == nString("fill"))  func = GL_FILL;
    else return false;
    return true;
}

//------------------------------------------------------------------------------
/**
*/
bool
GetGLFogMode(const nString& val, GLenum& func)
{
    //if      (val == nString("none"))   func = GL_NONE; // ???
    if      (val == nString("exp"))    func = GL_EXP;
    else if (val == nString("exp2"))   func = GL_EXP2;
    else if (val == nString("linear")) func = GL_LINEAR;
    else return false;
    return true;
}

//------------------------------------------------------------------------------
/**
*/
bool
GetGLFunc(const nString& val, GLenum& func)
{
    if      (val == nString("never"))        func = GL_NEVER;
    else if (val == nString("always"))       func = GL_ALWAYS;
    else if (val == nString("less"))         func = GL_LESS;
    else if (val == nString("lessequal"))    func = GL_LEQUAL;
    else if (val == nString("equal"))        func = GL_EQUAL;
    else if (val == nString("greaterequal")) func = GL_GEQUAL;
    else if (val == nString("greater"))      func = GL_GREATER;
    else if (val == nString("notequal"))     func = GL_NOTEQUAL;
    else return false;
    return true;
}

//------------------------------------------------------------------------------
/**
*/
bool
GetGLStencilOp(const nString& val, GLenum& op)
{
    if      (val == nString("keep"))         op = GL_KEEP;
    else if (val == nString("zero"))         op = GL_ZERO;
    else if (val == nString("replace"))      op = GL_REPLACE;
    else if (val == nString("incr"))         op = GL_INCR;
    else if (val == nString("decr"))         op = GL_DECR;
    else if (val == nString("invert"))       op = GL_INVERT;
    else if (val == nString("incrwrap"))     op = GL_INCR_WRAP;
    else if (val == nString("decrwrap"))     op = GL_DECR_WRAP;
    else return false;
    return true;
}

//------------------------------------------------------------------------------
/**
*/
bool
GetGLBlendMode(const nString& val, GLenum& op)
{
    if      (val == nString("add"))         op = GL_FUNC_ADD;
    else if (val == nString("subtract"))    op = GL_FUNC_SUBTRACT;
    else if (val == nString("rsubtract"))   op = GL_FUNC_REVERSE_SUBTRACT;
    else if (val == nString("min"))         op = GL_MIN;
    else if (val == nString("max"))         op = GL_MAX;
    else return false;
    return true;
}

//------------------------------------------------------------------------------
/**
*/
bool
GetGLBlendFunc(const nString& val, GLenum& op)
{
    if      (val == nString("zero"))             op = GL_ZERO;
    else if (val == nString("one"))              op = GL_ONE;
    else if (val == nString("srccolor"))         op = GL_SRC_COLOR;
    else if (val == nString("invsrccolor"))      op = GL_ONE_MINUS_SRC_COLOR;
    else if (val == nString("dstcolor"))         op = GL_DST_COLOR;
    else if (val == nString("invdstcolor"))      op = GL_ONE_MINUS_DST_COLOR;
    else if (val == nString("srcalpha"))         op = GL_SRC_ALPHA;
    else if (val == nString("invsrcalpha"))      op = GL_ONE_MINUS_SRC_ALPHA;
    else if (val == nString("dstalpha"))         op = GL_DST_ALPHA;
    else if (val == nString("invdstalpha"))      op = GL_ONE_MINUS_DST_ALPHA;
    else if (val == nString("constantcolor"))    op = GL_CONSTANT_COLOR;
    else if (val == nString("invconstantcolor")) op = GL_ONE_MINUS_CONSTANT_COLOR;
    else if (val == nString("constantalpha"))    op = GL_CONSTANT_ALPHA;
    else if (val == nString("invconstantalpha")) op = GL_ONE_MINUS_CONSTANT_ALPHA;
    else if (val == nString("srcalphasaturate")) op = GL_SRC_ALPHA_SATURATE;
    else return false;
    return true;
}

#define DECL_PASS_PARAM(name, type) void name##_Func(int, const nShaderArg&);
#include "opengl/npassparams.h"
#undef DECL_PASS_PARAM

/// the technique parameter functions string table
void (*nGLSLShader::passParamFunc[nPassState::NumParameters])(int, const nShaderArg&) =
{
#define DECL_PASS_PARAM(name, type) name##_Func,
#include "opengl/npassparams.h"
#undef DECL_PASS_PARAM
};

//------------------------------------------------------------------------------
/**
*/
bool
nGLSLShader::ParsePassParam(const char* name, int index, nString& val)
{
    nShaderArg arg;

    int i, n;
    bool warning;
    nArray<nString> subvalues;

    nFloat4 f4;
    matrix44 m44;
    float* float_ptr;

    GLenum func, op, mode;

    // TODO: replace these default values by glGet
    GLint  stencilFuncRef  = 0;
    GLuint stencilFuncMask = 1;

    GLenum stencilFailOp  = GL_KEEP;
    GLenum stencilZFailOp = GL_KEEP;
    GLenum stencilZPassOp = GL_KEEP;

    nPassState::Param passParam = nPassState::StringToParam(name);
    nShaderState::Type paramType = nPassState::ParamToType(passParam);

    val.ToLower();
    switch (passParam)
    {
        // Special cases
#pragma region Light States

    case nPassState::LightPhi:      nGLSL_STATE_NOT_IMPLEMENTED; return false;

    case nPassState::LightTheta:    nGLSL_STATE_NOT_IMPLEMENTED; return false;
    case nPassState::LightType:     nGLSL_STATE_NOT_IMPLEMENTED; return false;
#pragma endregion

#pragma region Material States
#pragma endregion

#pragma region Render States
    //---------------------------------------------------------------------------------------------------
    case nPassState::AlphaFunc:
        if (!GetGLFunc(val, func))
        {
            //glGetIntegerv(GL_ALPHA_TEST_FUNC, &func);
            n_message("nGLSLShader::ParsePassParam(): WARNING! Unknown value (%s) of the parameter AlphaFunc. Parameter skiped.\n", val.Get());
            return false;
        }
        arg.SetInt(func);
        break;

    case nPassState::BlendOp:
        if (!GetGLBlendMode(val, mode))
        {
            //glGetIntegerv(GL_BLEND_EQUATION, &mode);
            n_message("nGLSLShader::ParsePassParam(): WARNING! Unknown value (%s) of the parameter BlendOp. Parameter skiped.\n", val.Get());
            return false;
        }
        arg.SetInt(mode);
        break;

    case nPassState::ColorWriteEnable:    nGLSL_STATE_NOT_IMPLEMENTED; return false;
    case nPassState::DepthBias:           nGLSL_STATE_NOT_IMPLEMENTED; return false;

    case nPassState::DestBlend:
        if (!GetGLBlendFunc(val, func))
        {
            //glGetIntegerv(GL_BLEND_DST, &func);
            n_message("nGLSLShader::ParsePassParam(): WARNING! Unknown value (%s) of the parameter DestBlend. Parameter skiped.\n", val.Get());
            return false;
        }
        arg.SetInt(func);
        break;

    case nPassState::FillMode:
        if (!GetGLPolygonMode(val, mode))
        {
            //glGetIntegerv(GL_SHADE_MODEL, &mode);
            n_message("nGLSLShader::ParsePassParam(): WARNING! Unknown value (%s) of the parameter ShadeMode. Parameter skiped.\n", val.Get());
            return false;
        }
        arg.SetInt(mode);
        break;

    case nPassState::LastPixel:           nGLSL_STATE_NOT_IMPLEMENTED; return false;

    case nPassState::ShadeMode:
        if (!GetGLShadeMode(val, mode))
        {
            //glGetIntegerv(GL_SHADE_MODEL, &mode);
            n_message("nGLSLShader::ParsePassParam(): WARNING! Unknown value (%s) of the parameter ShadeMode. Parameter skiped.\n", val.Get());
            return false;
        }
        arg.SetInt(mode);
        break;

    case nPassState::SlopeScaleDepthBias: nGLSL_STATE_NOT_IMPLEMENTED; return false;

    case nPassState::SrcBlend:
        if (!GetGLBlendFunc(val, func))
        {
            //glGetIntegerv(GL_BLEND_SRC, &func);
            n_message("nGLSLShader::ParsePassParam(): WARNING! Unknown value (%s) of the parameter SrcBlend. Parameter skiped.\n", val.Get());
            return false;
        }
        arg.SetInt(func);
        break;

    case nPassState::StencilFail:
        if (!GetGLStencilOp(val, op))
        {
            //glGetIntegerv(GL_STENCIL_FAIL, &op);
            n_message("nGLSLShader::ParsePassParam(): WARNING! Unknown value (%s) of the parameter StencilFail. Parameter skiped.\n", val.Get());
            return false;
        }
        arg.SetInt(op);
        break;

    //---------------------------------------------------------------------------------------------------
    case nPassState::StencilFunc:
        if (!GetGLFunc(val, func))
        {
            //glGetIntegerv(GL_STENCIL_FUNC, &func);
            n_message("nGLSLShader::ParsePassParam(): WARNING! Unknown value (%s) of the parameter StencilFunc. Parameter skiped.\n", val.Get());
            return false;
        }
        arg.SetInt(func);
        break;

    //---------------------------------------------------------------------------------------------------
    case nPassState::StencilPass:
        if (!GetGLStencilOp(val, op))
        {
            //glGetIntegerv(GL_STENCIL_PASS_DEPTH_PASS, &op);
            n_message("nGLSLShader::ParsePassParam(): WARNING! Unknown value (%s) of the parameter StencilPass. Parameter skiped.\n", val.Get());
            return false;
        }
        arg.SetInt(op);
        break;

    case nPassState::StencilWriteMask: nGLSL_STATE_NOT_IMPLEMENTED; return false;

    //---------------------------------------------------------------------------------------------------
    case nPassState::StencilZFail:
        if (!GetGLStencilOp(val, op))
        {
            //glGetIntegerv(GL_STENCIL_PASS_DEPTH_FAIL, &op);
            n_message("nGLSLShader::ParsePassParam(): WARNING! Unknown value (%s) of the parameter StencilZFail. Parameter skiped.\n", val.Get());
            return false;
        }
        arg.SetInt(op);
        break;

    case nPassState::TextureFactor: nGLSL_STATE_NOT_IMPLEMENTED; return false;
    case nPassState::Wrap:          nGLSL_STATE_NOT_IMPLEMENTED; return false;

    //---------------------------------------------------------------------------------------------------
    case nPassState::ZFunc:
        if (!GetGLFunc(val, func))
        {
            //glGetIntegerv(GL_DEPTH_FUNC, &func);
            n_message("nGLSLShader::ParsePassParam(): WARNING! Unknown value (%s) of the parameter ZFunc. Parameter skiped.\n", val.Get());
            return false;
        }
        arg.SetInt(func);
        break;
#pragma endregion

#pragma region Vertex Pipe Render States
    case nPassState::Ambient:               nGLSL_STATE_NOT_IMPLEMENTED; return false;
    case nPassState::AmbientMaterialSource: nGLSL_STATE_NOT_IMPLEMENTED; return false;
    case nPassState::Clipping:              nGLSL_STATE_NOT_IMPLEMENTED; return false;

    case nPassState::ColorVertex:           nGLSL_STATE_NOT_IMPLEMENTED; return false;

    //---------------------------------------------------------------------------------------------------
    case nPassState::CullMode:
        if (val == nString("none"))
        {
            mode = 0;
        }
        else
        {
            if (val == nString("cw"))
            {
                mode = GL_FRONT; //mode = GL_CW;
            }
            else if (val == nString("ccw"))
            {
                mode = GL_BACK; //mode = GL_CCW;
            }
            else
            {
                //glGetIntegerv(GL_CULL_FACE_MODE, &mode);
                n_message("nGLSLShader::ParsePassParam(): WARNING! Unknown value (%s) of the parameter CullMode. Allowed values: None, Cw, Ccw. Parameter skiped.\n", val.Get());
                return false;
            }
        }
        arg.SetInt(mode);
        break;

    case nPassState::DiffuseMaterialSource:  nGLSL_STATE_NOT_IMPLEMENTED; return false;
    case nPassState::EmissiveMaterialSource: nGLSL_STATE_NOT_IMPLEMENTED; return false;

    case nPassState::FogTableMode:
        if (!GetGLFogMode(val, mode))
        {
            //glGetIntegerv(GL_FOG_MODE, &mode);
            n_message("nGLSLShader::ParsePassParam(): WARNING! Unknown value (%s) of the parameter FogTableMode. Parameter skiped.\n", val.Get());
            return false;
        }
        arg.SetInt(mode);
        break;

    case nPassState::FogVertexMode:
        if (!GetGLFogMode(val, mode))
        {
            //glGetIntegerv(GL_FOG_MODE, &mode);
            n_message("nGLSLShader::ParsePassParam(): WARNING! Unknown value (%s) of the parameter FogVertexMode. Parameter skiped.\n", val.Get());
            return false;
        }
        arg.SetInt(mode);
        break;

    case nPassState::IndexedVertexBlendEnable: nGLSL_STATE_NOT_IMPLEMENTED; return false;

    case nPassState::LocalViewer:          nGLSL_STATE_NOT_IMPLEMENTED; return false;
    case nPassState::MultiSampleAntialias: nGLSL_STATE_NOT_IMPLEMENTED; return false;
    case nPassState::MultiSampleMask:      nGLSL_STATE_NOT_IMPLEMENTED; return false;

    case nPassState::PatchSegments:        nGLSL_STATE_NOT_IMPLEMENTED; return false;
    case nPassState::PointScale_A:         nGLSL_STATE_NOT_IMPLEMENTED; return false;
    case nPassState::PointScale_B:         nGLSL_STATE_NOT_IMPLEMENTED; return false;
    case nPassState::PointScale_C:         nGLSL_STATE_NOT_IMPLEMENTED; return false;
    case nPassState::PointScaleEnable:     nGLSL_STATE_NOT_IMPLEMENTED; return false;

    case nPassState::RangeFogEnable:         nGLSL_STATE_NOT_IMPLEMENTED; return false;
    case nPassState::SpecularEnable:         nGLSL_STATE_NOT_IMPLEMENTED; return false;
    case nPassState::SpecularMaterialSource: nGLSL_STATE_NOT_IMPLEMENTED; return false;
    case nPassState::TweenFactor:            nGLSL_STATE_NOT_IMPLEMENTED; return false;
    case nPassState::VertexBlend:            nGLSL_STATE_NOT_IMPLEMENTED; return false;
#pragma endregion

#pragma region Sampler States
    case nPassState::Sampler:       nGLSL_STATE_NOT_IMPLEMENTED; return false;

    // *** Sampler Stage States
    case nPassState::AddressU:      nGLSL_STATE_NOT_IMPLEMENTED; return false;
    case nPassState::AddressV:      nGLSL_STATE_NOT_IMPLEMENTED; return false;
    case nPassState::AddressW:      nGLSL_STATE_NOT_IMPLEMENTED; return false;
    case nPassState::BorderColor:   nGLSL_STATE_NOT_IMPLEMENTED; return false;
    case nPassState::MagFilter:     nGLSL_STATE_NOT_IMPLEMENTED; return false;
    case nPassState::MaxAnisotropy: nGLSL_STATE_NOT_IMPLEMENTED; return false;
    case nPassState::MaxMipLevel:   nGLSL_STATE_NOT_IMPLEMENTED; return false;
    case nPassState::MinFilter:     nGLSL_STATE_NOT_IMPLEMENTED; return false;
    case nPassState::MipFilter:     nGLSL_STATE_NOT_IMPLEMENTED; return false;
    case nPassState::MipMapLodBias: nGLSL_STATE_NOT_IMPLEMENTED; return false;
    case nPassState::SRGBTexture:   nGLSL_STATE_NOT_IMPLEMENTED; return false;
#pragma endregion

#pragma region Texture States
    case nPassState::Texture:               nGLSL_STATE_NOT_IMPLEMENTED; return false;

    // *** Texture Stage States
    case nPassState::AlphaOp:               nGLSL_STATE_NOT_IMPLEMENTED; return false;
    case nPassState::AlphaArg0:             nGLSL_STATE_NOT_IMPLEMENTED; return false;
    case nPassState::AlphaArg1:             nGLSL_STATE_NOT_IMPLEMENTED; return false;
    case nPassState::AlphaArg2:             nGLSL_STATE_NOT_IMPLEMENTED; return false;
    case nPassState::ColorArg0:             nGLSL_STATE_NOT_IMPLEMENTED; return false;
    case nPassState::ColorArg1:             nGLSL_STATE_NOT_IMPLEMENTED; return false;
    case nPassState::ColorArg2:             nGLSL_STATE_NOT_IMPLEMENTED; return false;
    case nPassState::ColorOp:               nGLSL_STATE_NOT_IMPLEMENTED; return false;
    case nPassState::BumpEnvLScale:         nGLSL_STATE_NOT_IMPLEMENTED; return false;
    case nPassState::BumpEnvLOffset:        nGLSL_STATE_NOT_IMPLEMENTED; return false;
    case nPassState::BumpEnvMat00:          nGLSL_STATE_NOT_IMPLEMENTED; return false;
    case nPassState::BumpEnvMat01:          nGLSL_STATE_NOT_IMPLEMENTED; return false;
    case nPassState::BumpEnvMat10:          nGLSL_STATE_NOT_IMPLEMENTED; return false;
    case nPassState::BumpEnvMat11:          nGLSL_STATE_NOT_IMPLEMENTED; return false;
    case nPassState::ResultArg:             nGLSL_STATE_NOT_IMPLEMENTED; return false;
    case nPassState::TexCoordIndex:         nGLSL_STATE_NOT_IMPLEMENTED; return false;
    case nPassState::TextureTransformFlags: nGLSL_STATE_NOT_IMPLEMENTED; return false;
#pragma endregion

#pragma region Transform States
#pragma endregion

#pragma region Shader States
    case nPassState::FragmentShader:        nGLSL_STATE_NOT_IMPLEMENTED; return false;
    case nPassState::VertexShader:          nGLSL_STATE_NOT_IMPLEMENTED; return false;

    // *** Shader Constant States
    case nPassState::PixelShaderConstant:   nGLSL_STATE_NOT_IMPLEMENTED; return false;
    case nPassState::PixelShaderConstant1:  nGLSL_STATE_NOT_IMPLEMENTED; return false;
    case nPassState::PixelShaderConstant2:  nGLSL_STATE_NOT_IMPLEMENTED; return false;
    case nPassState::PixelShaderConstant3:  nGLSL_STATE_NOT_IMPLEMENTED; return false;
    case nPassState::PixelShaderConstant4:  nGLSL_STATE_NOT_IMPLEMENTED; return false;
    case nPassState::PixelShaderConstantB:  nGLSL_STATE_NOT_IMPLEMENTED; return false;
    case nPassState::PixelShaderConstantI:  nGLSL_STATE_NOT_IMPLEMENTED; return false;
    case nPassState::PixelShaderConstantF:  nGLSL_STATE_NOT_IMPLEMENTED; return false;
    case nPassState::VertexShaderConstant:  nGLSL_STATE_NOT_IMPLEMENTED; return false;
    case nPassState::VertexShaderConstant1: nGLSL_STATE_NOT_IMPLEMENTED; return false;
    case nPassState::VertexShaderConstant2: nGLSL_STATE_NOT_IMPLEMENTED; return false;
    case nPassState::VertexShaderConstant3: nGLSL_STATE_NOT_IMPLEMENTED; return false;
    case nPassState::VertexShaderConstant4: nGLSL_STATE_NOT_IMPLEMENTED; return false;
    case nPassState::VertexShaderConstantB: nGLSL_STATE_NOT_IMPLEMENTED; return false;
    case nPassState::VertexShaderConstantI: nGLSL_STATE_NOT_IMPLEMENTED; return false;
    case nPassState::VertexShaderConstantF: nGLSL_STATE_NOT_IMPLEMENTED; return false;
#pragma endregion

#pragma region Unknown
    case nPassState::FVF: nGLSL_STATE_NOT_IMPLEMENTED; return false;
#pragma endregion

    default:
        // standard params
        switch (paramType)
        {
        case nShaderState::Bool:
            if (val == nString("true") || val == "1")
            {
                arg.SetBool(true);
            }
            else
            {
                arg.SetBool(false);
                if (val != nString("false") && val != "0")
                {
                    n_printf("nGLSLShader::ParsePassParam(): Unknown value of the Bool pass parameter <%s> - <%s>. Use True or 1 and False or 0. False is default.\n", name, val.Get());
                }
            }
            break;
        case nShaderState::Int:
            if (val.IsValidInt())
            {
                arg.SetInt(val.AsInt());
            }
            else
            {
                arg.SetInt(0);
                n_printf("nGLSLShader::ParsePassParam(): Unknown value of the Int pass parameter <%s> - <%s>. 0 is default.\n", name, val.Get());
            }
            break;
        case nShaderState::Float:
            if (val.IsValidFloat())
            {
                arg.SetFloat(val.AsFloat());
            }
            else
            {
                arg.SetFloat(0.0);
                n_printf("nGLSLShader::ParsePassParam(): Unknown value of the Float pass parameter <%s> - <%s>. 0.0 is default.\n", name, val.Get());
            }
            break;
        case nShaderState::Float4:
            // Example: 1.0,0.2,0.5,0.0
            warning = false;
            float_ptr = (float*)&f4;

            f4.x = f4.y = f4.z = f4.w = 0.0;

            n = n_min(4, val.Tokenize(",", subvalues));
            for (i = 0; i < n; i++)
            {
                if (subvalues[i].IsValidFloat())
                {
                    float_ptr[i] = subvalues[i].AsFloat();
                }
                else
                {
                    warning = true;
                }
            }
            arg.SetFloat4(f4);

            if (warning || n != 4)
            {
                n_printf("nGLSLShader::ParsePassParam(): Wrong values or values count (need 4) of the Float4 pass parameter <%s>.\n", name);
            }
            break;
        case nShaderState::Matrix44:
            // Example: 1.0,0.2,0.5,0.0; 1.0,0.2,0.5,0.0; 1.0,0.2,0.5,0.0; 1.0,0.2,0.5,0.0
            warning = false;
            float_ptr = (float*)&m44.m;

            m44.M11 = m44.M12 = m44.M13 = m44.M14 =
            m44.M21 = m44.M22 = m44.M23 = m44.M24 =
            m44.M31 = m44.M32 = m44.M33 = m44.M34 =
            m44.M41 = m44.M42 = m44.M43 = m44.M44 = 0.0;

            n = n_min(16, val.Tokenize(",;", subvalues));
            for (i = 0; i < n; i++)
            {
                if (subvalues[i].IsValidFloat())
                {
                    float_ptr[i] = subvalues[i].AsFloat();
                }
                else
                {
                    warning = true;
                }
            }
            arg.SetMatrix44(&m44);

            if (warning || n != 16)
            {
                n_printf("nGLSLShader::ParsePassParam(): Wrong values or values count (need 16) of the Matrix44 pass parameter <%s>.\n", name);
            }
            break;
        default:
            n_message("nGLSLShader::ParsePassParam(): Unsupported pass parameter <%s:%d>.\n", name, paramType);
            return false;
        }
    }

    passParamFunc[paramType](index, arg);

    return true;
}


// Parameter functions

#pragma region Light States
void LightAmbient_Func(int index, const nShaderArg& arg)
{
    // TODO: check index parameter for correctness
    glLightfv(GL_LIGHT0 + index, GL_AMBIENT, (GLfloat*)&(arg.GetFloat4()));
}

void LightAttenuation0_Func(int index, const nShaderArg& arg)
{
    // TODO: check index parameter for correctness
    glLightf(GL_LIGHT0 + index, GL_CONSTANT_ATTENUATION, arg.GetFloat());
}

void LightAttenuation1_Func(int index, const nShaderArg& arg)
{
    // TODO: check index parameter for correctness
    glLightf(GL_LIGHT0 + index, GL_LINEAR_ATTENUATION, arg.GetFloat());
}

void LightAttenuation2_Func(int index, const nShaderArg& arg)
{
    // TODO: check index parameter for correctness
    glLightf(GL_LIGHT0 + index, GL_QUADRATIC_ATTENUATION, arg.GetFloat());
}

void LightDiffuse_Func(int index, const nShaderArg& arg)
{
    // TODO: check index parameter for correctness
    glLightfv(GL_LIGHT0 + index, GL_DIFFUSE, (GLfloat*)&(arg.GetFloat4()));
}

void LightDirection_Func(int index, const nShaderArg& arg)
{
    // TODO: check index parameter for correctness
    glLightfv(GL_LIGHT0 + index, GL_SPOT_DIRECTION, (GLfloat*)&(arg.GetFloat4()));
}

void LightEnable_Func(int index, const nShaderArg& arg)
{
    nGL_ENABLE_DISABLE(GL_LIGHT0 + index);
}

void LightFalloff_Func(int index, const nShaderArg& arg)
{
    // TODO: check if Falloff correspond to GL_SPOT_EXPONENT?
    // TODO: check index parameter for correctness
    glLightf(GL_LIGHT0 + index, GL_SPOT_EXPONENT, arg.GetFloat());
}

void LightPhi_Func(int index, const nShaderArg& arg) {}

void LightPosition_Func(int index, const nShaderArg& arg)
{
    // TODO: check index parameter for correctness
    glLightfv(GL_LIGHT0 + index, GL_POSITION, (GLfloat*)&(arg.GetFloat4()));
}

void LightRange_Func(int index, const nShaderArg& arg)
{
    // TODO: check if Range correspond to GL_SPOT_CUTOFF?
    // TODO: check index parameter for correctness
    glLightf(GL_LIGHT0 + index, GL_SPOT_CUTOFF, arg.GetFloat());
}

void LightSpecular_Func(int index, const nShaderArg& arg)
{
    // TODO: check index parameter for correctness
    glLightfv(GL_LIGHT0 + index, GL_SPECULAR, (GLfloat*)&(arg.GetFloat4()));
}

void LightTheta_Func(int index, const nShaderArg& arg) {}
void LightType_Func(int index, const nShaderArg& arg) {}
#pragma endregion

#pragma region Material States
void MaterialAmbient_Func(int index, const nShaderArg& arg)
{
    // TODO: check index parameter for correctness
    glMaterialfv(GL_LIGHT0 + index, GL_AMBIENT, (GLfloat*)&(arg.GetFloat4()));
}

void MaterialDiffuse_Func(int index, const nShaderArg& arg)
{
    // TODO: check index parameter for correctness
    glMaterialfv(GL_LIGHT0 + index, GL_DIFFUSE, (GLfloat*)&(arg.GetFloat4()));
}

void MaterialEmissive_Func(int index, const nShaderArg& arg)
{
    // TODO: check index parameter for correctness
    glMaterialfv(GL_LIGHT0 + index, GL_EMISSION, (GLfloat*)&(arg.GetFloat4()));
}

void MaterialPower_Func(int index, const nShaderArg& arg)
{
    // TODO: check if Power correspond to GL_SHININESS?
    // TODO: check index parameter for correctness
    glMaterialf(GL_LIGHT0 + index, GL_SHININESS, arg.GetFloat());
}

void MaterialSpecular_Func(int index, const nShaderArg& arg)
{
    // TODO: check index parameter for correctness
    glMaterialfv(GL_LIGHT0 + index, GL_SPECULAR, (GLfloat*)&(arg.GetFloat4()));
}
#pragma endregion

#pragma region Render States
void AlphaBlendEnable_Func(int index, const nShaderArg& arg)
{
    nGL_ENABLE_DISABLE(GL_BLEND);
}

void AlphaFunc_Func(int index, const nShaderArg& arg)
{
    GLint ref;

    glGetIntegerv(GL_ALPHA_TEST_FUNC, &ref);

    glAlphaFunc(arg.GetInt(), ref);
}

void AlphaRef_Func(int index, const nShaderArg& arg)
{
    GLint func;

    glGetIntegerv(GL_ALPHA_TEST_REF, &func);

    // !!! ref should be Float not Int
    glAlphaFunc(func, arg.GetInt());
}

void AlphaTestEnable_Func(int index, const nShaderArg& arg)
{
    nGL_ENABLE_DISABLE(GL_ALPHA_TEST);
}

void BlendOp_Func(int index, const nShaderArg& arg)
{
    // ????
    //glBlendEquation(arg.GetInt());
}

void ColorWriteEnable_Func(int index, const nShaderArg& arg) {}
void DepthBias_Func(int index, const nShaderArg& arg) {}

void DestBlend_Func(int index, const nShaderArg& arg)
{
    GLenum srcFunc;

    glGetIntegerv(GL_BLEND_SRC, (GLint*)&srcFunc);

    glBlendFunc(srcFunc, arg.GetInt());
}

void DitherEnable_Func(int index, const nShaderArg& arg)
{
    nGL_ENABLE_DISABLE(GL_DITHER);
}

void FillMode_Func(int index, const nShaderArg& arg)
{
    // TODO: check first parameter correctness
    glPolygonMode(GL_FRONT_AND_BACK, arg.GetInt());
}

void LastPixel_Func(int index, const nShaderArg& arg) {}

void ShadeMode_Func(int index, const nShaderArg& arg)
{
    glShadeModel(arg.GetInt());
}

void SlopeScaleDepthBias_Func(int index, const nShaderArg& arg) {}

void SrcBlend_Func(int index, const nShaderArg& arg)
{
    GLenum dstFunc;

    glGetIntegerv(GL_BLEND_DST, (GLint*)&dstFunc);

    glBlendFunc(arg.GetInt(), dstFunc);
}

void StencilEnable_Func(int index, const nShaderArg& arg)
{
    // TODO: check below should be GL_STENCIL or GL_STENCIL_TEST?
    nGL_ENABLE_DISABLE(GL_STENCIL_TEST);
}

void StencilFail_Func(int index, const nShaderArg& arg)
{
    GLenum zfail, zpass;

    glGetIntegerv(GL_STENCIL_PASS_DEPTH_FAIL, (GLint*)&zfail);
    glGetIntegerv(GL_STENCIL_PASS_DEPTH_PASS, (GLint*)&zpass);

    glStencilOp(arg.GetInt(), zfail, zpass);
}

void StencilFunc_Func(int index, const nShaderArg& arg)
{
    GLint ref;
    GLuint mask;

    glGetIntegerv(GL_STENCIL_REF, &ref);
    glGetIntegerv(GL_STENCIL_VALUE_MASK, (GLint*)&mask);

    glStencilFunc(arg.GetInt(), ref, mask);
}

void StencilMask_Func(int index, const nShaderArg& arg)
{
    GLenum func;
    GLint ref;

    glGetIntegerv(GL_STENCIL_FUNC, (GLint*)&func);
    glGetIntegerv(GL_STENCIL_REF, &ref);

    glStencilFunc(func, ref, arg.GetInt());
}

void StencilPass_Func(int index, const nShaderArg& arg)
{
    GLenum fail, zfail;

    glGetIntegerv(GL_STENCIL_FAIL, (GLint*)&fail);
    glGetIntegerv(GL_STENCIL_PASS_DEPTH_FAIL, (GLint*)&zfail);

    glStencilOp(fail, zfail, arg.GetInt());
}

void StencilRef_Func(int index, const nShaderArg& arg)
{
    GLenum func;
    GLuint mask;

    glGetIntegerv(GL_STENCIL_FUNC, (GLint*)&func);
    glGetIntegerv(GL_STENCIL_VALUE_MASK, (GLint*)&mask);

    glStencilFunc(func, arg.GetInt(), mask);
}

// is this equivalent to StencilMask_Func?
void StencilWriteMask_Func(int index, const nShaderArg& arg) {}

void StencilZFail_Func(int index, const nShaderArg& arg)
{
    GLenum fail, zpass;

    glGetIntegerv(GL_STENCIL_FAIL, (GLint*)&fail);
    glGetIntegerv(GL_STENCIL_PASS_DEPTH_PASS, (GLint*)&zpass);

    glStencilOp(fail, arg.GetInt(), zpass);
}

void TextureFactor_Func(int index, const nShaderArg& arg) {}
void Wrap_Func(int index, const nShaderArg& arg) {}

void ZEnable_Func(int index, const nShaderArg& arg)
{
    nGL_ENABLE_DISABLE(GL_DEPTH_TEST);
}

void ZFunc_Func(int index, const nShaderArg& arg)
{
    glDepthFunc(arg.GetInt());
}

void ZWriteEnable_Func(int index, const nShaderArg& arg)
{
    // TODO: what difference with ZEnable?
    nGL_ENABLE_DISABLE(GL_DEPTH_TEST);
}

void ScissorTestEnable_Func(int index, const nShaderArg& arg)     // ???
{
    nGL_ENABLE_DISABLE(GL_SCISSOR_TEST);
}
#pragma endregion

#pragma region Vertex Pipe Render States
void Ambient_Func(int index, const nShaderArg& arg) {}
void AmbientMaterialSource_Func(int index, const nShaderArg& arg) {}
void Clipping_Func(int index, const nShaderArg& arg) {}

void ClipPlaneEnable_Func(int index, const nShaderArg& arg)
{
    // TODO: check index parameter for correctness
    nGL_ENABLE_DISABLE(GL_CLIP_PLANE0 + index);
}

void ColorVertex_Func(int index, const nShaderArg& arg) {}

void CullMode_Func(int index, const nShaderArg& arg)
{
    if (0 == arg.GetInt())
    {
        glDisable(GL_CULL_FACE);
    }
    else
    {
        glEnable(GL_CULL_FACE);
        glCullFace(arg.GetInt());
        //glFrontFace(arg.GetInt());
    }
}

void DiffuseMaterialSource_Func(int index, const nShaderArg& arg) {}
void EmissiveMaterialSource_Func(int index, const nShaderArg& arg) {}

void FogColor_Func(int index, const nShaderArg& arg)
{
    glFogfv(GL_FOG_COLOR, (GLfloat*)&(arg.GetFloat4()));
}

void FogDensity_Func(int index, const nShaderArg& arg)
{
    glFogf(GL_FOG_DENSITY, arg.GetFloat());
}

void FogEnable_Func(int index, const nShaderArg& arg)
{
    nGL_ENABLE_DISABLE(GL_FOG);
}

void FogEnd_Func(int index, const nShaderArg& arg)
{
    glFogf(GL_FOG_END, arg.GetFloat());
}

void FogStart_Func(int index, const nShaderArg& arg)
{
    glFogf(GL_FOG_START, arg.GetFloat());
}

void FogTableMode_Func(int index, const nShaderArg& arg)
{
    glFogi(GL_FOG_MODE, arg.GetInt());
    glFogi(GL_FOG_COORD_SRC, GL_FRAGMENT_DEPTH); // TODO: check this
}

void FogVertexMode_Func(int index, const nShaderArg& arg)
{
    glFogi(GL_FOG_MODE, arg.GetInt());
    glFogi(GL_FOG_COORD_SRC, GL_FOG_COORD); // TODO: check this
}

void IndexedVertexBlendEnable_Func(int index, const nShaderArg& arg) {}

void Lighting_Func(int index, const nShaderArg& arg)
{
    nGL_ENABLE_DISABLE(GL_LIGHTING);
}

void LocalViewer_Func(int index, const nShaderArg& arg) {}
void MultiSampleAntialias_Func(int index, const nShaderArg& arg) {}
void MultiSampleMask_Func(int index, const nShaderArg& arg) {}

void NormalizeNormals_Func(int index, const nShaderArg& arg)
{
    nGL_ENABLE_DISABLE(GL_NORMALIZE);
}

void PatchSegments_Func(int index, const nShaderArg& arg) {}
void PointScale_A_Func(int index, const nShaderArg& arg) {}
void PointScale_B_Func(int index, const nShaderArg& arg) {}
void PointScale_C_Func(int index, const nShaderArg& arg) {}
void PointScaleEnable_Func(int index, const nShaderArg& arg) {}

void PointSize_Func(int index, const nShaderArg& arg)
{
    glPointSize(arg.GetFloat());
}

void PointSize_Min_Func(int index, const nShaderArg& arg)
{
    // ??? this function abscent in gl.h header. wrong GL version?
    //glPointParameterf(GL_POINT_SIZE_MIN, arg.GetFloat());
}

void PointSize_Max_Func(int index, const nShaderArg& arg)
{
    // ??? this function abscent in gl.h header. wrong GL version?
    //glPointParameterf(GL_POINT_SIZE_MAX, arg.GetFloat());
}

void PointSpriteEnable_Func(int index, const nShaderArg& arg)
{
    nGL_ENABLE_DISABLE(GL_POINT_SPRITE);
}

void RangeFogEnable_Func(int index, const nShaderArg& arg) {}
void SpecularEnable_Func(int index, const nShaderArg& arg) {}
void SpecularMaterialSource_Func(int index, const nShaderArg& arg) {}
void TweenFactor_Func(int index, const nShaderArg& arg) {}
void VertexBlend_Func(int index, const nShaderArg& arg) {}
#pragma endregion

#pragma region Sampler States
void Sampler_Func(int index, const nShaderArg& arg) {}

void AddressU_Func(int index, const nShaderArg& arg) {}
void AddressV_Func(int index, const nShaderArg& arg) {}
void AddressW_Func(int index, const nShaderArg& arg) {}
void BorderColor_Func(int index, const nShaderArg& arg) {}
void MagFilter_Func(int index, const nShaderArg& arg) {}
void MaxAnisotropy_Func(int index, const nShaderArg& arg) {}
void MaxMipLevel_Func(int index, const nShaderArg& arg) {}
void MinFilter_Func(int index, const nShaderArg& arg) {}
void MipFilter_Func(int index, const nShaderArg& arg) {}
void MipMapLodBias_Func(int index, const nShaderArg& arg) {}
void SRGBTexture_Func(int index, const nShaderArg& arg) {}
#pragma endregion

#pragma region Texture States
void Texture_Func(int index, const nShaderArg& arg) {}

// *** Texture Stage States
void AlphaOp_Func(int index, const nShaderArg& arg) {}
void AlphaArg0_Func(int index, const nShaderArg& arg) {}
void AlphaArg1_Func(int index, const nShaderArg& arg) {}
void AlphaArg2_Func(int index, const nShaderArg& arg) {}
void ColorArg0_Func(int index, const nShaderArg& arg) {}
void ColorArg1_Func(int index, const nShaderArg& arg) {}
void ColorArg2_Func(int index, const nShaderArg& arg) {}
void ColorOp_Func(int index, const nShaderArg& arg) {}
void BumpEnvLScale_Func(int index, const nShaderArg& arg) {}
void BumpEnvLOffset_Func(int index, const nShaderArg& arg) {}
void BumpEnvMat00_Func(int index, const nShaderArg& arg) {}
void BumpEnvMat01_Func(int index, const nShaderArg& arg) {}
void BumpEnvMat10_Func(int index, const nShaderArg& arg) {}
void BumpEnvMat11_Func(int index, const nShaderArg& arg) {}
void ResultArg_Func(int index, const nShaderArg& arg) {}
void TexCoordIndex_Func(int index, const nShaderArg& arg) {}
void TextureTransformFlags_Func(int index, const nShaderArg& arg) {}
#pragma endregion

#pragma region Transform States
void ProjectionTransform_Func(int index, const nShaderArg& arg)
{
    matrix44 m44 = nGfxServer2::Instance()->GetTransform(nGfxServer2::Projection);

    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf((GLfloat*)m44.m);
    //glLoadMatrixf((GLfloat*)arg.GetMatrix44()->m);
}

void TextureTransform_Func(int index, const nShaderArg& arg)
{
    // TODO: check index. Must be 0..3.
    matrix44 m44 = nGfxServer2::Instance()->GetTransform((nGfxServer2::TransformType)(nGfxServer2::Texture0 + index));

    glMatrixMode(GL_TEXTURE);
    // ??? not present in gl.h header. wrong GL version ?
    //glActiveTexture(GL_TEXTURE0 + index);
    glActiveTextureARB(GL_TEXTURE0_ARB + index);
    glLoadMatrixf((GLfloat*)m44.m);
    //glLoadMatrixf((GLfloat*)arg.GetMatrix44()->m);
}

void ViewTransform_Func(int index, const nShaderArg& arg)
{
    //matrix44 m44 = nGfxServer2::Instance()->GetTransform(nGfxServer2::Model) * *(arg.GetMatrix44());
    matrix44 m44 = nGfxServer2::Instance()->GetTransform(nGfxServer2::ModelView);

    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf((GLfloat*)m44.m);
}

void WorldTransform_Func(int index, const nShaderArg& arg)
{
    // TODO: is WorltTransform correspond to GL_MODELVIEW?
    //matrix44 m44 = nGfxServer2::Instance()->GetTransform(nGfxServer2::Model) * *(arg.GetMatrix44());
    matrix44 m44 = nGfxServer2::Instance()->GetTransform(nGfxServer2::ModelView);

    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf((GLfloat*)m44.m);
}
#pragma endregion

#pragma region Shader States
void FragmentShader_Func(int index, const nShaderArg& arg) {}                   // was PixelShader
void VertexShader_Func(int index, const nShaderArg& arg) {}

// *** Shader Constant States
void PixelShaderConstant_Func(int index, const nShaderArg& arg) {}
void PixelShaderConstant1_Func(int index, const nShaderArg& arg) {}
void PixelShaderConstant2_Func(int index, const nShaderArg& arg) {}
void PixelShaderConstant3_Func(int index, const nShaderArg& arg) {}
void PixelShaderConstant4_Func(int index, const nShaderArg& arg) {}
void PixelShaderConstantB_Func(int index, const nShaderArg& arg) {}
void PixelShaderConstantI_Func(int index, const nShaderArg& arg) {}
void PixelShaderConstantF_Func(int index, const nShaderArg& arg) {}
void VertexShaderConstant_Func(int index, const nShaderArg& arg) {}
void VertexShaderConstant1_Func(int index, const nShaderArg& arg) {}
void VertexShaderConstant2_Func(int index, const nShaderArg& arg) {}
void VertexShaderConstant3_Func(int index, const nShaderArg& arg) {}
void VertexShaderConstant4_Func(int index, const nShaderArg& arg) {}
void VertexShaderConstantB_Func(int index, const nShaderArg& arg) {}
void VertexShaderConstantI_Func(int index, const nShaderArg& arg) {}
void VertexShaderConstantF_Func(int index, const nShaderArg& arg) {}
#pragma endregion

#pragma region Unknown
void FVF_Func(int index, const nShaderArg& arg) {}
#pragma endregion
