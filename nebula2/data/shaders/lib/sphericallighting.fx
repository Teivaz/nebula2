//------------------------------------------------------------------------------
//  sphericallighting.fx
//
//  Declarations and utility functions for spherical lighting.
//  Spherical lighting defines 6 directional light colors (up, down, north,
//  south, east, west), and one primary light direction.
//
//  The primary light defines the specular highlight and is used for
//  the per-pixel bump effect. The color of the primary light is
//  defined by the directional colors in that direction.
//
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
float4 MatDiffuse;              // the material's diffuse color
float4 MatSpecular;             // the material's specular color
float  MatSpecularPower;        // the material's specular power

float4 DirAmbient[6] =          // directional ambient colors
{
    { 1.0f, 1.0f, 1.0f, 1.0f },
    { 0.5f, 0.5f, 0.5f, 1.0f },
    { 0.7f, 0.7f, 0.7f, 1.0f },
    { 0.7f, 0.7f, 0.7f, 1.0f },
    { 0.7f, 0.7f, 0.7f, 1.0f },
    { 0.7f, 0.7f, 0.7f, 1.0f},
};

const float3 AmbientDirections[6] =
{
    { 0.0f, 1.0f, 0.0f },       // up
    { 0.0f, -1.0f, 0.0f },      // down
    { 0.0f, 0.0f, -1.0f },      // north
    { 0.0f, 0.0f, 1.0f },       // south
    { 1.0f, 0.0f, 0.0f },       // east
    { -1.0f, 0.0f, 0.0f },      // west
};

//------------------------------------------------------------------------------
//  vsSphericalLighting
//
//  Computes all vertex shader data for spherical lighting:
//
//  - tangent space light vector
//  - tangent space half vector
//  - vertex position in light space
//  - diffuse and specular colors
//
//  NOTE: to simulate light coming from all sides, but still
//  get satisfying bump mapping, we let the diffuse lighting
//  always come from the viewer direction.
//
//  @param  pos         [in] position in model space
//  @param  normal      [in] normal in model space
//  @param  tangent     [in] tangent in model space
//  @param  lightVec    [out] light vector in tangent space
//  @param  halfVec     [out] half vector in tangent space
//  @param  lightPos    [out] position in light space
//  @param  diffuse     [out] resulting diffuse color
//  @param  specular    [out] resulting specular color
//------------------------------------------------------------------------------
void
vsSphericalLighting(in const float4 pos,
                    in const float3 normal,
                    in const float3 tangent,
                    out float3 lightVec,
                    out float3 halfVec,
                    out float4 lightPos,
                    out float4 diffuse,
                    out float4 specular)
{
    // compute light and half vector
    tangentSpaceEyeHalfVector(pos, ModelLightPos, ModelEyePos, normal, tangent, lightVec, halfVec);

    // compute vertex position in light space
    lightPos = mul(pos, ModelLightProjection);

    // compute the spherical diffuse term
    float4 color = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float3 worldNormal = mul(normal, (float3x3)Model);
    int i;
    for (i = 0; i < 6; i++)
    {
        color += DirAmbient[i] * saturate(dot(worldNormal, AmbientDirections[i]));
    }
    diffuse = color * MatDiffuse;
    specular = color * MatSpecular;
}

//------------------------------------------------------------------------------
//  psSphericalLighting
//
//  Computes all pixel shader data for spherical lighting, and returns
//  the resulting pixel color.
//
//  @param  lightVec        the tangent space light vector
//  @param  halfVec         the tangent space half vector
//  @param  diffLightColor  the per-vertex incoming light color
//  @param  specLightColor  the specular lighting color
//  @param  mapColor        the sampled color from the diffuse texture map
//  @param  bumpColor       the sampled color from the bump map (alpha is specular scale)
//  @return                 the resulting pixel color
//------------------------------------------------------------------------------
float4
psSphericalLighting(float3 lightVec,
                    float3 halfVec,
                    float4 diffLightColor,
                    float4 specLightColor,
                    float4 mapColor,
                    float4 bumpColor)
{
    float3 surfNormal = 2.0f * (bumpColor.rgb - 0.5f);
    float specScale = bumpColor.a;

    // compute diffuse and specular intensity
    float diffIntensity = saturate(dot(surfNormal, normalize(lightVec)));
    float specIntensity = specScale * pow(saturate(dot(surfNormal, normalize(halfVec))), MatSpecularPower);

    // compute final color
    float4 outColor;
    outColor.rgb = (diffIntensity * diffLightColor * mapColor) + (specIntensity * specLightColor);
    outColor.a = mapColor.a;
    return outColor;
}

//------------------------------------------------------------------------------
//  psSphericalLightingLeaf
//
//  Like psSphericalLight, but specialized for leaf rendering
//
//  @param  lightVec    the tangent space light vector
//  @param  lightColor  the per-vertex incoming light color
//  @param  mapColor    the sampled color from the diffuse texture map
//  @param  bumpColor   the sampled color from the bump map (alpha is specular scale)
//  @return             the resulting pixel color
//------------------------------------------------------------------------------
float4
psSphericalLightingLeaf(float3 lightVec,
                        float4 lightColor,
                        float4 mapColor,
                        float4 bumpColor)
{
    float3 surfNormal = 2.0f * (bumpColor.rgb - 0.5f);

    // compute diffuse and specular intensity
//    float diffIntensity = abs(dot(surfNormal, normalize(lightVec)));
    float diffIntensity = saturate(dot(surfNormal, float3(0.0f, 0.0f, 1.0f)));

    // compute final color
    float4 outColor;
    outColor.rgb = (diffIntensity * lightColor * mapColor);
    outColor.a = mapColor.a;
    return outColor;
}
