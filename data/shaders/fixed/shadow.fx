//------------------------------------------------------------------------------
//  shadow_static.fx
//
//  Shadow volume shader for static geometry.
//
//  (C) 2005 Radon Labs GmbH
//------------------------------------------------------------------------------
shared float4x4 ModelViewProjection;

int StencilFrontZFailOp = 1;    // int: front faces stencil depth fail operation
int StencilFrontPassOp  = 1;    // int: front faces stencil pass operation
int StencilBackZFailOp  = 1;    // int: front faces stencil depth fail operation
int StencilBackPassOp   = 1;    // int: front faces stencil depth fail operation

float3 ModelLightPos;   // the light's position in model space
int    LightType;       // 0 == Point, 1 == Directional
float  LightRange;      // light range, only meaningfull for point lights

static float DirExtrudeLen = 1000.0f;

//------------------------------------------------------------------------------
/**
    Vertex shader for static shadow volumes. This will do the extrusion
    on the GPU. The extrude vector's x component is either 0.0 if the vector
    should not be extruded, or 1.0 if the vector should be extruded.

    If the light is a directional light, ModelLightPos does not contain a
    position, but instead the light direction in model space.

    NOTE: the non-extruded component could also be a very small number to
    shift the shadow volume a little bit away from the light source.

    NOTE: the extrusion may clip against the far plane, this case is not
    currently handled at all (should we even care??).
*/
float4 vsMain(float4 position : POSITION) : POSITION
{
    // compute the extrusion vector
    float4 extrudeVec = 0;
    float extrudeLen;
    if (1 == LightType)
    {
        // handle directional light
        extrudeVec.xyz = normalize(-ModelLightPos);
        extrudeLen = DirExtrudeLen;
    }
    else
    {
        // handle point light
        extrudeVec.xyz = normalize(position.xyz - ModelLightPos);
        extrudeLen = LightRange;
    }
    float4 pos = float4(position.xyz + extrudeVec * extrudeLen * position.w + extrudeVec * 0.0025f, 1.0f);
    return mul(pos, ModelViewProjection);
}

//------------------------------------------------------------------------------
/**
    2-Pass-Technique for older GPUs.

    NOTE: because of the "Old NVIDIA cards don't validate shader files with
    vertex shaders even if software vertexprocessing is on" bug and
    Nebula workaround, we cannot have 2 technique in the fixed function
    pipeline. Thus only the "safe" TwoPass technique exists in this file.
*/
technique TwoPass
{
    pass FrontFace
    {
        ColorWriteEnable    = 0;
        AlphaBlendEnable    = false;
        StencilEnable       = true;
        ZEnable             = true;
        FogEnable           = false;
        AlphaTestEnable     = false;

        ZWriteEnable        = false;
        ZFunc               = LessEqual;

        CullMode            = Cw;

        StencilFunc         = Always;
        StencilZFail        = <StencilFrontZFailOp>;
        StencilPass         = <StencilFrontPassOp>;

        VertexShader        = compile vs_2_0 vsMain();
        ColorOp[0] = SelectArg1;
        ColorArg1[0] = Current;
        AlphaOp[0] = Disable;
    }
    pass BackFace
    {
        CullMode = Ccw;
        StencilZFail = <StencilBackZFailOp>;
        StencilPass = <StencilBackPassOp>;
    }
}
