#line 1 "pass_multipass.fx"
//------------------------------------------------------------------------------
//  phase_multipass.fx
//
//  Special multipass phase for the fixed function pipeline. This is necessary
//  because some multipass shaders require lighting, some not. Also the
//  state back functionality of phase shaders is required.
//
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
technique t0
{
    pass p0
    {
        ZWriteEnable     = True;        
        VertexShader     = 0;
        FogEnable        = True;
        Lighting         = True;
    }
}
