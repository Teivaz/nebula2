/************* TWEAKABLES **************/

float4x4 worldIT : WorldIT;
float4x4 ModelViewProjection : WorldViewProjection;
float4x4 Model : World;
float4x4 View : View;
float4x4 viewIT : ViewIT;

texture DiffMap0 : DiffuseMap;

float4 ModelLightPos : Position = {100.0f, 100.0f, 100.0f, 0.0f};

/////////////////

float4 MatDiffuse : Diffuse = {0.5f, 0.4f, 0.05f, 1.0f};
float4 coolColor : Diffuse = {0.05f, 0.05f, 0.6f, 1.0f};
float4 liteColor : Diffuse = {0.8f, 0.5f, 0.1f, 1.0f};
float4 darkColor : Diffuse = {0.0f, 0.0f, 0.0f, 1.0f};

/************* DATA STRUCTS **************/

/* data from application vertex buffer */
struct appdata {
    float3 Position	: POSITION;
    float4 UV		: TEXCOORD0;
    float4 Normal	: NORMAL;
};

/* data passed from vertex shader to pixel shader */
struct vertexOutput {
    float4 HPosition	: POSITION;
    float4 TexCoord0	: TEXCOORD0;
    float4 diffCol	: COLOR0;
    float4 specCol	: COLOR1;
};

/* Output pixel values */
struct pixelOutput {
  float4 col : COLOR;
};

/*********** vertex shader ******/

vertexOutput goochVS(appdata IN,
    uniform float4x4 WorldViewProj,
    uniform float4x4 WorldIT,
    uniform float4x4 World,
    uniform float4x4 ViewIT,
    uniform float4 LiteColor,
    uniform float4 DarkColor,
    uniform float4 WarmColor,
    uniform float4 CoolColor,
    uniform float3 LightPos
) {
    vertexOutput OUT;
    float3 Nn = mul(WorldIT, IN.Normal).xyz;
    Nn = normalize(Nn);

    float4 Po = float4(IN.Position.x,IN.Position.y,IN.Position.z,1.0);
    float3 Pw = mul(World, Po).xyz;
    float3 Ln = normalize(LightPos - Pw);
    float mixer = 0.5 * (dot(Ln,Nn) + 1.0);

    float4 surfColor = lerp(DarkColor,LiteColor,mixer);
    float4 toneColor = lerp(CoolColor,WarmColor,mixer);
    float4 mixColor = surfColor + toneColor;
    mixColor.w = 1.0;

    OUT.diffCol = mixColor;

    OUT.specCol = float4(0.0,0.0,0.0,1.0);	// not actually used
    OUT.TexCoord0 = IN.UV;

    OUT.HPosition = mul(Po, WorldViewProj);

    //OUT.HPosition = mul(float4(IN.Position,0.0), WorldViewProj);
    return OUT;
}

/********* pixel shader ********/

pixelOutput goochPS(vertexOutput IN,
    uniform sampler2D ColorMap
) {
    pixelOutput OUT; 
    float4 result = IN.diffCol * tex2D(ColorMap);
    OUT.col = result;
    return OUT;
}

/*************/

technique PixelShaderVersion
{
    pass p0 
    {		
        Texture[0] = <DiffMap0>;
  	Target[0] = Texture2D;
  	MinFilter[0] = Linear;
  	MagFilter[0] = Linear;
  	MipFilter[0] = Linear;

	VertexShader = compile vs_1_1 goochVS(ModelViewProjection,worldIT,Model,viewIT,
				liteColor,darkColor,
				MatDiffuse,coolColor,ModelLightPos);
        //PixelShader = compile ps_1_1 goochPS(DiffMap0);
	ZEnable = true;
	ZWriteEnable = true;
	CullMode = 2;
    }
}

technique textured
{
    pass p0
    {
        Texture[0] = <DiffMap0>;
  	Target[0] = Texture2D;
  	MinFilter[0] = Linear;
  	MagFilter[0] = Linear;
  	MipFilter[0] = Linear;

	//VertexShader = compile vs_1_1 goochVS(ModelViewProjection,worldIT,Model,View,liteColor,darkColor,MatDiffuse,coolColor,ModelLightPos);

        //PixelShader = compile ps_1_1 goochPS(DiffMap0);
	ZEnable = true;
	ZWriteEnable = true;
	CullMode = 2;

    }
}

technique NoPixelShaderVersion
{
    pass p0 
    {		
	    VertexShader = compile vs_1_1 goochVS(ModelViewProjection,worldIT,Model,View,
				    liteColor,darkColor,
				    MatDiffuse,coolColor,ModelLightPos);

	    ZEnable = true;
	    ZWriteEnable = true;
	    CullMode = None;

        SpecularEnable = false;

        ColorArg1[ 0 ] = Diffuse;
        ColorOp[ 0 ]   = SelectArg1;
        ColorArg2[ 0 ] = Specular;

        AlphaArg1[ 0 ] = Diffuse;
        AlphaOp[ 0 ]   = SelectArg1;
    }
}

/***************************** eof ***/