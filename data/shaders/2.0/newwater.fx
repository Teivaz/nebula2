shared float4x4 InvView;
shared float4x4 ModelViewProjection;    // the model * view * projection matrix
shared float4x4 Model;                  // the model matrix
shared float Time;
shared float4 RendertargetOffSet;
int RenderComplexity;



#define REFRACTION 1 // water refraction below surface
#define HDR 0 // hdr encoding

// -------------- TYPEDEFS -------------
typedef half3 color3;
typedef half4 color4;


float BumpScale = 1.0f;
float BumpScale2 = 0.25f;
float4 TexGenS = float4(1.0, 1.0, 1.0, 1.0);
float4 Velocity = float4(0.0, 0.0, 0.0, 0.0); // UVAnimaton2D, Water movement
float FresnelBias;
float FresnelPower;
float4 MatDiffuse;
float4 MatSpecular;
float Intensity0 = 1.0;
int CullMode = 2;
int AlphaSrcBlend = 5;
int AlphaDstBlend = 6;
bool  AlphaBlendEnable = true;

texture BumpMap0;
texture DiffMap1;
texture DiffMap2;

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

sampler Diff2Sampler = sampler_state
{
    Texture = <DiffMap2>;
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



//------------------------------------------------------------------------------
struct vsInputWater
{
    float4 position : POSITION;
    float3 normal   : NORMAL;  
    float2 uv0      : TEXCOORD0;
    float3 tangent  : TANGENT;
    float3 binormal : BINORMAL;  
};

struct vsOutputWater
{
    float4 position     : POSITION;
    float2 uv0          : TEXCOORD0;
    float3 row0         : TEXCOORD1; // first row of the 3x3 transform from tangent to cube space
    float3 row1         : TEXCOORD2; // second row of the 3x3 transform from tangent to cube space
    float3 row2         : TEXCOORD3; // third row of the 3x3 transform from tangent to cube space
    float4 bumpCoord0   : TEXCOORD4;
    float4 pos          : TEXCOORD5;
    float3 eyeVector    : TEXCOORD6;
};

//------------------------------------------------------------------------------
//  vertex shader
//------------------------------------------------------------------------------
vsOutputWater vsWater(const vsInputWater vsIn)
{
    vsOutputWater vsOut;
    
    float modTime = fmod(Time, 10000.0f);    
    float4 pos = vsIn.position;
    pos.y += (sin(modTime ) / 10.0f) * (pos.x / 100.0f);
    //pos.y += (sin(modTime + (pos.x / 100.0f)));
        
    // compute output vertex position   
    vsOut.position = mul(pos, ModelViewProjection);
    
    //position for calculate texture in screenspace (pixelshader) 
    vsOut.pos = vsOut.position;
    
    // pass texture coordinates for fetching the normal map
    vsOut.uv0.xy = vsIn.uv0;
    
    vsOut.bumpCoord0.xy = vsIn.uv0 * TexGenS.xy + modTime * Velocity.xy;
    vsOut.bumpCoord0.zw = vsIn.uv0 * TexGenS.xy + modTime * Velocity.xy * -1.0f;
    
    // compute the 3x3 tranform from tangent space to object space
    // first rows are the tangent and binormal scaled by the bump scale
    float3x3 objToTangentSpace;

    objToTangentSpace[0] = BumpScale * vsIn.tangent;
    objToTangentSpace[1] = BumpScale * cross(vsIn.normal, vsIn.tangent);
    objToTangentSpace[2] = vsIn.normal;
    
    vsOut.row0.xyz = mul(objToTangentSpace, Model[0].xyz);
    vsOut.row1.xyz = mul(objToTangentSpace, Model[1].xyz);
    vsOut.row2.xyz = mul(objToTangentSpace, Model[2].xyz);
    
    // compute the eye vector (going from shaded point to eye) in cube space
    float4 worldPos = mul(pos, Model);
    
    vsOut.eyeVector = InvView[3] - worldPos; // view inv. transpose contains eye position in world space in last row
       
    return vsOut;
}


//------------------------------------------------------------------------------
//  Helper Functions.
//------------------------------------------------------------------------------

color4 EncodeHDR(in color4 rgba)
{
    return rgba * color4(0.95, 0.95, 0.95, 1.0);
}
//------------------------------------------------------------------------------
//  pixel shader ueberwasser
//------------------------------------------------------------------------------
float4 psWater(vsOutputWater psIn, uniform bool hdr, uniform bool refraction) : COLOR
{
    // compute the normals
    // with 2 texture it looks more intressting
    
    float3 N0 = (tex2D(BumpSampler, psIn.bumpCoord0.xy).rgb) * 2.0 - 1.0;
    float3 N1 = (tex2D(BumpSampler, psIn.bumpCoord0.wz).rgb) * 2.0 - 1.0;
    // add both normals
    float3 N = N0.xyz + N1.xyz;
    // bring normals in worldspace
    half3x3 m; 
    m[0] = psIn.row0;
    m[1] = psIn.row1;
    m[2] = psIn.row2;
    float3 Nw = mul(m, N.xyz);
    Nw = normalize(Nw);
    // compute screen pos for the reflection refraction texture because both textures are in screencoordinates
    float4 pos = psIn.pos;
    // prepare pos - pos is in model view projection when pos.x and pos.y get divided with pos.w
    // you get a value between -1 and 1 but you need a number between 0 and 1
    // later pos.x and pos.y will divided with pos.w so here we precalculate
    pos.x += psIn.pos.w;    
    pos.w  = 2.0f * psIn.pos.w;
    
    float2 Screenpos = float2(psIn.pos.xy) / psIn.pos.w;
    Screenpos = (Screenpos + float2(1,1)) * 0.5;
    Screenpos = lerp(RendertargetOffSet.xz, RendertargetOffSet.yw, Screenpos);  
    
    // compute the distortion and prepare pos for texture lookup
    float3 E = normalize(psIn.eyeVector);
    // angle between eyevector and normal important for fresnel and distortion
    float angle = dot(E, Nw);
    //compute fresnel
    float facing = saturate(0.5 - max(angle, 0)); 
    float fresnel = FresnelBias + (1.0 - FresnelBias) * pow(facing , FresnelPower);
    
    // refraction rotate the refraction texture with 180 degree
    pos.y  = (-psIn.pos.y + psIn.pos.w);
    Screenpos.y = -Screenpos.y;

    float refractLookUp;
    float4 refractionComp;

    // compute distortion - the math is not correct but this is more faster information in my document
    // is computed with angle, because when you look from the side the distortion is stronger then you look from over the water
    // is computed with pos.z, because the distortions are in screencoordinates so the distortions have to get smaller with the distance
    //0.4 - Als Artist variable rauslegen
    
    float4 disVecRefract    = float4(Nw.x, Nw.y, Nw.z, 0.0) * (1 - angle) * 0.4 / pos.z * pos.w;
	    
	// read the depth from the water with full distortion - this is pasted in the alpha value   
    refractLookUp   = tex2D(Diff2Sampler, Screenpos + (disVecRefract.xy/pos.w)).a;
    //return float4(refractLookUp,1,1,1);
    // read the refractioncolor with distortion multiplied with the depth of the water so the distortion is at the strand smaller
    refractionComp  = tex2D(Diff2Sampler, Screenpos + (disVecRefract.xy/pos.w) * refractLookUp);
    // apply water color in deep direction 
    refractionComp.rgb = lerp(refractionComp.rgb, MatDiffuse.rgb, saturate(refractionComp.a));

    // apply water color in shallow direction
    // reflection 
    pos.y = psIn.pos.y + psIn.pos.w;
    Screenpos.y = -Screenpos.y;
    
    // same as refraction
    // Johannes: No dissort for the refelection!!
    
    if(RenderComplexity == 0)
    {
	    return color4(1,0,0,1);
    }
    else
    {
	    return color4(refractionComp.rgb,1);
    }
    	float4 reflectionComp = (tex2D(Diff1Sampler, Screenpos + (disVecRefract.xy/pos.w)));
    
    
    // compute all together   
    fresnel = 1.0f; 
    
    float4 refracPart = (1- saturate(refractionComp.a)) * refractionComp;
    //return color4(refracPart.rgb, 1);
    float4 reflecPart = (refractionComp.a * reflectionComp * fresnel);
    
    float4 colorWater = refracPart + reflecPart;
    
    //float4 colorWater = color4((1- saturate(refractionComp.a)) * refractionComp + (refractionComp.a * reflectionComp * fresnel));

    //colorWater = (1- saturate(refractionComp.a)) * refractionComp + (refractionComp.a * reflectionComp * fresnel);
    // reflection is multiplied with deep because so the reflection is on the strand less and you get soft edges
    
	
	if(hdr)
	{
    	return EncodeHDR(colorWater);
	}
	else
	{
		return colorWater;
	}
}
//------------------------------------------------------------------------------
technique tWaterRefraction
{
    pass p0
    {
        CullMode = <CullMode>;      
        AlphaBlendEnable = true;        
        SrcBlend  = SrcAlpha;
        DestBlend = InvSrcAlpha;
        VertexShader = compile vs_2_0 vsWater();
        PixelShader  = compile ps_2_0 psWater(false, true);
    }
}

technique tWaterHDR
{
    pass p0
    {
        CullMode = <CullMode>;      
        AlphaBlendEnable = true;
        SrcBlend  = SrcAlpha;
        DestBlend = InvSrcAlpha;
        VertexShader = compile vs_2_0 vsWater();
        PixelShader  = compile ps_2_0 psWater(true, false);
    }
}

technique tWaterHDRRefraction
{
    pass p0
    {
        CullMode = <CullMode>;      
        AlphaBlendEnable = false;
        VertexShader = compile vs_2_0 vsWater();
        PixelShader  = compile ps_2_0 psWater(true, true);
    }
}
