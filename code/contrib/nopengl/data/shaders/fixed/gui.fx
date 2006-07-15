#line 1 "gui.fx"
//------------------------------------------------------------------------------
//  gui.fx
//
//  A 2d rectangle shader for GUI rendering.
//------------------------------------------------------------------------------
uniform float4x4 Model : Model;
uniform float4x4 View : View;
uniform float4x4 Projection : Projection;

texture DiffMap0 : DiffuseMap;
float4 MatDiffuse : MaterialDiffuse;
float4 matAmbient : MaterialAmbient = { 1.0, 1.0, 1.0, 1.0 };
float4 matSpecular : MaterialSpecular = { 0.0, 0.0, 0.0, 0.0 };

//------------------------------------------------------------------------------
//  Texture sampler definitions
//------------------------------------------------------------------------------
sampler2D DiffSampler = sampler_state
{
    Texture = <DiffMap0>;
    AddressU = Clamp;
    AddressV = Clamp;
    MinFilter = Point;
    MagFilter = Point;
    MipFilter = None;
};

//------------------------------------------------------------------------------
technique t0
{
    pass p0
    {
        //WorldTransform[0] = <Model>;
        //ViewTransform = <View>;
    	//ProjectionTransform = <Projection>;

        ZWriteEnable     = false;
        ZEnable          = false;
        // ZFunc            = Always;
        ColorWriteEnable = true; //RED|GREEN|BLUE|ALPHA;        
        AlphaBlendEnable = true;
        SrcBlend         = SrcAlpha;
        DestBlend        = InvSrcAlpha;
        //AlphaTestEnable  = false;

        CullMode = None;

        StencilEnable = false;
        StencilZFail = Keep;
        StencilPass =  Keep;

        //VertexShader = 0;
        //PixelShader  = 0;

        Texture[0] = <DiffSampler>;
        Target[0] = Texture2D;
        MinFilter[0] = Point;
        MagFilter[0] = Point;
        MipFilter[0] = None;

        Lighting = true;
        //LightEnable[0] = false;
        SpecularEnable = false;
        //MaterialDiffuse = <MatDiffuse>;
        //MaterialAmbient = { 1.0, 1.0, 1.0, 1.0 };
        //MaterialSpecular = { 0.0, 0.0, 0.0, 0.0 };
        //Ambient = <MatDiffuse>;

        // FVF = XYZ | TEX1;
        
        TexCoordIndex[0] = 0;
        TextureTransformFlags[0] = Disable;
   
        ColorOp[0]   = Modulate;
        ColorArg1[0] = Texture;
        ColorArg2[0] = Diffuse;
        
        ColorOp[1]   = Disable;
        
        AlphaOp[0]   = Modulate;
        AlphaArg1[0] = Texture;
        AlphaArg2[0] = Diffuse;
        
        AlphaOp[1] = Disable;
    }
}

