#line 1 "phase_opaque.fx"
//------------------------------------------------------------------------------
//  phase_opaque.fx
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
        FogEnable        = True;
        LightEnable[0]   = True;            
        Lighting         = True;
    }
}
