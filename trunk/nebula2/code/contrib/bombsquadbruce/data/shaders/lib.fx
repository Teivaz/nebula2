#line 1 "lib.fx"
//------------------------------------------------------------------------------
//  lib.fx
//
//  Support functions for Crazy Chipmunk shader library.
//
//  (C) 2004 Rafael Van Daele-Hunt
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//  shader input/output declarations
//------------------------------------------------------------------------------

struct StdVsInput
{
    float4 position : POSITION;
    float3 normal : NORMAL;
    float2 uv0 :      TEXCOORD0;
};

struct StdVsOutput
{
    float4 position : POSITION;         // position in projection space
    float2 uv0      : TEXCOORD0;        // tex coord
    float4 diffuse : COLOR0;
    float fog : FOG;
};

//------------------------------------------------------------------------------
/**
	transformFishEye()
	
	Transform position into modelview-projection space without deformations.
	
	@param	pos		a position in model space
	@param  centre  The worldspace coordinates relative to which the fisheye occurs
	@return         transformed position
*/

float4 transformFishEye( float4 pos, float4 centre, const float4x4 Model, const float4x4 View, const float4x4 Projection )
{
    // fish eye transform
    float4 worldPos = mul( pos, Model );
    float2 effectivePos = { worldPos.x - centre.x, worldPos.z - centre.z };
    float dist = max( 1.0, pow( effectivePos.x * effectivePos.x + effectivePos.y * effectivePos.y, 0.25 ) / 15 );
    worldPos.x = effectivePos.x / dist + centre.x;
    worldPos.z = effectivePos.y / dist + centre.z; // the "y" is correct
    pos.y = pos.y / dist; // must do y scaling in model space
    float4 worldPos2 = mul( pos, Model ); // could be optimized, since we only want the new y value
    worldPos.y = worldPos2.y;
    
    return mul( mul( worldPos, View ), Projection );
}
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
    lightDiffuse()
	
    Calcuates diffuse colour after lighting
	
    @param    normal  a normal in model space
    @return           diffuse colour
*/

float4 lightDiffuse( const float3 normal, const float4x4 Model, const float4 LightAmbient, const float4 LightDiffuse, const float3 LightDirection )
{
    float3 worldNormal = mul( normal, Model );
    return saturate( LightAmbient + LightDiffuse * saturate( dot( -worldNormal, LightDirection ) ) );
}

//------------------------------------------------------------------------------
/**
    fog()
	
    Calcuates fog and alpha fog.
	
    @param    alpha           the alpha of the vertex's diffuse colour
    @param    screenPos       the vertex's position in screenspace
    @param    transparentDist the square of the distance from the camera at which the vertex should become completely transparent
    @return           diffuse colour
*/

float fog( inout float alpha, const float4 screenPos, const float transparentDist )
{
    float posSquared = screenPos.z * screenPos.z / transparentDist;
    alpha -= posSquared;
    return 1.0 - posSquared;
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