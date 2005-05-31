#line 1 "passes.fx"
//------------------------------------------------------------------------------
//  passes.fx
//
//  Set render states which are constant for the entire color pass.
//
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
shared float4x4 View;                   // the view matrix
shared float4x4 Projection;             // the projection matrix
shared float3 LightPos;             // the light's position in world space
shared float4 LightAmbient;         // light's ambient component
shared float4 LightDiffuse;         // light's diffuse component
shared float4 LightSpecular;        // light's specular component

technique tPassColor
{
    pass p0
    {
        ViewTransform       = <View>;
    	ProjectionTransform = <Projection>;    
        ColorWriteEnable    = RED|GREEN|BLUE|ALPHA;  
        NormalizeNormals    = True;
        ZEnable             = True;
        ZFunc               = LessEqual;
        StencilEnable       = False;
        DepthBias           = 0.0f;
        PixelShader         = 0;
        SpecularEnable	    = False;

        FogColor            = {0.5, 0.5, 0.5, 1.0};
        FogDensity          = 0.001f;
        FogVertexMode       = EXP;
        FogStart            = 200;
        FogEnd              = 1000;

        Ambient          = <LightAmbient>;
        LightEnable[0]   = True;	
        LightAmbient[0]  = <LightAmbient>;
        LightDiffuse[0]  = <LightDiffuse>;
        LightSpecular[0] = <LightSpecular>;
        LightPosition[0] = <LightPos>;
        LightRange[0]    = 500000.0;
        LightAttenuation0[0] = 1.0;
        LightAttenuation1[0] = 0.0;
        LightAttenuation2[0] = 0.0;

        // LightType must be the last light state that is set!
        LightType[0] = POINT;

        // FillMode         = Wireframe;
    }
}

