<?xml version="1.0" encoding="utf-8" ?>

<shader>
<!--
  shape.fx

  Used for debug visualization.

  Cloned from RadonLabs shape.fx

  (C) 2006 Oleg Khryptul (Haron)
-->
    <technique name="tColor">
        <pass name="p0">
            <param name="WorldTransform"        index="0" variable="Model" />
            <param name="TextureTransform"      index="0" variable="TextureTransform0" />
            <param name="ViewTransform"         variable="View" />
            <param name="ProjectionTransform"   variable="Projection" />

            <param name="ColorWriteEnable"      value="RED|GREEN|BLUE|ALPHA" />
            <param name="NormalizeNormals"      value="True" />
            <param name="ZEnable"               value="True" />
            <param name="ZFunc"                 value="LessEqual" />
            <param name="StencilEnable"         value="False" />
            <param name="DepthBias"             value="0.0" />
            <param name="FragmentShader"        value="NULL" />
            <param name="SpecularEnable"        value="False" />

            <param name="Ambient"               value="0.8,0.8,0.8,1.0" />
            <param name="LightEnable"           index="0" value="False" />
            <param name="LightAmbient"          index="0" value="1.0,1.0,1.0,1.0" />
            <param name="LightDiffuse"          index="0" value="1.0,1.0,1.0,1.0" />
            <param name="LightSpecular"         index="0" value="1.0,1.0,1.0,1.0" />
            <param name="LightPosition"         index="0" value="0.0,0.0,0.0" />
            <param name="LightRange"            index="0" value="500000.0" />
            <param name="LightAttenuation0"     index="0" value="1.0" />
            <param name="LightAttenuation1"     index="0" value="0.0" />
            <param name="LightAttenuation2"     index="0" value="0.0" />

            <!-- LightType must be the last light state that is set! ???? -->
            <param name="LightType"             index="0" value="POINT" />

            <param name="ZWriteEnable"          value="False" />
            <param name="AlphaBlendEnable"      value="True" />
            <param name="AlphaTestEnable"       value="False" />
            <param name="Lighting"              value="True" />
            <param name="VertexShader"          value="NULL" />
            <param name="FogEnable"             value="False" />

            <param name="CullMode"              value="None" />
            <param name="SrcBlend"              value="SrcAlpha" />
            <param name="DestBlend"             value="InvSrcAlpha" />
            <param name="MaterialAmbient"       variable="MatDiffuse" />
            <param name="MaterialDiffuse"       variable="MatDiffuse" />
            <param name="MaterialSpecular"      value="0.0,0.0,0.0,0.0" />

            <param name="FVF"                   value="XYZ|NORMAL|TEX1" />

            <param name="Texture"               index="0" value="NULL" />
            <param name="TexCoordIndex"         index="0" value="0" />
            <param name="TextureTransformFlags" index="0" value="Count2" />

            <param name="ColorOp"               index="0" value="SelectArg1" />
            <param name="ColorArg1"             index="0" value="Current" />

            <param name="ColorOp"               index="1" value="Disable" />

            <param name="AlphaOp"               index="0" value="SelectArg1" />
            <param name="AlphaArg1"             index="0" value="Diffuse" />

            <param name="AlphaOp"               index="1" value="Disable" />

            <!-- <param name="FillMode"              value="Wireframe" /> -->
        </pass>
    </technique>
</shader>
