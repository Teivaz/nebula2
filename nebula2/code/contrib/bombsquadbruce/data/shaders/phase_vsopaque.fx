#line 1 "phase_vsopaque.fx"
//------------------------------------------------------------------------------
//  phase_vsopaque.fx
//
//  Prepare opaque rendering with vertex shaders under DX7.
//
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
technique t0
{
    pass p0
    {
        ZEnable          = True;
        ZWriteEnable     = True;
        CullMode = Cw; // default value (CW); must not be 0 for software vp
        AlphaBlendEnable = False;
        AlphaTestEnable  = True;
        AlphaFunc        = GreaterEqual;
        Lighting         = False;
        FogEnable        = False;
        TextureTransformFlags[0] = 0;
    }
}
