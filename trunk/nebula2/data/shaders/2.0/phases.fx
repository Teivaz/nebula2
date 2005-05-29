#line 1 "phases.fx"
//------------------------------------------------------------------------------
//  phases.fx
//
//  Contains renderpath phase shaders.
//
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
technique tPhaseOpaque
{
    pass p0
    {
        ZWriteEnable     = true;
        AlphaBlendEnable = false;
        AlphaTestEnable  = false;
        AlphaFunc = GreaterEqual;
    }
}        

technique tPhaseEnvironment
{
    pass p0
    {
        ZWriteEnable     = true;
        AlphaBlendEnable = false;
        AlphaTestEnable  = false;
    }
}

technique tPhaseAlpha
{
    pass p0
    {
        ZWriteEnable     = false;
        AlphaBlendEnable = true;
        AlphaTestEnable  = false;
    }
}

technique tPhaseAlphaZWrite
{
    pass p0
    {
        ZWriteEnable     = true;
        AlphaBlendEnable = true;
        AlphaTestEnable  = false;
    }
}

technique tPhasePointSprites
{
    pass p0
    {
        ZWriteEnable     = false;
        AlphaBlendEnable = true;
        AlphaTestEnable  = false;
        AlphaFunc = GreaterEqual;
        PointSpriteEnable = True;
        PointScaleEnable  = False;        
    }
}
        
