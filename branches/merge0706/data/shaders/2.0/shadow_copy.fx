//------------------------------------------------------------------------------
//  shadow_copy.fx
//
//  shader for copying stencil mask to color texture with lightindex
//
//  (C) 2005 RadonLabs GmbH
//------------------------------------------------------------------------------
shared float4 ShadowIndex;

static const float4x4 Ident =  {1.0f, 0.0f, 0.0f, 0.0f,
                                0.0f, 1.0f, 0.0f, 0.0f,
                                0.0f, 0.0f, 1.0f, 0.0f,
                                0.0f, 0.0f, 0.0f, 1.0f };

technique t0
{
    pass p0
    {
        WorldTransform[0]   = <Ident>; // indent, the data is already transformed into world space on cpu
        ViewTransform       = <Ident>;
        ProjectionTransform = <Ident>;

        // clear a prevoius states
        ColorWriteEnable = RED|GREEN|BLUE|ALPHA;
        Lighting         = true;
        SpecularEnable	 = false;
        FogEnable        = false;
        AlphaTestEnable  = false;
        AlphaBlendEnable = true;
        SrcBlend         = One;
 	    DestBlend        = One;
        NormalizeNormals = false;

        Ambient             = {1.0,  1.0,  1.0,  1.0f};
        MaterialDiffuse     =  <ShadowIndex>;
        MaterialAmbient     =  <ShadowIndex>;
        LightEnable[0]      = false;
        TextureTransform[0]      = 0;
        TextureTransformFlags[0] = 0;

        ColorOp[0]   = SelectArg1;
        ColorArg1[0] = Diffuse;

        ColorOp[1]   = Disable;
        //ColorOp[0]   = Disable;
        AlphaOp[0]   = Disable;

        FVF = XYZ;

        VertexShader = 0;
        PixelShader  = 0;

        ZWriteEnable     = False;
        ZEnable          = False;
        StencilEnable    = true;  // enable stenciling
        StencilRef       = 0;
        StencilFunc      = NotEqual;
        StencilZFail     = KEEP;
        StencilPass      = ZERO; // set stencil pixel of zero for following lights
        CullMode = None;
    }
}