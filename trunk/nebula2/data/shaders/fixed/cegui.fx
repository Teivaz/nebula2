//------------------------------------------------------------------------------
//  cegui.fx
//  A 2D rectangle shader for CEGUI rendering.
//------------------------------------------------------------------------------
#define VS_PROFILE vs_2_0

shared float4x4 ModelViewProjection;   // modelview * projection matrix
texture DiffMap0;

static const float4x4 Ident = { 1.0f, 0.0f, 0.0f, 0.0f,
                                0.0f, 1.0f, 0.0f, 0.0f,
                                0.0f, 0.0f, 1.0f, 0.0f,
                                0.0f, 0.0f, 0.0f, 1.0f };

//------------------------------------------------------------------------------
struct vsInput
{
    float4 position             : POSITION;
    float4 color                : COLOR;
    float2 uv0                  : TEXCOORD0;
};

struct vsOutput
{
    float4 position             : POSITION;
    float4 color                : COLOR;
    float2 uv0                  : TEXCOORD0;
};

//------------------------------------------------------------------------------
sampler ColorMap = sampler_state
{
    Texture                     = <DiffMap0>;
    AddressU                    = Clamp;
    AddressV                    = Clamp;
    MinFilter                   = Linear;
    MagFilter                   = Linear;
    MipFilter                   = None;
};

//------------------------------------------------------------------------------
vsOutput vsMain(const vsInput vsIn)
{
    vsOutput vsOut;
    vsOut.position = mul(vsIn.position, ModelViewProjection);
    vsOut.color = vsIn.color;
    vsOut.uv0 = vsIn.uv0;
    return vsOut;
}

//------------------------------------------------------------------------------
technique t0
{
    pass p0
    {
        ColorWriteEnable        = RED | GREEN | BLUE | ALPHA;
        Lighting                = False;
        FogEnable               = False;
        StencilEnable           = False;
        AlphaBlendEnable        = True;
        SrcBlend                = SrcAlpha;
        DestBlend               = InvSrcAlpha;
        AlphaTestEnable         = False;
        ZEnable                 = True;
        ZWriteEnable            = True;
        ZFunc                   = LessEqual;
        CullMode                = None;

        WorldTransform[0]       = <Ident>;
        ViewTransform           = <Ident>;
        ProjectionTransform     = <ModelViewProjection>;
        Sampler[0]              = <ColorMap>;
        TexCoordIndex[0]        = 0;

        ColorOp[0]              = Modulate;
        ColorArg1[0]            = Texture;
        ColorArg2[0]            = Diffuse;
        ColorOp[1]              = Disable;

        AlphaOp[0]              = Modulate;
        AlphaArg1[0]            = Texture;
        AlphaArg2[0]            = Diffuse;
        AlphaOp[1]              = Disable;

        VertexShader            = compile VS_PROFILE vsMain();
    }
}
