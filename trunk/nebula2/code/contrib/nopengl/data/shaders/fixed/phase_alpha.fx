#line 1 "pass_alpha.fx"
//------------------------------------------------------------------------------
//  phase_alpha.fx
//
//  Set render states which are constant for the entire transparency phase in
//  the color pass.
//
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
technique t0
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

