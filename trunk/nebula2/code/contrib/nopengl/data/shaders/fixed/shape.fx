#line 1 "shape.fx"
//------------------------------------------------------------------------------
//  fixed/shape.fx
//
//  Used for debug visualization.
//  
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
uniform float4x4 Model;
uniform float4x4 View;                   // the view matrix
uniform float4x4 Projection;             // the projection matrix
uniform float3 LightPos;             // the light's position in world space

float4x4 TextureTransform0 = {1.0f, 0.0f, 0.0f, 0.0f,
                              0.0f, 1.0f, 0.0f, 0.0f, 
                              0.0f, 0.0f, 1.0f, 0.0f,
                              0.0f, 0.0f, 0.0f, 1.0f };
float4 MatDiffuse;

//------------------------------------------------------------------------------
technique t0
{
    pass p0
    {
        WorldTransform[0]   = <Model>;
        ViewTransform       = <View>;
    	ProjectionTransform = <Projection>;    
    	TextureTransform[0] = <TextureTransform0>;
    	
        ColorWriteEnable    = RED|GREEN|BLUE|ALPHA;  
        NormalizeNormals    = True;
        ZEnable             = True;
        ZFunc               = LessEqual;
        StencilEnable       = False;
        DepthBias           = 0.0f;
        PixelShader         = 0;
        SpecularEnable	    = False;

        Ambient          = {0.3, 0.3, 0.3, 1.0f};
        LightEnable[0]   = True;	
        LightAmbient[0]  = {0.3, 0.3, 0.3, 1.0f};
        LightDiffuse[0]  = {1.0, 1.0, 1.0, 1.0f};
        LightSpecular[0] = {1.0, 1.0, 1.0, 1.0f};
        LightPosition[0] = <LightPos>;
        LightRange[0]    = 500000.0;
        LightAttenuation0[0] = 1.0;
        LightAttenuation1[0] = 0.0;
        LightAttenuation2[0] = 0.0;

        // LightType must be the last light state that is set!
        LightType[0] = POINT;

        ZWriteEnable     = False;
        AlphaBlendEnable = True;
        AlphaTestEnable  = False;
        Lighting         = True;
        VertexShader     = 0;
        FogEnable        = False;

        CullMode            = Cw;
        SrcBlend            = SrcAlpha;
        DestBlend           = InvSrcAlpha;
        MaterialDiffuse     = <MatDiffuse>;
        MaterialAmbient     = {1.0f, 1.0f, 1.0f, 1.0f};

        // FVF = XYZ | NORMAL | TEX1;
        
        Texture[0] = 0;
        TexCoordIndex[0] = 0;
        TextureTransformFlags[0] = Count2;
   
        ColorOp[0]   = SelectArg1;
        ColorArg1[0] = Current;
        
        ColorOp[1]   = Disable;
        
        //FillMode         = Wireframe;        
    }
}

