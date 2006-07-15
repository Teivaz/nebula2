//------------------------------------------------------------------------------
//  passes.fx
//
//  Set render states which are constant for the entire color pass.
//
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
shared float4x4 View;                   // the view matrix
shared float4x4 Projection;             // the projection matrix
float4 FogDistances;
float4 FogColor;

technique tPassColor
{
    pass p0
    {
        ViewTransform       = <View>;
    	ProjectionTransform = <Projection>;
        ColorWriteEnable    = RED|GREEN|BLUE|ALPHA;
        NormalizeNormals    = True;
        ZEnable             = True;
        ZFunc               = LessEqual;
        StencilEnable       = False;
        DepthBias           = 0.0f;
        PixelShader         = 0;
        SpecularEnable	    = False;

        FogEnable           = True;
        FogColor            = <FogColor>;
        FogVertexMode       = None;
        FogTableMode        = Linear;
        FogStart            = <FogDistances.x>;
        FogEnd              = <FogDistances.y>;

        TextureTransformFlags[0] = 0;

        // FillMode         = Wireframe;
    }
}

technique tPassGui3D
{
    pass p0
    {
        ViewTransform       = <View>;
        ProjectionTransform = <Projection>;
        ColorWriteEnable    = RED|GREEN|BLUE|ALPHA;
        NormalizeNormals    = True;
        ZEnable             = True;
        ZWriteEnable        = True;
        StencilEnable       = False;
        DepthBias           = 0.0f;
        FogEnable           = False;
        AlphaBlendEnable    = True;
        AlphaTestEnable     = False;
        AlphaFunc           = GreaterEqual;
        SrcBlend            = SrcAlpha;
        DestBlend           = InvSrcAlpha;
        ScissorTestEnable   = False;

        TextureTransformFlags[0] = 0;

        //FillMode         = Wireframe;
    }
}
