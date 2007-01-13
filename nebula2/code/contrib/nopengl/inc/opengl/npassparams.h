//------------------------------------------------------------------------------
/**
    @ingroup OpenGL

    OpenGL shader pass parameters declarations

    2007 Haron (Oleg Kreptul)
*/

//-----------------------------------------------------------
// *** Light States
//-----------------------------------------------------------
DECL_PASS_PARAM( LightAmbient,      Float4)
DECL_PASS_PARAM( LightAttenuation0, Float )
DECL_PASS_PARAM( LightAttenuation1, Float )
DECL_PASS_PARAM( LightAttenuation2, Float )
DECL_PASS_PARAM( LightDiffuse,      Float4)
DECL_PASS_PARAM( LightDirection,    Float4)
DECL_PASS_PARAM( LightEnable,       Bool  )
DECL_PASS_PARAM( LightFalloff,      Float )
DECL_PASS_PARAM( LightPhi,          Float )
DECL_PASS_PARAM( LightPosition,     Float4)
DECL_PASS_PARAM( LightRange,        Float )
DECL_PASS_PARAM( LightSpecular,     Float4)
DECL_PASS_PARAM( LightTheta,        Float )
DECL_PASS_PARAM( LightType,         Int   )

//-----------------------------------------------------------
// *** Material States
//-----------------------------------------------------------
DECL_PASS_PARAM( MaterialAmbient,   Float4)
DECL_PASS_PARAM( MaterialDiffuse,   Float4)
DECL_PASS_PARAM( MaterialEmissive,  Float4)
DECL_PASS_PARAM( MaterialPower,     Float )
DECL_PASS_PARAM( MaterialSpecular,  Float4)

//-----------------------------------------------------------
// *** Render States
//-----------------------------------------------------------
DECL_PASS_PARAM( AlphaBlendEnable,    Bool )
DECL_PASS_PARAM( AlphaFunc,           Int  )
DECL_PASS_PARAM( AlphaRef,            Int  )  // should be Float!!!
DECL_PASS_PARAM( AlphaTestEnable,     Bool ) // was Int
DECL_PASS_PARAM( BlendOp,             Int  )
DECL_PASS_PARAM( ColorWriteEnable,    Int  )
DECL_PASS_PARAM( DepthBias,           Float)
DECL_PASS_PARAM( DestBlend,           Int  )
DECL_PASS_PARAM( DitherEnable,        Bool )
DECL_PASS_PARAM( FillMode,            Int  )
DECL_PASS_PARAM( LastPixel,           Int  )
DECL_PASS_PARAM( ShadeMode,           Int  )
DECL_PASS_PARAM( SlopeScaleDepthBias, Float)
DECL_PASS_PARAM( SrcBlend,            Int  )
DECL_PASS_PARAM( StencilEnable,       Bool )
DECL_PASS_PARAM( StencilFail,         Int  )
DECL_PASS_PARAM( StencilFunc,         Int  )
DECL_PASS_PARAM( StencilMask,         Int  )
DECL_PASS_PARAM( StencilPass,         Int  )
DECL_PASS_PARAM( StencilRef,          Int  )
DECL_PASS_PARAM( StencilWriteMask,    Int  )
DECL_PASS_PARAM( StencilZFail,        Int  )
DECL_PASS_PARAM( TextureFactor,       Int  )
DECL_PASS_PARAM( Wrap,                Int  )
DECL_PASS_PARAM( ZEnable,             Bool ) // was Int
DECL_PASS_PARAM( ZFunc,               Int  )
DECL_PASS_PARAM( ZWriteEnable,        Bool )
DECL_PASS_PARAM( ScissorTestEnable,   Bool )     // ???

//-----------------------------------------------------------
// *** Vertex Pipe Render States
//-----------------------------------------------------------
DECL_PASS_PARAM( Ambient,                  Float)
DECL_PASS_PARAM( AmbientMaterialSource,    Int  )
DECL_PASS_PARAM( Clipping,                 Bool )
DECL_PASS_PARAM( ClipPlaneEnable,          Int  )
DECL_PASS_PARAM( ColorVertex,              Bool )
DECL_PASS_PARAM( CullMode,                 Int  )
DECL_PASS_PARAM( DiffuseMaterialSource,    Int  )
DECL_PASS_PARAM( EmissiveMaterialSource,   Int  )
DECL_PASS_PARAM( FogColor,                 Float4) // was Int
DECL_PASS_PARAM( FogDensity,               Float)
DECL_PASS_PARAM( FogEnable,                Bool )
DECL_PASS_PARAM( FogEnd,                   Float)
DECL_PASS_PARAM( FogStart,                 Float)
DECL_PASS_PARAM( FogTableMode,             Int  )
DECL_PASS_PARAM( FogVertexMode,            Int  )
DECL_PASS_PARAM( IndexedVertexBlendEnable, Bool )
DECL_PASS_PARAM( Lighting,                 Bool )
DECL_PASS_PARAM( LocalViewer,              Bool )
DECL_PASS_PARAM( MultiSampleAntialias,     Bool )
DECL_PASS_PARAM( MultiSampleMask,          Int  )
DECL_PASS_PARAM( NormalizeNormals,         Bool )
DECL_PASS_PARAM( PatchSegments,            Float)
DECL_PASS_PARAM( PointScale_A,             Float)
DECL_PASS_PARAM( PointScale_B,             Float)
DECL_PASS_PARAM( PointScale_C,             Float)
DECL_PASS_PARAM( PointScaleEnable,         Bool )
DECL_PASS_PARAM( PointSize,                Float)
DECL_PASS_PARAM( PointSize_Min,            Float)
DECL_PASS_PARAM( PointSize_Max,            Float)
DECL_PASS_PARAM( PointSpriteEnable,        Bool )
DECL_PASS_PARAM( RangeFogEnable,           Bool )
DECL_PASS_PARAM( SpecularEnable,           Bool )
DECL_PASS_PARAM( SpecularMaterialSource,   Int  )
DECL_PASS_PARAM( TweenFactor,              Float)
DECL_PASS_PARAM( VertexBlend,              Int  )

//-----------------------------------------------------------
// *** Sampler States
//-----------------------------------------------------------
DECL_PASS_PARAM( Sampler, InvalidType) //Texture

//-----------------------------------------------------------
// *** Sampler Stage States
//-----------------------------------------------------------
DECL_PASS_PARAM( AddressU,      Int  )
DECL_PASS_PARAM( AddressV,      Int  )
DECL_PASS_PARAM( AddressW,      Int  )
DECL_PASS_PARAM( BorderColor,   Int  )
DECL_PASS_PARAM( MagFilter,     Int  )
DECL_PASS_PARAM( MaxAnisotropy, Int  )
DECL_PASS_PARAM( MaxMipLevel,   Int  )
DECL_PASS_PARAM( MinFilter,     Int  )
DECL_PASS_PARAM( MipFilter,     Int  )
DECL_PASS_PARAM( MipMapLodBias, Float)
DECL_PASS_PARAM( SRGBTexture,   Float)

//-----------------------------------------------------------
// *** Texture States
//-----------------------------------------------------------
DECL_PASS_PARAM( Texture, InvalidType) //Texture

//-----------------------------------------------------------
// *** Texture Stage States
//-----------------------------------------------------------
DECL_PASS_PARAM( AlphaOp,               Int  )
DECL_PASS_PARAM( AlphaArg0,             Int  )
DECL_PASS_PARAM( AlphaArg1,             Int  )
DECL_PASS_PARAM( AlphaArg2,             Int  )
DECL_PASS_PARAM( ColorArg0,             Int  )
DECL_PASS_PARAM( ColorArg1,             Int  )
DECL_PASS_PARAM( ColorArg2,             Int  )
DECL_PASS_PARAM( ColorOp,               Int  )
DECL_PASS_PARAM( BumpEnvLScale,         Float)
DECL_PASS_PARAM( BumpEnvLOffset,        Float)
DECL_PASS_PARAM( BumpEnvMat00,          Float)
DECL_PASS_PARAM( BumpEnvMat01,          Float)
DECL_PASS_PARAM( BumpEnvMat10,          Float)
DECL_PASS_PARAM( BumpEnvMat11,          Float)
DECL_PASS_PARAM( ResultArg,             Int  )
DECL_PASS_PARAM( TexCoordIndex,         Int  )
DECL_PASS_PARAM( TextureTransformFlags, Int  )

//-----------------------------------------------------------
// *** Transform States
//-----------------------------------------------------------
DECL_PASS_PARAM( ProjectionTransform, Matrix44)
DECL_PASS_PARAM( TextureTransform,    Matrix44)
DECL_PASS_PARAM( ViewTransform,       Matrix44)
DECL_PASS_PARAM( WorldTransform,      Matrix44)

//-----------------------------------------------------------
// *** Shader States
//-----------------------------------------------------------
DECL_PASS_PARAM( FragmentShader, InvalidType)                   // was PixelShader
DECL_PASS_PARAM( VertexShader,   InvalidType)

//-----------------------------------------------------------
// *** Shader Constant States
//-----------------------------------------------------------
DECL_PASS_PARAM( PixelShaderConstant,   InvalidType)
DECL_PASS_PARAM( PixelShaderConstant1,  InvalidType)
DECL_PASS_PARAM( PixelShaderConstant2,  InvalidType)
DECL_PASS_PARAM( PixelShaderConstant3,  InvalidType)
DECL_PASS_PARAM( PixelShaderConstant4,  InvalidType)
DECL_PASS_PARAM( PixelShaderConstantB,  InvalidType)
DECL_PASS_PARAM( PixelShaderConstantI,  InvalidType)
DECL_PASS_PARAM( PixelShaderConstantF,  InvalidType)
DECL_PASS_PARAM( VertexShaderConstant,  InvalidType)
DECL_PASS_PARAM( VertexShaderConstant1, InvalidType)
DECL_PASS_PARAM( VertexShaderConstant2, InvalidType)
DECL_PASS_PARAM( VertexShaderConstant3, InvalidType)
DECL_PASS_PARAM( VertexShaderConstant4, InvalidType)
DECL_PASS_PARAM( VertexShaderConstantB, InvalidType)
DECL_PASS_PARAM( VertexShaderConstantI, InvalidType)
DECL_PASS_PARAM( VertexShaderConstantF, InvalidType)

//-----------------------------------------------------------
// *** Unknown
//-----------------------------------------------------------
DECL_PASS_PARAM( FVF, InvalidType)
