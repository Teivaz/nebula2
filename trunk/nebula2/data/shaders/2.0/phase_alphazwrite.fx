//------------------------------------------------------------------------------
//  phase_alphazwrite.fx
//
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
technique t0
{
    pass p0
    {
        ZWriteEnable     = true;
        AlphaBlendEnable = true;
        AlphaTestEnable  = false;
    }
}

