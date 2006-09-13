//------------------------------------------------------------------------------
//  cegui.fx
//
//  A 2d rectangle shader for GUI rendering.
//------------------------------------------------------------------------------
shared float4x4 ModelViewProjection;   // the modelview*projection matrix
texture DiffMap0;

static const float4x4 Ident = { 1.0f, 0.0f, 0.0f, 0.0f,
                                0.0f, 1.0f, 0.0f, 0.0f,
                                0.0f, 0.0f, 1.0f, 0.0f,
                                0.0f, 0.0f, 0.0f, 1.0f };


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
struct vsInput
{       
    float4 position : POSITION;
    float2 uv0      : TEXCOORD0;
    float4 color    : COLOR;
};

struct vsOutput
{
    float4 position  : POSITION;
    float2 uv0 : TEXCOORD0;
    float4 color    : COLOR;
};

//------------------------------------------------------------------------------
sampler ColorMap = sampler_state
{
    Texture = <DiffMap0>;
    AddressU = Clamp;
    AddressV = Clamp;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = None;
};

//------------------------------------------------------------------------------
vsOutput vsMain(const vsInput vsIn)
{
    vsOutput vsOut;
    vsOut.position = mul(vsIn.position, ModelViewProjection);
    vsOut.uv0 = vsIn.uv0;
    vsOut.color = vsIn.color;
    return vsOut;
}

//------------------------------------------------------------------------------
technique t0
{
    pass p0
    {
    	Lighting            = False;
        ZWriteEnable        = True;
        ZEnable             = True;
        ZFunc               = LessEqual;
        ColorWriteEnable    = RED|GREEN|BLUE|ALPHA;
        AlphaBlendEnable    = True;
        SrcBlend            = SrcAlpha;
        DestBlend           = InvSrcAlpha;
        AlphaTestEnable     = False;
        StencilEnable       = False;
        CullMode            = None;

        WorldTransform[0]   = <Ident>;
        ViewTransform       = <Ident>;
        ProjectionTransform = <ModelViewProjection>;

        TexCoordIndex[0] = 0;

        Sampler[0]       = <ColorMap>;

        ColorOp[0]       = Modulate;
        ColorArg1[0]     = Texture;
        ColorArg2[0]     = Diffuse;

        ColorOp[1]       = Disable;

        AlphaOp[0]       = Modulate;
        AlphaArg1[0]     = Texture;
        AlphaArg2[0]     = Diffuse;

        AlphaOp[1]       = Disable;

        VertexShader = compile vs_2_0 vsMain();
    }
}
