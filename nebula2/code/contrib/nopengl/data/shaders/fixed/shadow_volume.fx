#line 1 "shadow_volume.fx"
//------------------------------------------------------------------------------
//  shadow_volume.fx
//
//  Write the stencil values for the shadow volumes, work on pretransformed,
//  extruded data.
//  
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
uniform float4x4 View;                   // the view matrix
uniform float4x4 Projection;             // the projection matrix

static const float4x4 Ident =  {1.0f, 0.0f, 0.0f, 0.0f,
                                0.0f, 1.0f, 0.0f, 0.0f, 
                                0.0f, 0.0f, 1.0f, 0.0f,
                                0.0f, 0.0f, 0.0f, 1.0f };

int StencilFrontZFailOp = 1;    // int: front faces stencil depth fail operation
int StencilFrontPassOp  = 1;    // int: front faces stencil pass operation
int StencilBackZFailOp  = 1;    // int: front faces stencil depth fail operation
int StencilBackPassOp   = 1;    // int: front faces stencil depth fail operation

//------------------------------------------------------------------------------
technique double_sided
{
    // double sided stencil operation
    pass frontface
    {
        WorldTransform[0]   = <Ident>; // indent, the data is already transformed into world space on cpu
        ViewTransform       = <View>;
        ProjectionTransform = <Projection>;    
        
        // clear a prevoius states
        ColorWriteEnable = 0;
        Lighting         = false;
        SpecularEnable	 = false;
        FogEnable        = false;
        AlphaTestEnable  = false;
        AlphaBlendEnable = false;
        NormalizeNormals = false;
        
        TextureTransform[0]      = 0;
        TextureTransformFlags[0] = 0;
        
        ColorOp[0]   = Disable;
        AlphaOp[0]   = Disable;
        
        // FVF = XYZ;
        
        VertexShader = 0;
        PixelShader  = 0;
        
        ZWriteEnable     = False;
        ZEnable          = True;
        ZFunc            = Less;
        
        StencilEnable = true;   // enable stenciling
        TwoSidedStencilMode = true;
        CullMode = None;
        
        StencilFunc = Always;   // allways pass stencil test
        StencilZFail = <StencilFrontZFailOp>;
        StencilPass  = <StencilFrontPassOp>;
        
        Ccw_StencilFunc = Always;   // allways pass stencil test
        Ccw_StencilZFail = <StencilBackZFailOp>;
        Ccw_StencilPass  = <StencilBackPassOp>;
    }
}

//------------------------------------------------------------------------------
technique single_sided
{
    pass frontface
    {
        WorldTransform[0]   = <Ident>; // indent, the data is already transformed into world space on cpu
        ViewTransform       = <View>;
        ProjectionTransform = <Projection>;    
        
        // clear a prevoius states
        ColorWriteEnable = 0;
        Lighting         = false;
        SpecularEnable	 = false;
        FogEnable        = false;
        AlphaTestEnable  = false;
        AlphaBlendEnable = false;
        NormalizeNormals = false;
        
        TextureTransform[0]      = 0;
        TextureTransformFlags[0] = 0;
        
        ColorOp[0]   = Disable;
        AlphaOp[0]   = Disable;
        
        FVF = XYZ;
        
        VertexShader = 0;
        PixelShader  = 0;
        
        ZWriteEnable     = False;
        ZEnable          = True;
        ZFunc            = Less;
        
        StencilEnable = true;   // enable stenciling
        StencilFunc = Always;   // allways pass stencil test
    
        CullMode = CW;
        StencilZFail = <StencilFrontZFailOp>;
        StencilPass  = <StencilFrontPassOp>;
    }
    
    pass backface
    {
        CullMode = CCW;
        StencilZFail = <StencilBackZFailOp>;
        StencilPass  = <StencilBackPassOp>;
    }
}
