#line 1 "phase_opaque_nolight_nofog.fx"
//------------------------------------------------------------------------------
//  phase_opaque_nolight_nofog.fx
//
//  Set render states which are constant for the entire opaque phase in
//  the color pass.
//
//  (C) 2004 Rafael Van Daele-Hunt
//------------------------------------------------------------------------------
shared float4x4 Model;
shared float4x4 View;
shared float4x4 Projection;

technique t0
{
    pass p0
    {
        WorldTransform[0] = <Model>;
        ViewTransform = <View>;
        ProjectionTransform = <Projection>;
    
        ZWriteEnable     = True;
        ZEnable          = True;
        ZFunc            = LessEqual;      

        VertexShader = 0;
        
        AlphaBlendEnable = False;    
        AlphaTestEnable  = False;
        FogEnable        = False;      
        LightEnable[0]   = False;
        Lighting         = False;
    }
}
