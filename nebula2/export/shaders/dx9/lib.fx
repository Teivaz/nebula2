//------------------------------------------------------------------------------
//  lib.fx
//
//  Support functions for RadonLabs shader library.
//
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------

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
float3
skinnedPosition(const float3 inPos, const float4 weights, const float4 indices, const float4 jointPalette[24 * 3])
{
    float3 pos[4];
    int i;
    for (i = 0; i < 4; i++)
    {
        pos[i].x = dot(float4(inPos, 1.0f), jointPalette[indices[i] + 0]);
        pos[i].y = dot(float4(inPos, 1.0f), jointPalette[indices[i] + 1]);
        pos[i].z = dot(float4(inPos, 1.0f), jointPalette[indices[i] + 2]);
    }
    return float3((pos[0] * weights.x) + 
                  (pos[1] * weights.y) + 
                  (pos[2] * weights.z) + 
                  (pos[3] * weights.w));
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
skinnedNormal(const float3 inNormal, const float4 weights, const float4 indices, const float4 jointPalette[24 * 3])
{
    float3 normal[4];
    int i;
    for (i = 0; i < 4; i++)
    {
        normal[i].x = dot(inNormal, jointPalette[indices[i] + 0].xyz);
        normal[i].y = dot(inNormal, jointPalette[indices[i] + 1].xyz);
        normal[i].z = dot(inNormal, jointPalette[indices[i] + 2].xyz);
    }
    return float3((normal[0] * weights.x) +
                  (normal[1] * weights.y) +
                  (normal[2] * weights.z) +
                  (normal[3] * weights.w));
}

//------------------------------------------------------------------------------
/**
	tangentSpaceLightVector()
	
	Compute the unnormalized tangent space light vector from a vertex position, light 
	position, normal and tangent (all in model space). This will compute
	an unnormalized light vector, and a binormal behind the scene.
*/
float3
tangentSpaceLightVector(const float3 pos, const float3 lightPos, const float3 normal, const float3 tangent)
{
    // compute the light vector in model space
    float3 lightVec = pos + lightPos;
    
    // compute the binormal
    float3 binormal = cross(normal, tangent);

	// transform model space light vector into tangent space
	float3 outVec;
	outVec.x = dot(lightVec, tangent);
	outVec.y = dot(lightVec, binormal);
	outVec.z = dot(lightVec, normal);
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
    float3 lightVec = normalize(pos - lightPos);
    float3 eyeVec   = normalize(pos - eyePos);
    float3 halfVec  = -(lightVec + eyeVec);

    // compute the binormal
    float3 binormal = cross(normal, tangent);

	// transform model space light vector into tangent space
	float3 outVec;
	outVec.x = dot(halfVec, tangent);
	outVec.y = dot(halfVec, binormal);
	outVec.z = dot(halfVec, normal);
    return outVec;
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


    
