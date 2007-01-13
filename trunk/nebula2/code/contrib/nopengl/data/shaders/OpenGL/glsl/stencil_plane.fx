<?xml version="1.0" encoding="utf-8" ?>

<shader>
<!--
  stencil_plane.fx

  Used for debug visualization.

  Cloned from RadonLabs stencil_plane.fx

  (C) 2006 Oleg Khryptul (Haron)
-->
    <technique name="tColor">
        <pass name="p0">
            <param name="WorldTransform"        index="0" variable="Model" />
            <param name="ViewTransform"         variable="View" />
            <param name="ProjectionTransform"   variable="Projection" />

            <param name="ZWriteEnable"          value="False" />
            <param name="ZEnable"               value="False" />
            <param name="AlphaTestEnable"       value="False" />

            <param name="CullMode"              value="None" />

            <param name="VertexShader"          value="0" />
            <param name="FragmentShader"        value="0" />

            <param name="ColorWriteEnable"      value="RED|GREEN|BLUE|ALPHA" />

            <param name="AlphaBlendEnable"      value="True" />
            <param name="SrcBlend"              value="SrcAlpha" />
            <param name="DestBlend"             value="InvSrcAlpha" />

            <param name="StencilEnable"         value="False" />
            <param name="StencilRef"            value="0" />
            <param name="StencilFunc"           value="NotEqual" />
            <param name="StencilZFail"          value="KEEP" />
            <param name="StencilPass"           value="KEEP" />

            <param name="FVF"                   value="XYZ" />

            <param name="Lighting"              value="True" />

            <param name="LightEnable"           index="0" value="False" />
            <param name="SpecularEnable"        value="False" />
            <param name="MaterialDiffuse"       variable="MatDiffuse" />
            <param name="MaterialAmbient"       value="1.0,1.0,1.0,1.0" />
            <param name="MaterialSpecular"      value="0.0,0.0,0.0,0.0" />
            <param name="Ambient"               variable="MatDiffuse" />

            <param name="ColorOp"               index="0" value="SelectArg1" />
            <param name="ColorArg1"             index="0" value="Diffuse" />

            <param name="ColorOp"               index="1" value="Disable" />

            <param name="AlphaOp"               index="0" value="SelectArg1" />
            <param name="AlphaArg1"             index="0" value="Diffuse" />

            <param name="AlphaOp"               index="1" value="Disable" />
        </pass>
    </technique>
</shader>
