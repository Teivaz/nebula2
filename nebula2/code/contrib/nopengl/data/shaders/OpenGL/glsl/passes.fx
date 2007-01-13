<?xml version="1.0" encoding="utf-8" ?>

<shader>
<!--
  passes.fx

  Render path passes.

  Cloned from RadonLabs passes.fx

  (C) 2006 Oleg Khryptul (Haron)
-->
    <technique name="tPassDepth">
        <pass name="p0">
            <param name="ViewTransform"       variable="View" />
            <param name="ProjectionTransform" variable="Projection" />
            <param name="ColorWriteEnable"    value="0" />
            <param name="NormalizeNormals"    value="True" />
            <param name="ZEnable"             value="True" />
            <param name="ZWriteEnable"        value="True" />
            <param name="StencilEnable"       value="False" />
            <param name="DepthBias"           value="0.0" />
            <param name="FogEnable"           value="False" />
            <param name="AlphaBlendEnable"    value="False" />
            <param name="AlphaFunc"           value="GreaterEqual" />
            <param name="ScissorTestEnable"   value="False" />
        </pass>
    </technique>

    <technique name="tPassColor">
        <pass name="p0">
            <param name="ViewTransform"       variable="View" />
            <param name="ProjectionTransform" variable="Projection" />
            <param name="ColorWriteEnable"    value="RED|GREEN|BLUE|ALPHA" />
            <param name="NormalizeNormals"    value="True" />
            <param name="ZEnable"             value="True" />
            <param name="ZWriteEnable"        value="False" />
            <param name="StencilEnable"       value="False" />
            <param name="DepthBias"           value="0.0" />
            <param name="FogEnable"           value="False" />
            <param name="AlphaBlendEnable"    value="True" />
            <param name="AlphaTestEnable"     value="True" />
            <param name="AlphaFunc"           value="GreaterEqual" />
            <param name="SrcBlend"            value="One" />
            <param name="DestBlend"           value="One" />
            <param name="ScissorTestEnable"   value="True" />
            <!-- <param name="FillMode"            value="Wireframe" /> -->
        </pass>
    </technique>

    <technique name="tPassEnvironment">
        <pass name="p0">
            <param name="ViewTransform"       variable="View" />
            <param name="ProjectionTransform" variable="Projection" />
            <param name="ColorWriteEnable"    value="RED|GREEN|BLUE|ALPHA" />
            <param name="NormalizeNormals"    value="True" />
            <param name="ZEnable"             value="True" />
            <param name="ZWriteEnable"        value="True" />
            <param name="StencilEnable"       value="False" />
            <param name="DepthBias"           value="0.0" />
            <param name="FogEnable"           value="False" />
            <param name="AlphaBlendEnable"    value="True" />
            <param name="AlphaTestEnable"     value="True" />
            <param name="AlphaFunc"           value="GreaterEqual" />
            <param name="SrcBlend"            value="One" />
            <param name="DestBlend"           value="One" />
            <param name="ScissorTestEnable"   value="True" />
            <!-- <param name="FillMode"            value="Wireframe" /> -->
        </pass>
    </technique>

    <technique name="tPassGui3D">
        <pass name="p0">
            <param name="ViewTransform"       variable="View" />
            <param name="ProjectionTransform" variable="Projection" />
            <param name="ColorWriteEnable"    value="RED|GREEN|BLUE|ALPHA" />
            <param name="NormalizeNormals"    value="True" />
            <param name="ZEnable"             value="True" />
            <param name="ZWriteEnable"        value="True" />
            <param name="StencilEnable"       value="False" />
            <param name="DepthBias"           value="0.0" />
            <param name="FogEnable"           value="False" />
            <param name="AlphaBlendEnable"    value="True" />
            <param name="AlphaTestEnable"     value="False" />
            <param name="AlphaFunc"           value="GreaterEqual" />
            <param name="SrcBlend"            value="SrcAlpha" />
            <param name="DestBlend"           value="InvSrcAlpha" />
            <param name="ScissorTestEnable"   value="False" />
            <!-- <param name="FillMode"            value="Wireframe" /> -->
        </pass>
    </technique>
</shader>
