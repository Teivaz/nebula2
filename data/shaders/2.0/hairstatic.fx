
// -------------- TYPEDEFS -------------
typedef half3 color3;
typedef half4 color4;

shared float4x4 ModelViewProjection;
shared float3 ModelEyePos;


// -------------- VARIABLES -------------
float MatSpecularPower;
float3 ModelLightPos;
int CullMode = 2;
bool AlphaBlendEnable = true;
int AlphaSrcBlend = 5;
int AlphaDstBlend = 6;

// -------------- TEXTURES -------------
texture DiffMap0;
texture DiffMap1;

// -------------- SAMPLERS -------------
sampler DiffSampler = sampler_state
{
	Texture = <DiffMap0>;
	AddressU = Wrap;
	AddressV = Wrap;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear;
	MipMapLodBias = -0.75;
};

sampler Diff1Sampler = sampler_state
{
    Texture = <DiffMap1>;
    AddressU  = Wrap;
    AddressV  = Wrap;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
    MipMapLodBias = -0.75;
};

struct vsInputHair
{
	float4 position	:   POSITION;
   	float3 Normal	:  	NORMAL;
   	float2 uv0		:   TEXCOORD0;
   	float3 Tangent	: 	TANGENT;
};

struct vsOutputHair 
{
 	float4 position:     POSITION;
   	float2 uv0:      TEXCOORD0;
   	float3 Tangent: TEXCOORD1;
   	float3 viewDir: TEXCOORD2;

};



color4 EncodeHDR(in color4 rgba)
{
    return rgba * color4(0.5, 0.5, 0.5, 1.0);
}

vsOutputHair vsHairColor(vsInputHair In)
{
	vsOutputHair Out;
    Out.position     = mul(In.position, ModelViewProjection);
    Out.Tangent = In.Tangent;
    // compute normalized vector from vertex to camera
    Out.viewDir = normalize(ModelEyePos - In.position);
    Out.uv0 = In.uv0;
    return Out;
}

float4 psHairColor(vsOutputHair In, uniform bool hdr) : COLOR 
{
	// FIXME: Achtung: dieser Shader klappt nur fuer Directional Lights!
	// In ModelLightPos steht fuer eine Punktlichtquelle eine echte
	// Position, bei einer Directional-Lichtquelle aber ein Vektor!

   float dotTL = dot (In.Tangent, ModelLightPos);
   float dotTV = dot (In.Tangent, normalize (In.viewDir));

   float sinTL = sqrt (1 - dotTL*dotTL);
   float sinTV = sqrt (1 - dotTV*dotTV);
   
   color3 Kd = tex2D (DiffSampler, In.uv0).rgb;
   color3 Ks = tex2D (Diff1Sampler, In.uv0).rgb;
   
   float3 diffuse = Kd * sinTL;
   float3 specular = Ks * pow (dotTL*dotTV + sinTL*sinTV, MatSpecularPower);
   
   float4 color = float4(diffuse + specular, 0);
   color.rgb *= tex2D (Diff1Sampler, In.uv0.yx).a;
   color.a = tex2D (DiffSampler, In.uv0).a;
   
   if(hdr)
   {
   		return EncodeHDR(color);
   }
   else
   {
	   return color;
   }
}

technique tStaticHairColor
{
	pass p0
	{
		CullMode = <CullMode>;
		SrcBlend     = <AlphaSrcBlend>;
        DestBlend    = <AlphaDstBlend>;
		VertexShader = compile VS_PROFILE vsHairColor();
		PixelShader = compile PS_PROFILE psHairColor(false);
	}
}

technique tStaticHairColorHDR
{
	pass p0
	{
		CullMode = <CullMode>;
		SrcBlend     = <AlphaSrcBlend>;
        DestBlend    = <AlphaDstBlend>;
		VertexShader = compile VS_PROFILE vsHairColor();
		PixelShader = compile PS_PROFILE psHairColor(true);
	}
}

technique tStaticHairNoAlphaColor
{
	pass p0
	{
		CullMode = <CullMode>;
		VertexShader = compile VS_PROFILE vsHairColor();
		PixelShader = compile PS_PROFILE psHairColor(false);
	}
}

technique tStaticHairNoAlphaColorHDR
{
	pass p0
	{
		CullMode = <CullMode>;
		VertexShader = compile VS_PROFILE vsHairColor();
		PixelShader = compile PS_PROFILE psHairColor(true);
	}
}