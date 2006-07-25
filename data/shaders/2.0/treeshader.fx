//------------------------------------------------------------------------------
//  shaders.fx
//
//  Nebula2 standard shaders for shader model 2.0
//
//  (C) 2005 Radon Labs GmbH
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/**
    shader parameters

    NOTE: VS_PROFILE and PS_PROFILE macros are usually provided by the
    application and contain the highest supported shader models.
*/    
/*
#define VS_PROFILE vs_2_0
#define PS_PROFILE ps_2_0
*/

#define DIRLIGHTS_ENABLEOPPOSITECOLOR 1
#define DIRLIGHTS_OPPOSITECOLOR float3(0.1f, 0.15f, 0.2f)
#define DEBUG_LIGHTCOMPLEXITY 0
#define INNERLIGHTINTENSITY 0.01f
#define OUTERLIGHTINTENSITY 1.0f

#define DECLARE_SCREENPOS(X) float4 screenPos : X;
#define PS_SCREENPOS psComputeScreenCoord(psIn.screenPos)
#define VS_SETSCREENPOS(X) vsOut.screenPos = vsComputeScreenCoord(X);
#define PI 3.141592654f

typedef half3 color3;
typedef half4 color4;

shared float4   HalfPixelSize;
shared float4x4 ModelViewProjection;
shared float3   ModelEyePos;
shared float3   EyePos;
shared float4x4 Light;
shared float4x4 Model;
shared float4x4 View;
shared float4x4 Projection;

shared float Time;
float3 ModelLightPos;

shared float3   EyeDir;
shared float3   LightPos;
shared int      LightType;
shared float    LightRange;
shared float4   LightDiffuse;
shared float4   LightSpecular;
shared float4   LightAmbient;
shared float4   ShadowIndex;

float BumpScale = 0.0f;
float4 MatDiffuse;
float4 MatEmissive = float4(0.0f, 0.0f, 0.0f, 0.0f);
float  MatEmissiveIntensity = 0.0f;
float4 MatSpecular;
float4 MatAmbient;
float Intensity0;
float  MatSpecularPower;
float Scale;

int AlphaRef;
int CullMode = 2;
float3 BoxCenter;
float3 BoxMinPos;                   // model space bounding box min
float3 BoxMaxPos;                   // model space bounding box max
float4 Wind;                        // wind direction, strength in .w
float3x3 Swing;                     // the swing rotation matrix

texture DiffMap0;
texture DiffMap1;
texture BumpMap0;
texture BumpMap1;
texture SpecMap0;
shared texture AmbientMap1;
bool  AlphaBlendEnable = false;

// leaf table
float4	LeafCluster[192];

// speedwind
float4x4	WindMatrices[4];
float4x4	LeafAngleMatrices[6];

#include "shaders:../lib/lib.fx"
#include "shaders:../lib/randtable.fx"

//------------------------------------------------------------------------------
/**
    shader input/output structure definitions
*/

struct vsInputStaticDepth
{
    float4 position : POSITION;
    float2 windAttribs 	: TEXCOORD2;
};

struct vsOutputStaticDepth
{
    float4 position : POSITION;
    float  depth : TEXCOORD0;
};

struct vsInputUvDepth
{
    float4 position : POSITION;
    float2 uv0 : TEXCOORD0;
    float2 windAttribs 	: TEXCOORD2;
};

struct vsInputStaticUvDepth
{
    float4 position : POSITION;
    float2 uv0 : TEXCOORD0;
};

struct vsOutputUvDepth
{
    float4 position : POSITION;
    float3 uv0depth : TEXCOORD0;
};

struct vsInputBranchColor
{
    float4 position 	: POSITION;
    float3 normal   	: NORMAL;
    float2 uv0      	: TEXCOORD0;
    float2 uv1      	: TEXCOORD1;
    float2 windAttribs 	: TEXCOORD2;
    float3 tangent  	: TANGENT;
    float3 binormal 	: BINORMAL;
};

struct vsOutputBranchColor
{
    float4 position      : POSITION;
    float2 uv0           : TEXCOORD0;
    float3 lightVec      : TEXCOORD1;
    float3 modelLightVec : TEXCOORD2;
    float3 halfVec       : TEXCOORD3;
    float3 eyePos        : TEXCOORD4;
    DECLARE_SCREENPOS(TEXCOORD5)
};

struct vsOutputFrondColor
{
    float4 position      : POSITION;
    float2 uv0           : TEXCOORD0;
    float3 lightVec      : TEXCOORD1;    
    float3 eyeVec        : TEXCOORD2;
    float3 normal        : TEXCOORD3;
    float selfShadow     : TEXCOORD4;
    DECLARE_SCREENPOS(TEXCOORD5)
};

struct vsInputLeavesDepth
{
	float4 position				: POSITION; 
	float3 normal				: NORMAL;
	float2 uv0					: TEXCOORD0;
	float2 windAttribs			: TEXCOORD1;
	float3 leafAttribs			: TANGENT;
};

struct vsInputLeavesColor
{
	float4 position				: POSITION; 
	float3 normal				: NORMAL;
	float2 uv0					: TEXCOORD0;
	float2 windAttribs			: TEXCOORD1;
	float3 leafAttribs			: TANGENT; // need float3 for leafAttribs
};

struct vsOutputLeavesColor
{
    float4	position		: POSITION;
    float4	color			: TEXCOORD0;
    float2	uv0				: TEXCOORD1;
    float3	lightVec     	: TEXCOORD2;
};

struct vsInputBillboardColor
{
	float4 position : POSITION;
	float2 uv0 		: TEXCOORD0;		
};

struct vsOutputBillboardColor
{
	float4 position : POSITION;
	float2 uv0 		: TEXCOORD0;	
	float3 pos 		: TEXCOORD1;
	float3 eyeVec 	: TEXCOORD2;
	float3 lightVec : TEXCOORD3;		
};

struct vsInputClusterColor
{
	float4 position : POSITION;
	float3 modelPos : NORMAL;
	float2 uv0 		: TEXCOORD0;
};
//------------------------------------------------------------------------------
/**
    sampler definitions
*/
sampler DiffSampler = sampler_state
{
    Texture = <DiffMap0>;
    AddressU  = Wrap;
    AddressV  = Wrap;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
    MipMapLodBias = -0.75;
};

sampler BillboardSampler = sampler_state
{
    Texture = <DiffMap0>;
    AddressU  = Wrap;
    AddressV  = Wrap;
    //BorderColor = float4(0,0,0,0);
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
    //MipMapLodBias = -0.75;
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

sampler BumpSampler = sampler_state
{
    Texture   = <BumpMap0>;
    AddressU  = Wrap;
    AddressV  = Wrap;
    MinFilter = Point;
    MagFilter = Linear;
    MipFilter = Linear;
    MipMapLodBias = -0.75;
};

sampler ShadowSampler = sampler_state
{
    Texture = <AmbientMap1>;
    AddressU  = Clamp;
    AddressV  = Clamp;
    MinFilter = Point;
    MagFilter = Point;
    MipFilter = None;
};

sampler NoiseSampler = sampler_state
{
    Texture = <SpecMap0>;
    AddressU  = Wrap;
    AddressV  = Wrap;
    MinFilter = Point;
    MagFilter = Linear;
    MipFilter = Linear;
    MipMapLodBias = -0.75;
};

//==============================================================================
//  Helper functions
//==============================================================================

//------------------------------------------------------------------------------
/**
    Scale down pseudo-HDR-value into RGB8.
*/
/*
float4 EncodeHDR(in float4 rgba)
{
    const float colorSpace = 0.8;
    const float maxHDR = 8;
    const float hdrSpace = 1 - colorSpace;
    const float hdrPow = 10;
    const float hdrRt = 0.1;
    
    float3 col = clamp(rgba.rgb,0,1) * colorSpace;
    float3 hdr = pow(clamp(rgba.rgb,1,10),hdrRt)-1;
    float4 result;
    hdr = clamp(hdr, 0, hdrSpace);
    result.rgb = col + hdr;
    result.a = rgba.a;
    return result;
}
*/

color4 EncodeHDR(in color4 rgba)
{
    return rgba * color4(0.5, 0.5, 0.5, 1.0);
}

//------------------------------------------------------------------------------
/**
    Vertex shader part to compute texture coordinate shadow texture lookups.
*/
float4 vsComputeScreenCoord(float4 pos)
{
    return pos;
}

//------------------------------------------------------------------------------
/**
    Pixel shader part to compute texture coordinate shadow texture lookups.
*/
float2 psComputeScreenCoord(float4 pos)
{
    float2 screenCoord = ((pos.xy / pos.ww) * float2(0.5, -0.5)) + float2(0.5f, 0.5f);
    screenCoord += HalfPixelSize.xy;
    return screenCoord;
}

//------------------------------------------------------------------------------
/**
    Vertex shader part for per-pixel-lighting. Computes the light
    and half vector in tangent space which are then passed to the
    interpolators.
*/
void vsLight(in float3 position, 
             in float3 normal, 
             in float3 tangent,
             in float3 binormal,
             in float3 modelEyePos,
             in float3 modelLightPos,
             out float3 tangentLightVec,
             out float3 modelLightVec,
             out float3 halfVec,
             out float3 tangentEyePos)
{
    float3 eVec = normalize(modelEyePos - position);
    if (LightType == 0)
    {
        // point light
        modelLightVec = modelLightPos - position;
    }
    else
    {
        // directional light
        modelLightVec = modelLightPos;
    }
    float3 hVec = normalize(normalize(modelLightVec) + eVec);
    float3x3 tangentMatrix = float3x3(tangent, binormal, normal);
    tangentLightVec = mul(tangentMatrix, modelLightVec);
    halfVec = mul(tangentMatrix, hVec);
    tangentEyePos = normalize(mul(tangentMatrix, eVec));
}

//------------------------------------------------------------------------------
/**
    Compute shadow value. Returns a shadow intensity between 1 (not in shadow)
    and 0 (fully in shadow)
*/
half psShadow(in float2 screenCoord)
{
    // determine if pixel is in shadow
    color4 shadow = tex2D(ShadowSampler, screenCoord);
    half shadowValue = 1.0 - saturate(length(ShadowIndex * shadow));
    return shadowValue;    
}

//------------------------------------------------------------------------------
/**
    Compute per-pixel lighting.
    
    NOTE: lightVec.w contains the distance to the light source
*/
color4 psBranchLight(in color4 mapColor, in float3 tangentSurfaceNormal, in float3 lightVec, in float3 modelLightVec, in float3 halfVec, in half shadowValue)
{
    color4 color = mapColor * color4(LightAmbient.rgb + MatEmissive.rgb * MatEmissiveIntensity, MatDiffuse.a) * Intensity0;
        
    // light intensities
    //half specIntensity = pow(saturate(dot(tangentSurfaceNormal, normalize(halfVec))), MatSpecularPower); // Specular-Modulation * mapColor.a;
    half diffIntensity = dot(tangentSurfaceNormal, normalize(lightVec));

    
    color3 diffColor = mapColor.rgb * LightDiffuse.rgb * MatDiffuse.rgb;
//     color3 specColor = specIntensity * LightSpecular.rgb * MatSpecular.rgb;

    // attenuation
    if (LightType == 0)
    {
        // point light source
        diffIntensity *= shadowValue * (1.0f - saturate(length(modelLightVec) / LightRange));
    }
    else
    {
        #if DIRLIGHTS_ENABLEOPPOSITECOLOR
        color.rgb += saturate(1-diffIntensity) * DIRLIGHTS_OPPOSITECOLOR * mapColor.rgb * MatDiffuse.rgb;
        #endif    
        diffIntensity *= shadowValue;
    }    
    color.rgb += saturate(diffIntensity) * (diffColor.rgb /*+ specColor*/);
    return color;
}


//------------------------------------------------------------------------------
/**
    Compute per-pixel lighting.
    
    NOTE: lightVec.w contains the distance to the light source
*/
color4 psFrondLight(in color4 mapColor, in float3 normal, in float3 lightVec, in float3 eyeVec, in float selfShadow, in half shadowValue)
{
    color4 color = mapColor * color4(LightAmbient.rgb + MatEmissive.rgb * MatEmissiveIntensity, MatDiffuse.a) * Intensity0;
        
    // light intensities
    half diffIntensity = dot(normalize(normal), normalize(lightVec));
    
    color3 diffColor = mapColor.rgb * LightDiffuse.rgb * MatDiffuse.rgb;

    // attenuation
    if (LightType == 0)
    {
        // point light source
        diffIntensity *= shadowValue * (1.0f - saturate(length(lightVec) / LightRange));
    }
    else
    {
        #if DIRLIGHTS_ENABLEOPPOSITECOLOR
        color.rgb += saturate(1-diffIntensity) * DIRLIGHTS_OPPOSITECOLOR * mapColor.rgb * MatDiffuse.rgb;
        #endif    
        diffIntensity *= shadowValue;
    }    
    color.rgb +=  saturate(selfShadow * diffIntensity) * (diffColor.rgb);
    return color;
}
//------------------------------------------------------------------------------
/**
    Returns the uv offset for parallax mapping
*/
half2 ParallaxUv(float2 uv, sampler bumpMap, float3 eyeVect)
{
    float amount = BumpScale / 1000.0f;
    return ((tex2D(bumpMap, uv).a) * amount - amount * 0.5) * eyeVect;
}

//==============================================================================
//  Vertex and pixel shader functions.
//==============================================================================

//------------------------------------------------------------------------------
/**
    Vertex shader: generate depth values for static geometry.
*/
vsOutputStaticDepth vsBranchDepth(const vsInputStaticDepth vsIn)
{
    vsOutputStaticDepth vsOut;
	float4 windyPoint = lerp(vsIn.position,										// interpolate between original position
						mul(vsIn.position, WindMatrices[vsIn.windAttribs.x]),	//	and full wind-blown position
						vsIn.windAttribs.y);	
    vsOut.position = mul(windyPoint, ModelViewProjection);
    vsOut.depth = vsOut.position.z;
    return vsOut;
}

//------------------------------------------------------------------------------
/**
    Vertex shader: generate depth values for static geometry with uv 
    coordinates.
*/
vsOutputUvDepth vsLeafUvDepth(const vsInputLeavesDepth vsIn)
{
    vsOutputUvDepth vsOut;
    
   	//float posLen = length(vsIn.position);
    float4 windyPoint = lerp(vsIn.position,										// interpolate between original position
						mul(vsIn.position, WindMatrices[vsIn.windAttribs.x]),	//	and full wind-blown position
						vsIn.windAttribs.y);
	//windyPoint.xyz = normalize(windyPoint.xyz).xyz * posLen;									//	using the wind weight

    windyPoint.xyz = mul(windyPoint.xyz, Swing); 	
	windyPoint.xyz += mul(LeafCluster[vsIn.leafAttribs.x] * vsIn.leafAttribs.y,
 						LeafAngleMatrices[vsIn.leafAttribs.z]);						// leaf placement using SpeedWind leaf rocking

	vsOut.position = mul(windyPoint, ModelViewProjection);	
    vsOut.uv0depth.xy = vsIn.uv0;
    vsOut.uv0depth.z = vsOut.position.z;
    return vsOut;
}

//------------------------------------------------------------------------------
/**
*/
color4 psBranchDepth(const vsOutputStaticDepth psIn) : COLOR
{
    return float4(psIn.depth, 0.0f, 0.0f, 1.0f);
}

//------------------------------------------------------------------------------
/**
    Pixel shader: generate depth values for static geometry with
    alpha test.
*/
color4 psBranchDepthATest(const vsOutputUvDepth psIn) : COLOR
{
    float alpha = tex2D(DiffSampler, psIn.uv0depth.xy).a;
    clip(alpha - (AlphaRef / 256.0f));
    return float4(psIn.uv0depth.z, 0.0f, 0.0f, alpha);
}

//------------------------------------------------------------------------------
/**
    Vertex shader: generate depth values for static geometry with uv 
    coordinates.
*/
vsOutputUvDepth vsWindUvDepth(const vsInputUvDepth vsIn)
{
    vsOutputUvDepth vsOut;
   	//float posLen = max(length(vsIn.position),0);
    float4 windyPoint = lerp(vsIn.position,										// interpolate between original position
						mul(vsIn.position, WindMatrices[vsIn.windAttribs.x]),	//	and full wind-blown position
						vsIn.windAttribs.y);	
	//windyPoint.xyz = normalize(windyPoint.xyz).xyz * posLen;									//	using the wind weight			
    vsOut.position = mul(windyPoint, ModelViewProjection);	
    vsOut.uv0depth.xy = vsIn.uv0;
    vsOut.uv0depth.z = vsOut.position.z;
    return vsOut;
}

//------------------------------------------------------------------------------
/**
    Vertex shader: generate depth values for static geometry with uv 
    coordinates.
*/
vsOutputUvDepth vsStaticUvDepth(const vsInputStaticUvDepth vsIn)
{
    vsOutputUvDepth vsOut;
    
	vsOut.position = mul(vsIn.position, ModelViewProjection);						// project to screen
    vsOut.uv0depth.xy = vsIn.uv0;
    vsOut.uv0depth.z = vsOut.position.z;
    return vsOut;
}
//------------------------------------------------------------------------------
/**
    Vertex shader: generate color values for static geometry.
*/
vsOutputBranchColor vsBranchColor(const vsInputBranchColor vsIn)
{
    vsOutputBranchColor vsOut;
   	//float posLen = max(length(vsIn.position),0);
    float4 windyPoint = lerp(vsIn.position,										// interpolate between original position
						mul(vsIn.position, WindMatrices[vsIn.windAttribs.x]),	//	and full wind-blown position
						vsIn.windAttribs.y);	
	//windyPoint.xyz = normalize(windyPoint.xyz).xyz * posLen;									//	using the wind weight			
    vsOut.position = mul(windyPoint, ModelViewProjection);
    VS_SETSCREENPOS(vsOut.position);
    vsOut.uv0 = vsIn.uv0;
    vsLight(vsIn.position, vsIn.normal, vsIn.tangent, vsIn.binormal, ModelEyePos, ModelLightPos, vsOut.lightVec, vsOut.modelLightVec, vsOut.halfVec, vsOut.eyePos);
    return vsOut;
}

//------------------------------------------------------------------------------
/**
    Vertex shader: generate color values for static geometry.
*/
vsOutputFrondColor vsFrondColor(const vsInputBranchColor vsIn)
{
    vsOutputFrondColor vsOut;
   	//float posLen = max(length(vsIn.position),0);
    float4 windyPoint = lerp(vsIn.position,										// interpolate between original position
						mul(vsIn.position, WindMatrices[vsIn.windAttribs.x]),	//	and full wind-blown position
						vsIn.windAttribs.y);	
	//windyPoint.xyz = normalize(windyPoint.xyz).xyz * posLen;									//	using the wind weight		
	vsOut.eyeVec = normalize(ModelEyePos - windyPoint);	
	
    vsOut.position = mul(windyPoint, ModelViewProjection);
    VS_SETSCREENPOS(vsOut.position);
    vsOut.uv0 = vsIn.uv0;
    vsOut.normal = vsIn.normal;// + normalize(windyPoint);
    
    float relDistToCenter = dot(windyPoint.xz, windyPoint.xz) / dot(BoxMaxPos.xz*0.07f, BoxMaxPos.xz*0.07f);
    float relHeight = dot(windyPoint.y, windyPoint.y) / dot(BoxMaxPos.y*0.1f, BoxMaxPos.y*0.1f);// dot(BoxMaxPos, BoxMaxPos);
    vsOut.selfShadow = lerp(INNERLIGHTINTENSITY, OUTERLIGHTINTENSITY,  relHeight);// );      ;   relDistToCenter +
    vsOut.selfShadow += lerp(INNERLIGHTINTENSITY, OUTERLIGHTINTENSITY,  relDistToCenter * relDistToCenter);

    if (LightType == 0)
    {
        // point light
        vsOut.lightVec = ModelLightPos - windyPoint;
    }
    else
    {
        // directional light
        vsOut.lightVec = ModelLightPos;
    }   
    
    return vsOut;
}


//------------------------------------------------------------------------------
/**
    Vertex shader: generate color values for static geometry.
*/
vsOutputFrondColor vsGrassColor(const vsInputBranchColor vsIn)
{
    vsOutputFrondColor vsOut;
   	//float posLen = max(length(vsIn.position),0);
    float4 windyPoint = lerp(vsIn.position,										// interpolate between original position
						mul(vsIn.position, WindMatrices[vsIn.windAttribs.x]),	//	and full wind-blown position
						vsIn.windAttribs.y);	
	//windyPoint.xyz = normalize(windyPoint.xyz).xyz * posLen;									//	using the wind weight			
    vsOut.position = mul(windyPoint, ModelViewProjection);
    VS_SETSCREENPOS(vsOut.position);
    vsOut.uv0 = vsIn.uv0;
    vsOut.normal = vsIn.normal;// + normalize(windyPoint);
    
    float relHeight = dot(windyPoint.y, windyPoint.y) / dot(BoxMaxPos.y*0.175f, BoxMaxPos.y*0.175f);// dot(BoxMaxPos, BoxMaxPos);
    vsOut.selfShadow = 1.0f;//lerp(INNERLIGHTINTENSITY, OUTERLIGHTINTENSITY, relHeight + 0.75f);
        
    vsOut.eyeVec = normalize(ModelEyePos - windyPoint);
    if (LightType == 0)
    {
        // point light
        vsOut.lightVec = ModelLightPos - windyPoint;
    }
    else
    {
        // directional light
        vsOut.lightVec = ModelLightPos;
    }   
    
    return vsOut;
}

//------------------------------------------------------------------------------
/**
    Pixel shader: generate color values for static geometry.
*/
color4 psBranchColor(const vsOutputBranchColor psIn, uniform bool hdr, uniform bool shadow) : COLOR
{
#if DEBUG_LIGHTCOMPLEXITY
    return float4(0.5, 0.0f, 0.0f, 1.0f);
#else
    half shadowIntensity = shadow ? psShadow(PS_SCREENPOS) : 1.0;
    half2 uvOffset = half2(0.0f, 0.0f);
    if (BumpScale != 0.0f) 
    {
        uvOffset = ParallaxUv(psIn.uv0, BumpSampler, psIn.eyePos);
    }    
    color4 diffColor = tex2D(DiffSampler, psIn.uv0 + uvOffset);
    float3 tangentSurfaceNormal = (tex2D(BumpSampler, psIn.uv0 + uvOffset).rgb * 2.0f) - 1.0f;    
    
    color4 baseColor = psBranchLight(diffColor, tangentSurfaceNormal, psIn.lightVec, psIn.modelLightVec, psIn.halfVec, shadowIntensity);
    if (hdr)
    {
        return EncodeHDR(baseColor);
    }
    else
    {
        return baseColor;
    }
#endif
};

//------------------------------------------------------------------------------
/**
    Pixel Sahder for Frond Geometry, optimized for "fichten"
*/
color4 psFrondColor(const vsOutputFrondColor psIn, uniform bool hdr, uniform bool shadow) : COLOR
{
#if DEBUG_LIGHTCOMPLEXITY
    return float4(0.5, 0.0f, 0.0f, 1.0f);
#else
    half shadowIntensity = shadow ? psShadow(PS_SCREENPOS) : 1.0;
    color4 diffColor = tex2D(DiffSampler, psIn.uv0);
    
    color4 baseColor = psFrondLight(diffColor, psIn.normal, psIn.lightVec, psIn.eyeVec, psIn.selfShadow, 1.0f);//shadowIntensity);//float4(normalize(psIn.normal), 1.0f);//
    if (hdr)
    {
        return EncodeHDR(baseColor);
    }
    else
    {
        return baseColor;
    }
#endif
};

//------------------------------------------------------------------------------
/**
    Pixel Shader for Grass Patches
*/
color4 psGrassColor(const vsOutputFrondColor psIn, uniform bool hdr, uniform bool shadow) : COLOR
{
#if DEBUG_LIGHTCOMPLEXITY
    return float4(0.5, 0.0f, 0.0f, 1.0f);
#else
    half shadowIntensity = shadow ? psShadow(PS_SCREENPOS) : 1.0;
    color4 diffColor = tex2D(DiffSampler, psIn.uv0);
    
    color4 baseColor = psFrondLight(diffColor, psIn.normal, psIn.lightVec, psIn.eyeVec, psIn.selfShadow, 1.0f);//shadowIntensity);//float4(normalize(psIn.normal), 1.0f);//
    if (hdr)
    {
        return EncodeHDR(baseColor);
    }
    else
    {
        return baseColor;
    }
#endif
};

//------------------------------------------------------------------------------
/**  
	Leaf Vertex Shader
*/

vsOutputLeavesColor vsLeafColor(const vsInputLeavesColor vsIn)
{
	vsOutputLeavesColor vsOut;
	   	  	
   	//float posLen = length(vsIn.position);
    float4 windyPoint = lerp(vsIn.position,										// interpolate between original position
						mul(vsIn.position, WindMatrices[vsIn.windAttribs.x]),	//	and full wind-blown position
						vsIn.windAttribs.y);	
	//windyPoint.xyz = normalize(windyPoint.xyz).xyz * posLen;									//	using the wind weight

	windyPoint.xyz = mul(windyPoint.xyz, Swing);

	windyPoint.xyz += mul(LeafCluster[vsIn.leafAttribs.x] * vsIn.leafAttribs.y,
 						LeafAngleMatrices[vsIn.leafAttribs.z]);						// leaf placement using SpeedWind leaf rocking
	float3 originPos = windyPoint;	
							
	vsOut.position = mul(windyPoint, ModelViewProjection);  // project to screen
	
	vsOut.uv0 = vsIn.uv0;		// pass texcoords through
	float veclen = length(originPos);		
	//originPos.y -= veclen * 0.5f;
	originPos = originPos/veclen;
	
	float3 movedNormal = originPos;
	vsOut.color.rgb = movedNormal.rgb;
 	vsOut.color.a = 1.0f;															// we only want the texture alpha
 	
 	if (LightType == 0)
    {
        // point light
        vsOut.lightVec = ModelLightPos - windyPoint;
    }
    else
    {
        // directional light
        vsOut.lightVec = ModelLightPos;
    } 
	
	return vsOut;
}

//------------------------------------------------------------------------------
/**  
	Leaf Pixel Shader
*/

float4 psLeafColor(vsOutputLeavesColor psIn, uniform bool hdr) : COLOR
{
#if DEBUG_LIGHTCOMPLEXITY
   	return float4(0.5, 0.0f, 0.0f, 1.0f);
#else
    float4 texLeaf = tex2D(DiffSampler, psIn.uv0);
    
    float diffIntensity = dot(psIn.color, normalize(psIn.lightVec));    

    float4 baseColor = 0;
    if (LightType == 0)
    {
        // point light source
        diffIntensity *= (1.0f - saturate(length(psIn.lightVec) / LightRange));
    }
    else
    {
        #if DIRLIGHTS_ENABLEOPPOSITECOLOR
        baseColor.rgb += saturate(1-diffIntensity) * DIRLIGHTS_OPPOSITECOLOR * texLeaf.rgb * MatDiffuse.rgb;
        #endif    
    }

    baseColor.rgb += texLeaf.rgb * MatDiffuse.rgb * diffIntensity * LightDiffuse.rgb * Intensity0;					// compute the color due to lighting
    baseColor.a = texLeaf.a;
	
	if (hdr)
    {
        return EncodeHDR(baseColor);
    }
    else
    {
        return baseColor;
    }
#endif
}


//------------------------------------------------------------------------------
/**  
	Billboard Vertex Shader
*/

vsOutputBillboardColor vsBillboardColor(const vsInputBillboardColor vsIn)
{
	vsOutputBillboardColor vsOut;

	vsOut.position = mul(vsIn.position, ModelViewProjection);							// project to screen
	
	// pass texcoords through
	vsOut.uv0 = vsIn.uv0;	
	vsOut.pos = vsIn.position;
	vsOut.pos.y = 0;
	vsOut.eyeVec = normalize(vsIn.position - ModelEyePos);
	vsOut.lightVec = vsIn.position - ModelLightPos;
	
	return vsOut;
}

//------------------------------------------------------------------------------
/**
    Pixel shader: generate depth values for static geometry with
    alpha test.
*/
color4 psBillboardDepthATest(const vsOutputUvDepth psIn) : COLOR
{
    float alpha = tex2D(BillboardSampler, psIn.uv0depth.xy).a;
    clip(alpha - (AlphaRef / 256.0f));
    return float4(psIn.uv0depth.z, 0.0f, 0.0f, alpha);
}

//------------------------------------------------------------------------------
/**  
	Billboard Pixel Shader
*/

float4 psBillboardColor(vsOutputBillboardColor psIn, uniform bool hdr) : COLOR
{
#if DEBUG_LIGHTCOMPLEXITY
   	return float4(0.5, 0.0f, 0.0f, 1.0f);
#else
// 	return float4(0.0, 0.8f, 0.0f, 1.0f);
    float4 tex = tex2D(BillboardSampler, psIn.uv0);
    
    // compute cylindrical normal
    //float2 midNormal = (psIn.normal.xz + psIn.pos.xz);
    float lightIntensity = 1 - dot(normalize(psIn.eyeVec + psIn.pos), normalize(psIn.lightVec));//lerp(0.35f, 1.0f, dot(midNormal, normalize(psIn.lightVec)));
    
    if (LightType == 0)
    {
        // point light source
        lightIntensity *= (1.0f - saturate(length(psIn.lightVec) / LightRange));
    }
    float4 baseColor = saturate(lightIntensity) * tex * LightDiffuse * Intensity0;		// diffuse lighting
    baseColor.a = tex.a;
    
    if (hdr)
    {
        return EncodeHDR(baseColor);
    }
    else
    {
        return baseColor;
     }	
#endif
}

//------------------------------------------------------------------------------
/**  
	Billboard Cluster Vertex Shader
*/

vsOutputBillboardColor vsClusterColor(const vsInputClusterColor vsIn)
{
	vsOutputBillboardColor vsOut;

	vsOut.position = mul(vsIn.position, ModelViewProjection);							// project to screen
	
	// pass texcoords through
	vsOut.uv0 = vsIn.uv0;	
	
// in normal billboard shader:	
// 	vsOut.pos = vsIn.position;
// 	vsOut.pos.y = 0;
// 	vsOut.eyeVec = normalize(vsIn.position - ModelEyePos);
// 	vsOut.lightVec = vsIn.position - ModelLightPos;
	
	vsOut.pos = vsIn.modelPos;
	vsOut.pos.y = 0;
	vsOut.eyeVec = normalize(vsIn.modelPos - ModelEyePos);
	vsOut.lightVec = vsIn.modelPos - ModelLightPos;
	
	return vsOut;
}

//------------------------------------------------------------------------------
/**  
	Billboard Cluster Pixel Shader
*/

float4 psClusterColor(vsOutputBillboardColor psIn, uniform bool hdr) : COLOR
{
#if DEBUG_LIGHTCOMPLEXITY
   	return float4(0.5, 0.0f, 0.0f, 1.0f);
#else
	//return float4(0.0, 0.8f, 0.8f, 1.0f);
    float4 tex = tex2D(BillboardSampler, psIn.uv0);
    
    //return float4(psIn.uv0,0,1.0);//tex;
      
    // compute cylindrical normal
    float3 midNormal = normalize(-EyeDir + psIn.pos);
    float lightIntensity = dot(midNormal, normalize(psIn.lightVec));//lerp(0.35f, 1.0f, dot(midNormal, normalize(psIn.lightVec)));
    
    if (LightType == 0)
    {
        // point light source
        lightIntensity *= (1.0f - saturate(length(psIn.lightVec) / LightRange));
    }
    float4 baseColor = tex * lightIntensity * LightDiffuse * Intensity0;		// diffuse lighting
    baseColor.a = tex.a;
    
	if (hdr)
    {
        return EncodeHDR(baseColor);
    }
    else
    {
        return baseColor;
    }	
#endif
}

//------------------------------------------------------------------------------
/**
    Techniques for shader "branch"
*/
technique tBranchDepth
{
    pass p0
    {
        CullMode     = <CullMode>;        
        VertexShader = compile VS_PROFILE vsWindUvDepth();
        PixelShader  = compile PS_PROFILE psBranchDepthATest();
    }
}

technique tBranchColor
{
    pass p0
    {
        CullMode = <CullMode>;
        AlphaBlendEnable = <AlphaBlendEnable>;
        VertexShader = compile VS_PROFILE vsBranchColor();
        PixelShader  = compile PS_PROFILE psBranchColor(false, false);
    }
}

technique tBranchColorHDR
{
    pass p0
    {
        CullMode = <CullMode>;
        AlphaBlendEnable = <AlphaBlendEnable>;
        VertexShader = compile VS_PROFILE vsBranchColor();
        PixelShader  = compile PS_PROFILE psBranchColor(true, false);
    }
}

technique tBranchColorShadow
{
    pass p0
    {
        CullMode = <CullMode>; 
        AlphaBlendEnable = <AlphaBlendEnable>;
        VertexShader = compile VS_PROFILE vsBranchColor();
        PixelShader  = compile PS_PROFILE psBranchColor(false, true);
    }
}

technique tBranchColorHDRShadow
{
    pass p0
    {
        CullMode = <CullMode>; 
        AlphaBlendEnable = <AlphaBlendEnable>;
        VertexShader = compile VS_PROFILE vsBranchColor();
        PixelShader  = compile PS_PROFILE psBranchColor(true, true);
    }
}
// ----------------------------------------------------------------------------
// --------- FRONDS ------------------------------------------------------------
technique tFrondDepth
{
    pass p0
    {
        CullMode     = None;
        AlphaBlendEnable = <AlphaBlendEnable>;
        VertexShader = compile VS_PROFILE vsWindUvDepth();
        PixelShader  = compile PS_PROFILE psBranchDepthATest();
    }
}

technique tFrondColorShadow
{
    pass p0
    {
        CullMode 	 = None; 
        AlphaBlendEnable = <AlphaBlendEnable>;
        VertexShader = compile VS_PROFILE vsFrondColor();
        PixelShader  = compile PS_PROFILE psFrondColor(false, true);
    }
}

technique tFrondColorHDRShadow
{
    pass p0
    {
        CullMode = None; 
        AlphaBlendEnable = <AlphaBlendEnable>;
        VertexShader = compile VS_PROFILE vsFrondColor();
        PixelShader  = compile PS_PROFILE psFrondColor(true, true);
    }
}

technique tGrassColorShadow
{
	pass p0
    {
        CullMode 	 = None; 
        AlphaBlendEnable = <AlphaBlendEnable>;
        VertexShader = compile VS_PROFILE vsGrassColor();
        PixelShader  = compile PS_PROFILE psFrondColor(false, true);
    }
}

technique tGrassColorHDR
{
	pass p0
    {
        CullMode = None; 
        AlphaBlendEnable = <AlphaBlendEnable>;
        VertexShader = compile VS_PROFILE vsGrassColor();
        PixelShader  = compile PS_PROFILE psFrondColor(true, false);
    }
}

technique tGrassColorHDRShadow
{
	pass p0
    {
        CullMode 	 = None; 
        AlphaBlendEnable = <AlphaBlendEnable>;
        VertexShader = compile VS_PROFILE vsGrassColor();
        PixelShader  = compile PS_PROFILE psFrondColor(true, true);
    }
}
// ----------------------------------------------------------------------------
// --------- LEAVES -----------------------------------------------------------
technique tLeavesDepth
{
    pass p0
    {
        CullMode = <CullMode>;
        AlphaBlendEnable = <AlphaBlendEnable>;
        VertexShader = compile VS_PROFILE vsLeafUvDepth();
        PixelShader  = compile PS_PROFILE psBranchDepthATest();
    }
}

technique tLeavesColor
{
    pass p0
    {
        CullMode 	 = <CullMode>;
        AlphaBlendEnable = <AlphaBlendEnable>;
        VertexShader = compile VS_PROFILE vsLeafColor();
        PixelShader  = compile PS_PROFILE psLeafColor(false);
    }
}

        
technique tLeavesColorHDR
{
    pass p0
    {
        CullMode = <CullMode>;
        AlphaBlendEnable = <AlphaBlendEnable>;
        VertexShader = compile VS_PROFILE vsLeafColor();
        PixelShader  = compile PS_PROFILE psLeafColor(true);
    }
}

// ----------------------------------------------------------------------------
// --------- BILLBOARD -----------------------------------------------------------
technique tBillboardDepth
{
    pass p0
    {
        CullMode = <CullMode>;      
        AlphaBlendEnable = <AlphaBlendEnable>;
        VertexShader = compile VS_PROFILE vsStaticUvDepth();
        PixelShader  = compile PS_PROFILE psBillboardDepthATest();
    }
}

technique tBillboardColor
{
    pass p0
    {
        CullMode	 = <CullMode>;   
        AlphaBlendEnable = <AlphaBlendEnable>;
        VertexShader = compile VS_PROFILE vsBillboardColor();
        PixelShader  = compile PS_PROFILE psBillboardColor(false);
    }
}
        
technique tBillboardColorHDR
{
    pass p0
    {
        CullMode = <CullMode>;   
        AlphaBlendEnable = <AlphaBlendEnable>;
        VertexShader = compile VS_PROFILE vsBillboardColor();
        PixelShader  = compile PS_PROFILE psBillboardColor(true);
    }
}

//------------------------------------------------------------------------------
/**
    Techniques for shader "treecluster".
*/
technique tClusterColor
{
    pass p0
    {
        CullMode     = <CullMode>;        
        AlphaBlendEnable = <AlphaBlendEnable>;
        VertexShader = compile VS_PROFILE vsClusterColor();
        PixelShader  = compile PS_PROFILE psBillboardColor(false);
    }
}

technique tClusterColorHDR
{
    pass p0
    {
        CullMode     = <CullMode>;      
        AlphaBlendEnable = <AlphaBlendEnable>;  
        VertexShader = compile VS_PROFILE vsClusterColor();
        PixelShader  = compile PS_PROFILE psBillboardColor(true);
    }
}
