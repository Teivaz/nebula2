//------------------------------------------------------------------------------
//  gui.fx
//
//  A 2d rectangle shader for GUI rendering.
//------------------------------------------------------------------------------

//uniform sampler2D DiffMap0;
//vec4 MatDiffuse = vec4(1.0, 1.0, 1.0, 1.0);

void main(void)
{
    gl_FragColor = vec4(0.4,0.4,0.8,1.0);
    //gl_FragColor = texture2D(DiffMap0, gl_TexCoord[0].st) * MatDiffuse;
}
