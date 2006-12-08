#line 1 "lib.fx"
//------------------------------------------------------------------------------
//  lib.fx
//
//  Support functions for RadonLabs shader library.
//
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------

#define pi 3.14159265358
#define scatterEccentricity 0.999995 // Henyey-Greenstein variable

const float scatterTurbidiy = 1.0;
const float3 SunColor = {1.53, 0.612, 0.153};
const float3 SunDir = {-0.9657, 0.0193, -0.2588};

//------------------------------------------------------------------------------
/**
	transformStatic()
	
	Transform position into modelview-projection space without deformations.
	
	@param	pos		a position in model space
	@param	mvp		the modelViewProjection matrix
	@return         transformed position
*/
float4
transformStatic(const float3 pos, const float4x4 mvp)
{
	return mul(float4(pos, 1.0), mvp);
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
skinnedPosition(const float4 inPos, const float4 weights, const float4 indices, const float4x3 jointPalette[72])
{
    float3 pos[4];
    
    int i;
    for (i = 0; i < 4; i++)
    {
        pos[i] = (mul(inPos, jointPalette[indices[i]])) * weights[i];
    }
    return float4(pos[0] + pos[1] + pos[2] + pos[3], 1.0f);
}

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
skinnedNormal(const float3 inNormal, const float4 weights, const float4 indices, const float4x3 jointPalette[72])
{
    float3 normal[4];
    int i;
    for (i = 0; i < 4; i++)
    {
        normal[i] = mul(inNormal, (float3x3)jointPalette[indices[i]]) * weights[i];
    }
    return float3(normal[0] + normal[1] + normal[2] + normal[3]);
}

//------------------------------------------------------------------------------
/**
	tangentSpaceVector()
	
	Compute an unnormalized tangent space vector from a vertex position, reference 
	position, normal and tangent (all in model space). This will compute
	an unnormalized light vector, and a binormal behind the scene.
*/
float3
tangentSpaceVector(const float3 pos, const float3 refPos, const float3 normal, const float3 tangent)
{
    // compute the light vector in model space
    float3 vec = refPos - pos;
    
    // compute the binormal
    float3 binormal = cross(normal, tangent);

	// transform with transpose of tangent matrix!
	float3 outVec = mul(float3x3(tangent, binormal, normal), vec);
	return outVec;
}

//------------------------------------------------------------------------------
/**
	tangentSpaceHalfVector()
	
	Compute the unnormalized tangent space half vector from a vertex position, light 
	position, eye position, normal and tangent (all in model space). This
	will compute a normalized lightVec, a normalized eyeVec, an unnormalized
	half vector and a binormal behind the scenes.
*/
float3
tangentSpaceHalfVector(const float3 pos, 
                       const float3 lightPos, 
                       const float3 eyePos, 
                       const float3 normal, 
                       const float3 tangent)
{
    // compute the light vector, eye vector and half vector in model space
    float3 lightVec = normalize(lightPos - pos);
    float3 eyeVec   = normalize(eyePos - pos);
    float3 halfVec  = lightVec + eyeVec;

    // compute the binormal
    float3 binormal = cross(normal, tangent);

	// transform with transpose of tangent matrix!
	float3 outVec = mul(float3x3(tangent, binormal, normal), halfVec);
    return outVec;
}

//------------------------------------------------------------------------------
/**
	tangentSpaceLightHalfVector()
	
	Compute tangent space light and half vectors.
*/
void
tangentSpaceLightHalfVector(in const float3 pos,
                            in const float3 lightPos,
                            in const float3 eyePos,
                            in const float3 normal,
                            in const float3 tangent,
                            out float3 lightVec,
                            out float3 halfVec)
{
    // compute the light vector, eye vector and half vector in model space
    float3 lVec = normalize(lightPos - pos);
    float3 eVec = normalize(eyePos - pos);
    float3 hVec = normalize(lVec + eVec);
                                
    // compute the binormal and tangent matrix
    float3 binormal = cross(normal, tangent);
    float3x3 tangentMatrix = float3x3(tangent, binormal, normal);

	// transform with transpose of tangent matrix!
    lightVec = mul(tangentMatrix, lVec);
    halfVec  = mul(tangentMatrix, hVec);
}

//------------------------------------------------------------------------------
/**
	tangentSpaceEyeHalfVector()
	
	Compute tangent space eye and half vectors.
*/
void
tangentSpaceEyeHalfVector(in const float3 pos,
                          in const float3 lightPos,
                          in const float3 eyePos,
                          in const float3 normal,
                          in const float3 tangent,
                          out float3 eyeVec,
                          out float3 halfVec)
{
    // compute the light vector, eye vector and half vector in model space
    float3 lVec = normalize(lightPos - pos);
    float3 eVec = normalize(eyePos - pos);
    float3 hVec = lVec + eVec;
                                
    // compute the binormal and tangent matrix
    float3 binormal = cross(normal, tangent);
    float3x3 tangentMatrix = float3x3(tangent, binormal, normal);

	// transform with transpose of tangent matrix!
    eyeVec = mul(tangentMatrix, eVec);
    halfVec = mul(tangentMatrix, hVec);
}

//------------------------------------------------------------------------------
/**
	tangentSpaceLightHalfEyeVector()
	
	Compute tangent space light and half vectors.
*/
void
tangentSpaceLightHalfEyeVector(in const float3 pos,
                               in const float3 lightPos,
                               in const float3 eyePos,
                               in const float3 normal,
                               in const float3 tangent,
                               out float3 lightVec,
                               out float3 halfVec,
                               out float3 eyeVec)
{
    // compute the light vector, eye vector and half vector in model space
    float3 lVec = normalize(lightPos - pos);
    float3 eVec = normalize(eyePos - pos);
    float3 hVec = lVec + eVec;
                                
    // compute the binormal and tangent matrix
    float3 binormal = cross(normal, tangent);
    float3x3 tangentMatrix = float3x3(tangent, binormal, normal);

	// transform with transpose of tangent matrix!
    lightVec = mul(tangentMatrix, lVec);
    halfVec  = mul(tangentMatrix, hVec);
    eyeVec   = mul(tangentMatrix, eVec);
}

//------------------------------------------------------------------------------
/**
    reflectionVector()
    
    Returns the eye vector reflected around the surface normal in world space.
*/
float3
reflectionVector(const float3 pos,
                 const float3 eyePos,
                 const float3 normal,
                 const float4x4 model)
{
    float3 eyeVec = eyePos - pos;
    float3 reflVec = reflect(eyeVec, normal);
    float3 worldVec = mul(reflVec, (float3x3)model);
    return 0.5f * (1.0f + normalize(worldVec));
}

//------------------------------------------------------------------------------
/**
    fog()
    
    Compute a distance/layer fog.
    
    @param  pos                     the current vertex position in model space
    @param  worldPos,               the current vertex position in world space
    @param  modelEyePos             the eye position in model space
    @param  fogDistances            fog plane distances, x=near, y=far, z=bottom, w=top
    @param  fogNearBottomColor      the color at the near bottom, rgb=color, a=intensity
    @param  fogNearTopColor         the color at the near top
    @param  fogFarBottomColor       the color at the far bottom
    @param  fogFarTopColor          the color at the far top
*/
float4
fog(const float3 pos,
    const float3 worldPos,
    const float3 modelEyePos,
    const float4 fogDistances,
    const float4 fogNearBottomColor,
    const float4 fogNearTopColor,
    const float4 fogFarBottomColor,
    const float4 fogFarTopColor)
{
    // get normalized vertical and horizontal distance
    float2 dist;
    dist.x = clamp(distance(pos.xz, modelEyePos.xz), fogDistances.x, fogDistances.y);
    dist.y = clamp(worldPos.y, fogDistances.z, fogDistances.w);
    dist.x = (dist.x - fogDistances.x) / (fogDistances.y - fogDistances.x);
    dist.y = (dist.y - fogDistances.z) / (fogDistances.w - fogDistances.z);
    
    // get 2 horizontal interpolated colors
    float4 topColor = lerp(fogNearTopColor, fogFarTopColor, dist.x);
    float4 bottomColor = lerp(fogNearBottomColor, fogFarBottomColor, dist.x);
    
    // get resulting fog color
    float4 fogColor = lerp(bottomColor, topColor, dist.y);
    return fogColor;
}

//------------------------------------------------------------------------------
/**
    shadow()
    
    Compute the shadow modulation color.
    
    @param  shadowPos           position in shadow space
    @param  noiseSampler        sampler with a noise texture
    @param  shadowMapSampler    sampler with shadow map
    @param  shadowModSampler    shadow modulation sampler to fade shadow color in/out
    @return                     a shadow modulation color
*/
float4
shadow(const float4 shadowPos, float distOffset, sampler shadowMapSampler)
{
    // get projected position in shadow space
    float3 projShadowPos = shadowPos.xyz / shadowPos.w;
    
    // jitter shadow map position using noise texture lookup
//    projShadowPos.xy += tex2D(noiseSampler, projShadowPos.xy * 1234.5f).xy * 0.0005f;
    
    // sample shadow depth from shadow map
    float4 shadowDepth = tex2D(shadowMapSampler, projShadowPos.xy) + distOffset;
    
    // in/out test
    float4 shadowModulate;
    if ((projShadowPos.x < 0.0f) || 
        (projShadowPos.x > 1.0f) ||
        (projShadowPos.y < 0.0f) ||
        (projShadowPos.y > 1.0f))
    {
        // outside shadow projection
        shadowModulate = float4(1.0f, 1.0f, 1.0f, 1.0f);
    }
    else if ((shadowDepth.x > projShadowPos.z) || (shadowPos.z > shadowPos.w))
    {
        // not in shadow
        shadowModulate = float4(1.0f, 1.0f, 1.0f, 1.0f);
    }
    else
    {    
        shadowModulate = float4(0.5f, 0.5f, 0.5f, 1.0f);
    
        // in shadow
        //shadowModulate = tex2D(shadowModSampler, projShadowPos.xy);
        //float4 shadowColor = tex2D(shadowModSampler, projShadowPos.xy);
        //float4 blendColor = float4(1.0f, 1.0f, 1.0f, 1.0f);
        //float relDist = saturate((projShadowPos.z - shadowDepth.x) * 20.0f);
        //shadowModulate = lerp(shadowColor, blendColor, relDist);
    }
    return shadowModulate;
}

//------------------------------------------------------------------------------
//  vsLighting
//
//  Vertex shader support function for simpler per-pixel lighting.
//
//  @param  pos                 [in] vertex position
//  @param  normal              [in] vertex normal
//  @param  tangent             [in] vertex tangent
//  @param  primLightVec        [out] primary light vector in tangent space
//  @param  primHalfVec         [out] primary half vector in tangent space
//------------------------------------------------------------------------------
void
vsLighting(in const float4 pos,
           in const float3 normal,
           in const float3 tangent,
           in const float3 modelPrimLightPos,
           in const float3 modelEyePos,
           out float3 primLightVec,
           out float3 primHalfVec)
{           
    // compute light and half vector
    tangentSpaceLightHalfVector(pos, modelPrimLightPos, modelEyePos, normalize(normal), normalize(tangent), primLightVec, primHalfVec);
}

//------------------------------------------------------------------------------
//  psLighting
//
//  Pixel shader functionality for simple per-pixel lighting.
//
//  @param  diffSampler         the diffuse texture sampler
//  @param  bumpSampler         the bump map sampler
//  @param  uv                  uv coordinates for diffuse and bump sampler
//  @param  primLightVec        the primary light vector
//  @param  primHalfVec         the primary light half vector
//  @param  primDiffuse         the primary diffuse color
//  @param  primSpecular        the primary specular color
//  @param  primAmbient         the primary ambient color
//  @param  matSpecularPower    the material specular power
//------------------------------------------------------------------------------
float4
psLighting(in sampler diffSampler,
           in sampler bumpSampler,
           in const float2 uv,
           in const float3 primLightVec,
           in const float3 primHalfVec,
           in const float4 primDiffuse,
           in const float4 primSpecular,
           in const float4 primAmbient,
           in const float  matSpecularPower)
{
    // sample diffuse and bump texture
    float4 mapColor = tex2D(diffSampler, uv);
    float4 bumpColor = tex2D(bumpSampler, uv);

    // compute surface normal in tangent space
    float3 tangentSurfaceNormal = (2.0f * bumpColor.rgb) - 1.0f;
    
    // compute light intensities
    float primDiffIntensity = saturate(dot(tangentSurfaceNormal, normalize(primLightVec)));
    float primSpecIntensity = pow(saturate(dot(tangentSurfaceNormal, normalize(primHalfVec))), matSpecularPower);
    
    // compute light colors
    float4 diffColor = primDiffIntensity * mapColor * primDiffuse;
    float4 specColor = primSpecIntensity * primSpecular * primDiffIntensity;
    float4 envColor  = mapColor * primAmbient;
    float4 color;
    color.rgba = (diffColor + specColor) + envColor;
    color.a = mapColor.a;
    return color;
}

//------------------------------------------------------------------------------
//  psLighting1
//
//  Pixel shader functionality for 2-sided per-pixel lighting.
//
//  @param  diffSampler         the diffuse texture sampler
//  @param  bumpSampler         the bump map sampler
//  @param  uv                  uv coordinates for diffuse and bump sampler
//  @param  primLightVec        the primary light vector
//  @param  primHalfVec         the primary light half vector
//  @param  primDiffuse         the primary diffuse color
//  @param  primSpecular        the primary specular color
//  @param  primAmbient         the primary ambient color
//  @param  secDiffuse          the secondary diffuse color
//  @param  matSpecularPower    the material specular power
//------------------------------------------------------------------------------
float4
psLighting2(in const float4 mapColor,
            in const float3 tangentSurfaceNormal,
            in const float3 primLightVec,
            in const float3 primHalfVec,
            in const float4 primDiffuse,
            in const float4 primSpecular,
            in const float4 primAmbient,
            in const float4 secDiffuse,
            in const float  matSpecularPower,
            in const float  specModulate)
{
    // compute light intensities
    float dotNL = dot(tangentSurfaceNormal, normalize(primLightVec));
    float dotNH = dot(tangentSurfaceNormal, normalize(primHalfVec));
    float primDiffIntensity = saturate(dotNL);
    float secDiffIntensity  = saturate(-dotNL);
    float primSpecIntensity = pow(saturate(dotNH), matSpecularPower);
    
    // compute light colors
    float4 diffColor = (primDiffIntensity * primDiffuse + secDiffIntensity * secDiffuse) * mapColor;
    float4 specColor = primSpecIntensity * primSpecular * primDiffIntensity * specModulate;
    float4 envColor  = mapColor * primAmbient;
    float4 color;
    color.rgba = (diffColor + specColor) + envColor;
    color.a = mapColor.a;
    return color;
}

//------------------------------------------------------------------------------
//  psLightingAlpha
//
//  Pixel shader functionality for simple per-pixel lighting. The specular
//  highlight will also increase the alpha channels value (nice for glass
//  and other transparent surfaces).
//
//  @param  diffSampler         the diffuse texture sampler
//  @param  bumpSampler         the bump map sampler
//  @param  uv                  uv coordinates for diffuse and bump sampler
//  @param  primLightVec        the primary light vector
//  @param  primHalfVec         the primary light half vector
//  @param  primDiffuse         the primary diffuse color
//  @param  primSpecular        the primary specular color
//  @param  primAmbient         the primary ambient color
//  @param  matSpecularPower    the material specular power
//------------------------------------------------------------------------------
float4
psLightingAlpha(in const float4 mapColor,
                in const float3 tangentSurfaceNormal,
                in const float3 primLightVec,
                in const float3 primHalfVec,
                in const float4 primDiffuse,
                in const float4 primSpecular,
                in const float4 primAmbient,
                in const float4 secDiffuse,
                in const float  matSpecularPower,
                in const float  specModulate)
{
    // compute light intensities
    float dotNL = dot(tangentSurfaceNormal, normalize(primLightVec));
    float dotNH = dot(tangentSurfaceNormal, normalize(primHalfVec));
    float primDiffIntensity = saturate(dotNL);
    float secDiffIntensity  = saturate(-dotNL);
    float primSpecIntensity = pow(saturate(dotNH), matSpecularPower);
    
    // compute light colors
    float4 diffColor = (primDiffIntensity * primDiffuse + secDiffIntensity * secDiffuse) * mapColor;
    float4 specColor = primSpecIntensity * primSpecular * primDiffIntensity * specModulate;
    float4 envColor  = mapColor * primAmbient;
    float4 color;
    color.rgba = (diffColor + specColor) + envColor;
    color.a = mapColor.a + primSpecIntensity;
    return color;
}


//------------------------------------------------------------------------------
//  psLightingLeaf
//
//  Pixel shader functionality for simple lighting specialized for leafs.
//
//  @param  diffSampler         the diffuse texture sampler
//  @param  primLightVec        the primary light vector
//  @param  primDiffuse         the primary diffuse color
//------------------------------------------------------------------------------
float4
psLightingLeaf(in sampler diffSampler,
               in const float2 uv,
               in const float4 primDiffuse,
               in const float4 primAmbient)
{              
    // sample diffuse and bump texture
    float4 mapColor = tex2D(diffSampler, uv);

    // compute light colors
    float4 diffColor = mapColor * primDiffuse;
    float4 envColor  = mapColor * primAmbient;
    float4 color;
    color.rgb = diffColor + envColor;
    color.a = mapColor.a;
    return color;
}

//------------------------------------------------------------------------------
//  vsEnvLighting
//
//  Vertex shader support function for environmental lighting.
//
//  @param  pos                 [in] vertex position
//  @param  normal              [in] vertex normal
//  @param  tangent             [in] vertex tangent
//  @param  primLightVec        [out] primary light vector in tangent space
//  @param  primHalfVec         [out] primary half vector in tangent space
//  @param  envNormal           [out] normal for tangent-to-world matrix
//  @param  envTangent          [out] tangent for tangent-to-world matrix
//  @param  envBinormal         [out] binormal for tangent-to-world matrix
//------------------------------------------------------------------------------
void
vsEnvLighting(in const float4 pos,
              in const float3 normal,
              in const float3 tangent,
              in const float3 modelPrimLightPos,
              in const float3 modelEyePos,
              in const float4x4 model,
              in const float4x4 modelLightProjection,
              out float3 primLightVec,
              out float3 primHalfVec,
              out float3 envTangent,
              out float3 envBinormal,
              out float3 envNormal)
{
    // compute light and half vector
    tangentSpaceLightHalfVector(pos.xyz, modelPrimLightPos, modelEyePos, normal, tangent, primLightVec, primHalfVec);
    
    // compute the normal and tangent for the tangent-to-worldspace-matrix.
    envNormal   = mul(normal, (float3x3) model);
    envTangent  = mul(tangent, (float3x3) model);
    envBinormal = cross(envNormal, envTangent);
}

//------------------------------------------------------------------------------
//  psEnvLighting
//
//  Pixel shader functionality for environment lighting.
//
//  @param  diffSampler         the diffuse texture sampler
//  @param  bumpSampler         the bump map sampler
//  @param  envSampler          the environment map sampler
//  @param  uv                  uv coordinates for diffuse and bump sampler
//  @param  primLightVec        the primary light vector
//  @param  primHalfVec         the primary light half vector
//  @param  normal              the world space normal
//  @param  tangent             the world space tangent
//  @param  binormal            the world space binormal
//  @param  primDiffuse         the primary diffuse color
//  @param  primSpecular        the primary specular color
//  @param  primAmbient         the primary ambient color
//  @param  matSpecularPower    the material specular power
//------------------------------------------------------------------------------
float4
psEnvLighting(in sampler diffSampler,
              in sampler bumpSampler,
              in sampler envSampler,
              in const float2 uv,
              in const float3 primLightVec,
              in const float3 primHalfVec,
              in const float3 tangent,
              in const float3 binormal,
              in const float3 normal,
              in const float4 primDiffuse,
              in const float4 primSpecular,
              in const float4 primAmbient,
              in const float  matSpecularPower)
{              
    // sample diffuse and bump texture
    float4 mapColor = tex2D(diffSampler, uv);
    float4 bumpColor = tex2D(bumpSampler, uv);

    // compute surface normal in tangent space
    float3 tangentSurfaceNormal = 2.0f * (bumpColor.rgb - 0.5f);
    
    // compute world space surface normal
    float3 worldSurfaceNormal = mul(tangentSurfaceNormal, float3x3(tangent, binormal, normal));

    // lookup environment color
    float4 envMapColor = texCUBE(envSampler, worldSurfaceNormal);
    
    // compute light intensities
    float primDiffIntensity = saturate(dot(tangentSurfaceNormal, normalize(primLightVec)));
    float primSpecIntensity = pow(saturate(dot(tangentSurfaceNormal, normalize(primHalfVec))), matSpecularPower);
    
    // compute light colors
    float4 diffColor = primDiffIntensity * mapColor * primDiffuse;
    float4 specColor = primSpecIntensity * primSpecular;
    float4 envColor  = mapColor * envMapColor * primAmbient;
    float4 color;
    color.rgb = (diffColor + specColor) + envColor;
    color.a = mapColor.a;
    return color;
}

//------------------------------------------------------------------------------
//  psEnvLightingLeaf
//
//  Pixel shader functionality for environment lighting specialized for leafs.
//
//  @param  diffSampler         the diffuse texture sampler
//  @param  envSampler          the environment map sampler
//  @param  primLightVec        the primary light vector
//  @param  normal              the world space normal
//  @param  tangent             the world space tangent
//  @param  binormal            the world space binormal
//  @param  primDiffuse         the primary diffuse color
//------------------------------------------------------------------------------
float4
psEnvLightingLeaf(in sampler diffSampler,
                  in sampler envSampler,
                  in const float2 uv,
                  in const float3 primLightVec,
                  in const float3 normal,
                  in const float4 primDiffuse,
                  in const float4 primAmbient)
{              
    // sample diffuse and bump texture
    float4 mapColor = tex2D(diffSampler, uv);

    // lookup environment color
    float4 envMapColor = texCUBE(envSampler, normal);
    
    // compute light intensities
    float primDiffIntensity = 1+clamp(dot(normalize(normal), normalize(SunDir)), -1, 0);
        
    // compute light colors
    float4 diffColor = primDiffIntensity * mapColor * primDiffuse;
    float4 envColor  = mapColor * envMapColor;
    float4 color;
    color.rgb = diffColor + envColor * primAmbient;
    color.a = mapColor.a;
    return color;
}

//------------------------------------------------------------------------------
/**
	RelOptLength()
	
	Calculate the relative optical length from the ground to the top of the
	atmosphere.
	
	@param	cosAngle	cosine of the angle to the zenit
	@return             relative optical length
*/
float
RelOptLength(const float cosAngle)
{
// original: 1.0 / (cosAngle + 0.15 * pow(93.885 - acos(cosAngle)/pi*180.0f, -1.253));
// changed for radiant calculation:
    return 1.0 / (cosAngle + 9.4e-4 * pow(1.6386 - acos(cosAngle), -1.253));
}

//------------------------------------------------------------------------------
/**
	RayleighCoeffTotal()
	
	Calculate the total Rayleigh scattering coefficient (scattering caused by
	molecules) based on standard values for air
	
	@return     total coefficient
*/
float3
RayleighCoeffTotal()
{
//const float n =  1.0003;    // refractive index
//const float N =  2.545e25;  // number of molecules per unit volume in air
//const float pn = 0.0035;    // depolarization factor for air

// original: 8.0*pow(pi, 3.0)*pow(pow(n, 2.0) - 1.0, 2.0)*(6.0 + 3.0*pn)/(3.0 * N)/(6.0 - 7.0*pn) / lambda4;
   return float3(6.95e-6, 1.18e-5, 2.44e-5); // precalculated
}

//------------------------------------------------------------------------------
/**
	RayleighCoeffAngular()
	
	Calculate the angular Rayleigh scattering coefficient (scattering caused by
	molecules) based on standard values for air, the total scattering
	coefficient and the angle to the sun.
	
	@param  coeffTotal  the total scattering coefficient
	@param  cosAngle    cosine of the angle to the sun
	@return             angular scattering coefficient
*/
float3
RayleighCoeffAngular(float3 coeffTotal, float cosAngle)
{
    float tempVal;
    // formula sais tempVal = cosAngle^2, but this looks better:
    if (cosAngle < 0.0)
      tempVal = 0.0;
    else
      tempVal = pow(cosAngle, 6.0);
    return 3.0 / (16.0 * pi) * (1.0 + tempVal) * coeffTotal;
}

//------------------------------------------------------------------------------
/**
	MieCoeffTotal()
	
	Calculate the total Mie scattering coefficient (scattering caused by
	aerosols) based on standard values for air and the turbidity.
	
	@return             total scattering coefficient
*/
float3
MieCoeffTotal()
{
    const float3 K = {0.685, 0.679, 0.656};  // different for red, green and blue
    const float3 lambda = {650e-9, 570e-9, 475e-9};  // red, green, blue
    float3 lambda2 = pow(lambda, 2.0);

    float c = lerp(6e-17, 25e-17, scatterTurbidiy);
    return 0.434 * c * pow(pi, 3.0) * 4.0 * K / lambda2;
}

//------------------------------------------------------------------------------
/**
	MieCoeffAngular()
	
	Calculate the angular Mie scattering coefficient (scattering caused by
	aerosols) based on standard values for air, the total scattering
	coefficient and the angle to the sun.
	
	@param  coeffTotal  total scattering coefficient
	@param  cosAngle    cosine of the angle to the sun
	@return             angular scattering coefficient
*/
float3
MieCoeffAngular(float3 coeffTotal, float cosAngle)
{
    return 1/(4*pi) * ((1-pow(scatterEccentricity, 2.0)) / 
        pow(1 - 2*scatterEccentricity*cosAngle + pow(scatterEccentricity, 2.0), 1.5)) * coeffTotal;
}

//------------------------------------------------------------------------------
/**
	OutscatterCoeff()
	
	Calculate the coefficient of light outscattered at a path through the air 
	based on the Rayleigh and Mie scattering coefficients and the length of the
	path.
	
	@param  rayleighCoeffTotal  total Rayleigh scattering coefficient
	@param  mieCoeffTotal       total Mie scattering coefficient
	@param  distance            length of the path
	@return                     outscattering coefficient
*/
float3
OutscatterCoeff(float3 rayleighCoeffTotal, float3 mieCoeffTotal, float distance)
{
    return exp(-(rayleighCoeffTotal + mieCoeffTotal) * distance);
}

//------------------------------------------------------------------------------
/**
	InscatterCoeff()
	
	Calculate the coefficient of light scattered in at a path through the air 
	based on the Rayleigh and Mie scattering coefficients and the length of the
	path.
	
	@param  rayleighCoeffTotal      total Rayleigh scattering coefficient
	@param  rayleighCoeffAngular    angular Rayleigh scattering coefficient
	@param  mieCoeffTotal           total Mie scattering coefficient
	@param  mieCoeffAngular         angular Mie scattering coefficient
	@param  outscatterCoeff         coefficient of outscattering
	@return     inscattering coefficient
*/
float3
InscatterCoeff(float3 rayleighCoeffTotal, float3 rayleighCoeffAngular,
    float3 mieCoeffTotal, float3 mieCoeffAngular, float3 outscatterCoeff)
{
    return (rayleighCoeffAngular + mieCoeffAngular) / 
        (rayleighCoeffTotal + mieCoeffTotal) * (1 - outscatterCoeff);
}

//------------------------------------------------------------------------------
/**
	vsAthmoFog()
	
	Calculate athmospheric fog, usable for all objects.
	
	@param  vertPos         [in] vertex position
	@param  modelEyePos     [in] camera position in model space
	@param  modelSunPos     [in] sun position in model space
	@param  inscattered     [out] inscattered light
	@param  outscattered    [out] coefficient of light outscattering
*/
void
vsAthmoFog(in const float3 vertPos,
           in const float3 modelEyePos,
           in const float3 modelSunPos,
           out float3 inscattered,
           out float3 outscattered)
{
    float3 relVertPos = vertPos - modelEyePos;
    float relVertDist = length(relVertPos);
    float cosViewSunAngle = dot(relVertPos, normalize(modelSunPos)) / relVertDist;
    // prevent the sun from shining through the objects
    cosViewSunAngle = min(cos(radians(2.0)), cosViewSunAngle);

    float3 betaRTotal = 0.5 * RayleighCoeffTotal();
    float3 betaRAngular = RayleighCoeffAngular(betaRTotal, cosViewSunAngle);
    float3 betaMTotal = 1e-3 * MieCoeffTotal();
    float3 betaMAngular = MieCoeffAngular(betaMTotal, cosViewSunAngle);
    
    outscattered = OutscatterCoeff(betaRTotal, betaMTotal, relVertDist*6);
    inscattered = 50 * lerp(SunColor, 0.3, 1-cosViewSunAngle) *
        InscatterCoeff(betaRTotal, betaRAngular, betaMTotal, betaMAngular, outscattered);
}

//------------------------------------------------------------------------------
/**
	psAthmoFog()
	
	Combine in- and outscattering with texture.
	
	@param  inscatter       [in] color of inscattered light
	@param  outscatter      [in] coefficient of outscattered light
	@param  baseColor       [in] actual color of object
	@return     final object color
*/
float4
psAthmoFog(in const float3 inscatter,
           in const float3 outscatter,
           in const float4 baseColor)
{
    return float4(outscatter * baseColor.rgb + inscatter, baseColor.a);
}

//------------------------------------------------------------------------------
/**
	vsSky()
	
	Calculate Sky Color.
	
	@param  vertPos         [in] vertex position
	@param  modelSunPos     [in] sun position in model space
	@param  color           [out] color of the sky
*/
void
vsSky(in const float3 vertPos,
      in const float3 modelSunPos,
      out float4 color)
{
    float vertDist = length(vertPos);
    float cosSunZenitAngle = normalize(modelSunPos).y;
    float cosViewSunAngle = dot(vertPos.xyz, normalize(modelSunPos)) / vertDist;
    float cosViewZenitAngle = saturate(vertPos.y / vertDist);
    float relWay = RelOptLength(cosViewZenitAngle);
    
    float3 betaRTotal = 0.5 * RayleighCoeffTotal();
    float3 betaRAngular = RayleighCoeffAngular(betaRTotal, cosViewSunAngle);
    float3 betaMTotal = 1e-3 * MieCoeffTotal();
    float3 betaMAngular = MieCoeffAngular(betaMTotal, cosViewSunAngle);

    float3 outscattered = OutscatterCoeff(betaRTotal, betaMTotal, 8400.0 * relWay);
    color.rgb = 50 * lerp(SunColor, 0.3, 1-cosViewSunAngle) *
        InscatterCoeff(betaRTotal, betaRAngular, betaMTotal, betaMAngular, outscattered);
    color.a = 0.5+0.2*pow(1-cosViewZenitAngle, 4);
}

//------------------------------------------------------------------------------
/**
    vsExpFog()
    
    Compute exponential fog in vertex shader. Returns fog color in rgb and
    fog density in a.
    
    legend:
    fogParams.x     -> fog layer ground height
    fogParams.y     -> fog horizontal density (EXP)
    fogParams.z     -> fog vertical density (EXP)
*/
float
vsExpFog(in const float3 modelVertexPos, 
         in const float3 worldVertexPos,
         in const float3 modelEyePos,
         in const float4 fogParams)
{
    const float e = 2.71828;
    float eyeDist = distance(modelVertexPos, modelEyePos);
    float vertDist = max(worldVertexPos.y - fogParams.x, 0); 
    float heightModulate = 1.0 / pow(e, vertDist * fogParams.z);
    float fogDensity = 1.0 / pow(e, eyeDist * fogParams.y);
    return heightModulate * (1.0 - fogDensity);
}
