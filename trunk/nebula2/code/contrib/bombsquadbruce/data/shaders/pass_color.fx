#line 1 "pass_color.fx"
//------------------------------------------------------------------------------
//  pass_color.fx
//
//  Set render states which are constant for the entire color pass.
//
//  (C) 2004 Rafael Van Daele-Hunt
//------------------------------------------------------------------------------
shared float4x4 View;                   // the view matrix
shared float4x4 Projection;             // the projection matrix

technique t0
{
    pass p0
    {   
        ColorWriteEnable    = RED|GREEN|BLUE|ALPHA;  
        NormalizeNormals    = True;
        ZFunc               = LessEqual;
        StencilEnable       = False;
        DepthBias           = 0.0f;
        PixelShader         = 0;
        SpecularEnable	    = False;

        // FillMode         = Wireframe;
    }
}

