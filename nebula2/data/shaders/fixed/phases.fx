#line 1 "phases.fx"
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
        FogEnable        = False;
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
    }
}

technique tPhaseOpaque
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
        AlphaTestEnable  = True;
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
        FogEnable        = False;
        TextureTransformFlags[0] = 0;
    }
}

technique tPhaseVsOpaque
{
    pass p0
    {
        ZWriteEnable     = True;
        AlphaBlendEnable = False;
        AlphaTestEnable  = True;
        AlphaFunc        = GreaterEqual;
        Lighting         = False;
        FogEnable        = False;
        TextureTransformFlags[0] = 0;
    }
}
