<?xml version="1.0" encoding="utf-8" ?>

<shader>
    <source type="vertex"   path="gui.vert" />
    <source type="fragment" path="gui.frag" />

    <technique name="tColor">
        <pass name="p0">
            <param name="ZWriteEnable"     value="False" />
            <param name="ZEnable"          value="False" />
            <param name="ColorWriteEnable" value="RED|GREEN|BLUE|ALPHA" />
            <param name="AlphaBlendEnable" value="True" />
            <param name="SrcBlend"         value="SrcAlpha" />
            <param name="DestBlend"        value="InvSrcAlpha" />
            <param name="AlphaTestEnable"  value="False" />
            <param name="StencilEnable"    value="False" />
            <param name="CullMode"         value="None" />
        </pass>
    </technique>
</shader>