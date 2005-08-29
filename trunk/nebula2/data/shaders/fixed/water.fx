//------------------------------------------------------------------------------
//  water.fx
//
//  Simple water shader for the fixed function pipeline
//  based on the enviroment alpha shader
//
//  (C) 2005 RadonLabs GmbH
//------------------------------------------------------------------------------

shared float4x4 Model;
shared float4x4 InvModel;
shared float4x4 InvView;
shared float4x4 ModelViewProjection;
shared float4x4 ViewProjection;
shared float3   ModelEyePos;

static const float4x4 Ident = { 1.0f, 0.0f, 0.0f, 0.0f,
                                0.0f, 1.0f, 0.0f, 0.0f, 
                                0.0f, 0.0f, 1.0f, 0.0f,
                                0.0f, 0.0f, 0.0f, 1.0f };

bool AlphaBlendEnable;
int AlphaSrcBlend = 5;
int AlphaDstBlend = 6;
int CullMode = 2;       // CW
float4 MatDiffuse;
float4 MatSpecular;

texture DiffMap0;       
texture CubeMap0;


//------------------------------------------------------------------------------
/**
    Sampler definitions
*/
sampler DiffSampler = sampler_state
{
    Texture = <DiffMap0>;
    AddressU  = Wrap;
    AddressV  = Wrap;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
    MipMapLodBias = -0.75;    
};

sampler EnvironmentSampler = sampler_state
{
    Texture = <CubeMap0>;
    AddressU  = Wrap;
    AddressV  = Wrap;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
};

struct vsInputWater
{
    float4 position  : POSITION; 
    float3 normal    : NORMAL;
    float2 uv0       : TEXCOORD0;
    float4 color     : COLOR0;
};

struct vsOutputWater
{
    float4 position : POSITION;
    float2 uv0      : TEXCOORD0;
    float3 uv1      : TEXCOORD1;
    float4 diffuse  : COLOR0;
};

//------------------------------------------------------------------------------
/**
    Emulated vertex shader for dx7 water reflection.
*/
vsOutputWater vsWater(const vsInputWater vsIn)
{   
    vsOutputWater vsOut;

    // transform the position and normal
    // OPTIMIZATION: as long as the object is not rotated wrt world
    // space, the view vector can be computed in model space
    //float3 worldPos = mul(vsIn.position, (float4x3) Model);              // position (view space)
    //float3 worldEye = mul(ModelEyePos, Model);
    //float3 view = worldPos - worldEye;
    float3 view = vsIn.position - ModelEyePos;
    
    // transform normal to world space
    // OPTIMIZATION: see above
    //float3 normal = normalize(mul(vsIn.normal, transpose(InvModel)));
    float3 reflView = reflect(view, vsIn.normal);
    
    vsOut.position  = mul(vsIn.position, ModelViewProjection);          // position (projected)
    vsOut.uv0       = vsIn.uv0;                                         // uv1 from explicit uv coordinated
    vsOut.uv1       = reflView;                                         // cube map lookup by reflected view vector
    vsOut.diffuse   = vsIn.color;

    return vsOut;
}

technique tWater
{
    pass p0
    {
        CullMode  = <CullMode>;
        
        TextureTransform[0] = 0;
        TextureTransformFlags[0] = 0;
        TexCoordIndex[0] = 0;
        
        VertexShader = compile vs_2_0 vsWater();

        Sampler[0] = <DiffSampler>;
        Sampler[1] = <EnvironmentSampler>;

        //SrcBlend  = <AlphaSrcBlend>;
        //DestBlend = <AlphaDstBlend>;
        
        // Base Texture
        ColorOp[0]   = SelectArg1;
        ColorArg1[0] = Texture;
        AlphaOp[0]   = SelectArg1;
        AlphaArg1[0] = Texture;        
        
        // Reflection Texture
        ColorOp[1]   = ModulateColor_AddAlpha;
        ColorArg1[1] = Texture;
        ColorArg2[1] = Current;
        AlphaOp[1]   = SelectArg1;
        AlphaArg1[1] = Current;

        ColorOp[2] = Disable;
        AlphaOp[2] = Disable;
    }
}