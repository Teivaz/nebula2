//------------------------------------------------------------------------------
//  phase_pointsprites.fx
//
//  Set render states which are constant for the entire pointsprite phase in
//  the color pass.
//
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
technique t0
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
