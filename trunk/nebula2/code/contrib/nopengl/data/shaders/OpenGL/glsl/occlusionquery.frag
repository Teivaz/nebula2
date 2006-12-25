//------------------------------------------------------------------------------
//  occlusionquery.frag
//
//  Simple shader for occlusion queries. Just renders the passed geometry
//  with zwrite and color writes disabled.
//------------------------------------------------------------------------------

void main(void)
{
    gl_FragColor = vec4(0.0, 0.0, 0.0, 0.0);
}
