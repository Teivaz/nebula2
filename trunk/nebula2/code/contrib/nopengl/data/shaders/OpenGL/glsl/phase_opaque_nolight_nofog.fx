#line 1 "phase_opaque_nolight_nofog.fx"
//------------------------------------------------------------------------------
//  phase_opaque_nolight_nofog.fx
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
