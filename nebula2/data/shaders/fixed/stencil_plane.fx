//------------------------------------------------------------------------------
//  stencil_plane.fx
//
//------------------------------------------------------------------------------
shared float4x4 Model;
shared float4x4 View;
shared float4x4 Projection;

float4 MatDiffuse;

//------------------------------------------------------------------------------
technique t0
{
    pass p0
    {
        WorldTransform[0] = <Model>;
        ViewTransform = <View>;
    	ProjectionTransform = <Projection>;

        ZWriteEnable     = False;
        ZEnable          = False;
        AlphaTestEnable  = False;

        CullMode = None;

        VertexShader = 0;
        PixelShader  = 0;

        ColorWriteEnable = RED|GREEN|BLUE|ALPHA;

        AlphaBlendEnable = true;
        SrcBlend         = SrcAlpha;
        DestBlend        = InvSrcAlpha;

        StencilEnable = True;  // enable stenciling
        StencilRef = 0;
        StencilFunc = NotEqual;
        StencilZFail = KEEP;
        StencilPass =  KEEP;

        FVF = XYZ;

        Lighting = True;
        LightEnable[0] = False;
        SpecularEnable = False;
        MaterialDiffuse = <MatDiffuse>;
        MaterialAmbient = { 1.0, 1.0, 1.0, 1.0 };
        MaterialSpecular = { 0.0, 0.0, 0.0, 0.0 };
        Ambient = <MatDiffuse>;

        ColorOp[0]   = SelectArg1;
        ColorArg1[0] = Diffuse;

        ColorOp[1]   = Disable;

        AlphaOp[0]   = SelectArg1;
        AlphaArg1[0] = Diffuse;

        AlphaOp[1] = Disable;
    }
}

