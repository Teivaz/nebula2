//------------------------------------------------------------------------------
//  occlusionquery.vert
//
//  Simple shader for occlusion queries. Just renders the passed geometry
//  with zwrite and color writes disabled.
//------------------------------------------------------------------------------

uniform mat4 ModelViewProjection;

void main(void)
{
    gl_Position = ModelViewProjection * gl_Vertex;
}
