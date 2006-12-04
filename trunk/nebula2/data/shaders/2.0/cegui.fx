//------------------------------------------------------------------------------
//  cegui.fx
//  A 2D rectangle shader for CEGUI rendering.
//------------------------------------------------------------------------------
#define VS_PROFILE vs_2_0
#define PS_PROFILE ps_2_0

typedef half4 color4;

shared float4x4 ModelViewProjection;   // modelview * projection matrix
texture DiffMap0;

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
color4 psMain(const vsOutput psIn) : COLOR
{
    return tex2D(ColorMap, psIn.uv0) * psIn.color;
}

//------------------------------------------------------------------------------
technique t0
{
    pass p0
    {
        ColorWriteEnable        = RED | GREEN | BLUE | ALPHA;
        StencilEnable           = False;
        AlphaBlendEnable        = True;
        SrcBlend                = SrcAlpha;
        DestBlend               = InvSrcAlpha;
        AlphaTestEnable         = True;
        ZEnable                 = True;
        ZWriteEnable            = True;
        ZFunc                   = LessEqual;
        CullMode                = None;

        VertexShader            = compile VS_PROFILE vsMain();
        PixelShader             = compile PS_PROFILE psMain();
    }
}
