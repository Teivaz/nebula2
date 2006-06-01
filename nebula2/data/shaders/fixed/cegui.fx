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
    }
}
