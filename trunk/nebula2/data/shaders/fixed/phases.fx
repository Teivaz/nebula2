//------------------------------------------------------------------------------
//  phases.fx
//
//  DX7 renderpath phase definitions.
//
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
technique tPhaseAlpha
{
    pass p0
    {
        ZWriteEnable     = False;
        AlphaBlendEnable = True;
        AlphaTestEnable  = False;
        Lighting         = True;
        VertexShader     = 0;
        FogEnable        = True;
    }
}

technique tPhaseMultiPass
{
    pass p0
    {
        ZWriteEnable     = True;        
        VertexShader     = 0;
        FogEnable        = True;
        Lighting         = True;
        
        // set zero texture transforms, so the layered shader won't change the default alpha
        TexCoordIndex[0] = 0;
        TexCoordIndex[1] = 0;
        
        TextureTransformFlags[0] = 0;
        TextureTransformFlags[1] = 0;
        
        TextureTransform[0] = 0;
        TextureTransform[1] = 0;
    }
}

technique tPhaseOpaque
{
    pass p0
    {
        ZWriteEnable     = True;
        AlphaBlendEnable = False;
        AlphaTestEnable  = False;
        AlphaFunc        = GreaterEqual;
        VertexShader     = 0;
        FogEnable        = True;
        LightEnable[0]   = True;            
        Lighting         = True;
    }
}

technique tPhaseOpaqueNoFog
{
    pass p0
    {
        ZWriteEnable     = True;
        AlphaBlendEnable = False;
        AlphaTestEnable  = False;
        AlphaFunc        = GreaterEqual;
        VertexShader     = 0;
        FogEnable        = False;      
        LightEnable[0]   = True;
        Lighting         = True;
    }
}

technique tPhaseOpaqueATest
{
    pass p0
    {
        ZWriteEnable     = True;
        AlphaBlendEnable = False;
        AlphaTestEnable  = True;
        AlphaFunc        = GreaterEqual;
        VertexShader     = 0;
        FogEnable        = True;
        LightEnable[0]   = True;            
        Lighting         = True;
    }
}

technique tPhaseOpaqueNoLightNoFog
{
    pass p0
    {
        ZWriteEnable     = True;
        AlphaBlendEnable = False;
        AlphaTestEnable  = False;
        AlphaFunc        = GreaterEqual;
        VertexShader     = 0;
        FogEnable        = False;      
        LightEnable[0]   = False;
        Lighting         = False;
    }
}

technique tPhaseVsAlpha
{
    pass p0
    {
        ZWriteEnable     = False;
        AlphaBlendEnable = True;
        AlphaTestEnable  = False;
        Lighting         = False;
        FogEnable        = True;
    }
}

technique tPhaseVsOpaque
{
    pass p0
    {
        ZWriteEnable     = True;
        SrcBlend         = One;
        DestBlend        = One;
        AlphaTestEnable  = False;
        AlphaFunc        = GreaterEqual;
        Lighting         = False;
        FogEnable        = True;
    }
}

technique tPhaseVsOpaqueATest
{
    pass p0
    {
        ZWriteEnable     = True;
        SrcBlend         = One;
        DestBlend        = One;        
        AlphaTestEnable  = True;
        AlphaFunc        = GreaterEqual;
        Lighting         = False;
        FogEnable        = True;
    }
}

technique tPhaseGui3D
{
    pass p0
    {
        ZWriteEnable     = False;
        AlphaBlendEnable = True;
        AlphaTestEnable  = False;
        Lighting         = True;
        FogEnable        = False;
    }
}
