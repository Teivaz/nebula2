#line 1 "pass_alpha.fx"
//------------------------------------------------------------------------------
//  phase_alpha.fx
//
//  Set render states which are constant for the entire transparency phase in
//  the color pass.
//
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
float4 MatDiffuse;
float4x4 TextureTransform0 = {1.0f, 0.0f, 0.0f, 0.0f,
                              0.0f, 1.0f, 0.0f, 0.0f, 
                              0.0f, 0.0f, 1.0f, 0.0f,
                              0.0f, 0.0f, 0.0f, 1.0f };
                              
technique t0
{
    pass p0
    {
    	TextureTransform[0] = <TextureTransform0>;
        MaterialDiffuse = <MatDiffuse>;
            	    	    
        ZWriteEnable     = False;
        AlphaBlendEnable = True;
        SrcBlend         = SrcAlpha;
        DestBlend        = InvSrcAlpha;
        AlphaTestEnable  = False;
        Lighting         = True;
        VertexShader     = 0;
    }
}

