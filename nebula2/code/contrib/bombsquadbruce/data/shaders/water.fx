#line 1 "water.fx"
//------------------------------------------------------------------------------
//  water.fx
//
//  The water shader for DX8 cards
//  
//  (C) 2004 Rafael Van Daele-Hunt
//------------------------------------------------------------------------------

float4 TexGenS;
shared float4x4 ModelViewProjection;
texture DiffMap0;
float Time;
float Frequency; // how quickly texture "waves" roll over the water
float WaveHeight = 100;
float WaveSpeed = 5;
float4 MatTransparency;

// wave constants for 4 waves: amplitude, frequency
float2 WaveParams[] = {
    { -0.0368069, 0.241924 },
    { 0.00235418, 0.435527},
    { -0.0298042, 0.401714},
    {0.0361108, 0.323711 },
};

float2 WaveDirs[] = {
   0.470025 , 0.170519,
   0.387961 , -0.315415,
   0.499167 , -0.028842,
   0.464146 , -0.185926,
};
//------------------------------------------------------------------------------
//  Texture samplers
//------------------------------------------------------------------------------
sampler WaterSampler = sampler_state
{
    Texture = <DiffMap0>;
    AddressU  = Wrap;
    AddressV  = Wrap;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Point;
};

//------------------------------------------------------------------------------
//  shader input/output declarations
//------------------------------------------------------------------------------

struct VsInput
{
    float4 position : POSITION;
    float2 uv0 :      TEXCOORD0;
};

struct VsOutput
{
    float4 position : POSITION;         // position in projection space
    float2 uv0      : TEXCOORD0;        // tex coord
    float4 diffuse : COLOR;             // only controls the alpha, actually
};
//------------------------------------------------------------------------------
//  The vertex shader.
//------------------------------------------------------------------------------

VsOutput vsMain(const VsInput vsIn)
{
    VsOutput vsOut;

    float4 pos = vsIn.position;
    
    // transform vertex position
    int i;
    for (i = 0; i < 4; i++)
    {
        pos.y += WaveHeight * WaveParams[i][0] *
             (float) sin( WaveSpeed * WaveParams[i][1] * (Time + WaveDirs[i].x * vsIn.position.x + WaveDirs[i].y * vsIn.position.z)); 
    }
    vsOut.position = mul(pos, ModelViewProjection);
    
                              
    // generate time-variable texture coordinates
    // TexGenS is set by the C++ and causes the texture to scroll in a direction
    // equal and opposite to that of the camera movement.  This disguises the
    // fact that the water plane is following the player around.
    vsOut.uv0 = vsIn.uv0 + TexGenS + Frequency * Time;
    vsOut.diffuse = MatTransparency;
    return vsOut;
}

//------------------------------------------------------------------------------
technique t0
{
    pass p0
    {
        ZWriteEnable = True;
        ColorWriteEnable = RED|GREEN|BLUE|ALPHA;       
        ZEnable          = True;
        ZFunc            = LessEqual;

        AlphaBlendEnable = True;
        SrcBlend         = SrcAlpha; // One
        DestBlend        = InvSrcAlpha;
        AlphaTestEnable  = False;

        CullMode = 2; // default value (CW); must not be 0 for software vp

        VertexShader = compile vs_1_1 vsMain();
        PixelShader  = 0;

        Lighting = False;

        FogEnable = false;
 
        Sampler[0] = <WaterSampler>;

        ColorOp[0]   = SelectArg1;
        ColorArg1[0] = Texture;
        
        AlphaOp[0]   = SelectArg1;
        AlphaArg1[0] = Diffuse;
        
        ColorOp[1]   = Disable;
        AlphaOp[1] = Disable;
    }
}

