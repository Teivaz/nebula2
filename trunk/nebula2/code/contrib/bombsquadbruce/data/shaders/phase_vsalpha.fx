#line 1 "phase_vsalpha.fx"
//------------------------------------------------------------------------------
//  phase_vsalpha.fx
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
        AlphaBlendEnable = True;
        SrcBlend         = SrcAlpha;
        DestBlend        = InvSrcAlpha;
        CullMode         = Cw; // default value (CW); must not be 0 for software vp
        Lighting         = False;
    }
}
