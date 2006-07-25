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
matrix<float,4,3> JointPalette[72];

shared float4x4 View;           // the view matrix
shared float4x4 Projection;     // the projection matrix



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
	float4 weights  : 	BLENDWEIGHT;
    float4 indices  : 	BLENDINDICES;
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

//------------------------------------------------------------------------------
/**
    skinnedNormal()
    
    Compute a skinned normal vector (without renormalization).
    
    @param  inNormal        input normal vector
    @param  weights         4 weights
    @param  indices         4 joint indices into the joint palette
    @param  jointPalette    joint palette as vector4 array
    @return                 the skinned normal
*/
float3
skinnedNormal(const float3 inNormal, const float4 weights, const float4 indices, const matrix<float,4,3> jointPalette[72])
{
    float3 normal[4];
    int i;
    for (i = 0; i < 4; i++)
    {
        normal[i] = mul(inNormal, (matrix<float,3,3>)jointPalette[indices[i]]) * weights[i];
    }
    return float3(normal[0] + normal[1] + normal[2] + normal[3]);
}
//------------------------------------------------------------------------------
/**
    skinnedPosition()
    
    Compute a skinned position.
    
    @param  inPos           input vertex position
    @param  weights         4 weights
    @param  indices         4 joint indices into the joint palette
    @param  jointPalette    joint palette as vector4 array
    @return                 the skinned position
*/
float4
skinnedPosition(const float4 inPos, const float4 weights, const float4 indices, const matrix<float,4,3> jointPalette[72])
{
	float3 pos[4];
    
    int i;
    for (i = 0; i < 4; i++)
    {
        pos[i] = (mul(inPos, jointPalette[indices[i]])) * weights[i];
    }
    return float4(pos[0] + pos[1] + pos[2] + pos[3], 1.0f);
    
}

color4 EncodeHDR(in color4 rgba)
{
    return rgba * color4(0.5, 0.5, 0.5, 1.0);
}

vsOutputHair vsHairColor(vsInputHair In)
{
	vsOutputHair Out;

    // get skinned position, normal and tangent
    float4 skinPos     = skinnedPosition(In.position, In.weights, In.indices, JointPalette);
    Out.position     = mul(skinPos, ModelViewProjection);
    float4 ModelViewPos = Out.position;
    Out.Tangent = skinnedNormal(In.Tangent, In.weights, In.indices, JointPalette);
    // compute normalized vector from vertex to camera
    Out.viewDir = normalize(ModelEyePos - In.position);
    Out.uv0 = In.uv0;
    return Out;
}

float4 psHairColor(vsOutputHair In, uniform bool hdr) : COLOR 
{
   float dotTL = dot (In.Tangent, ModelLightPos);
   float dotTV = dot (In.Tangent, normalize (In.viewDir));

   float sinTL = sqrt (1 - dotTL*dotTL);
   float sinTV = sqrt (1 - dotTV*dotTV);
   
   color3 Kd = tex2D (DiffSampler, In.uv0).rgb;
   color3 Ks = tex2D (Diff1Sampler, In.uv0).rgb;
   
   
   
   float3 diffuse = Kd * sinTL;
   float3 specular = Ks * pow (dotTL*dotTV + sinTL*sinTV, MatSpecularPower);
   
   float4 color = float4(diffuse + specular, 0);
   color.rgb *= tex2D (Diff1Sampler, In.uv0).a;
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

technique tHairColor
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

technique tHairColorHDR
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

technique tHairNoAlphaColor
{
	pass p0
	{
		CullMode = <CullMode>;
		VertexShader = compile VS_PROFILE vsHairColor();
		PixelShader = compile PS_PROFILE psHairColor(false);
	}
}

technique tHairNoAlphaColorHDR
{
	pass p0
	{
		CullMode = <CullMode>;
		VertexShader = compile VS_PROFILE vsHairColor();
		PixelShader = compile PS_PROFILE psHairColor(true);
	}
}