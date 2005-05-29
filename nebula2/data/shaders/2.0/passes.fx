#line 1 "passes.fx"
//------------------------------------------------------------------------------
//  passes.fx
//
//  Render path passes.
//
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
shared float4x4 View;                   // the view matrix
shared float4x4 Projection;             // the projection matrix
shared const float4 FogColor = {0.65, 0.65, 0.65, 0.0f};
shared const float4 FogParams = { 80.0f, 0.001f, 0.002f, 0.0f};     // FogLayerGround, FogHoriDensity, FogVertDensity

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
        FogEnable           = False;
        //FillMode         = Wireframe;
    }
}

