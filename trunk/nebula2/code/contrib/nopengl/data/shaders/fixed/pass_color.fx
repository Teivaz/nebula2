#line 1 "pass_color.fx"
//------------------------------------------------------------------------------
//  pass_color.fx
//
//  Set render states which are constant for the entire color pass.
//
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
uniform float4x4 View : View;                   // the view matrix
uniform float4x4 Projection : Projection;             // the projection matrix
uniform float3 LightPos : Position;             // the light's position in world space
uniform float4 LightAmbient : Ambient;         // light's ambient component
uniform float4 LightDiffuse : Diffuse;         // light's diffuse component
uniform float4 LightSpecular : Specular;        // light's specular component

technique t0
{
    pass p0
    {
        //ViewTransform       = <View>;
    	//ProjectionTransform = <Projection>;    
        ColorWriteEnable    = RED|GREEN|BLUE|ALPHA;  
        NormalizeNormals    = true;
        ZEnable             = true;
        ZFunc               = LessEqual;
        StencilEnable       = false;
        DepthBias           = 0.0f;
        PixelShader         = 0;
        SpecularEnable	    = false;

        FogColor            = {0.5, 0.5, 0.5, 1.0};
        FogDensity          = 0.001f;
        FogVertexMode       = EXP;
        FogStart            = 200;
        FogEnd              = 1000;

        //Ambient          = <LightAmbient>;
        //LightEnable[0]   = true;	
        //LightAmbient[0]  = <LightAmbient>;
        //LightDiffuse[0]  = <LightDiffuse>;
        //LightSpecular[0] = <LightSpecular>;
        //LightPosition[0] = <LightPos>;
        //LightRange[0]    = 500000.0;
        //LightAttenuation0[0] = 1.0;
        //LightAttenuation1[0] = 0.0;
        //LightAttenuation2[0] = 0.0;

        // LightType must be the last light state that is set!
        //LightType[0] = POINT;

        // FillMode         = Wireframe;
    }
}

