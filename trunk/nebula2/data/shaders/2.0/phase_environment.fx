#line 1 "phase_environment.fx"
//------------------------------------------------------------------------------
//  phase_environment.fx
//
//  Set render states which are constant for the entire opaque phase in
//  the color pass.
//
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
technique t0
{
    pass p0
    {
        ZWriteEnable     = true;
        AlphaBlendEnable = false;
        AlphaTestEnable  = false;
    }
}
