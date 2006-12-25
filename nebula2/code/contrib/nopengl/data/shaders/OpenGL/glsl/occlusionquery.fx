<?xml version="1.0" encoding="utf-8" ?>

<shader>
    <source type="vertex"   path="occlusionquery.vert" />
    <source type="fragment" path="occlusionquery.frag" />

    <technique name="t0">
        <pass name="p0">
            <param name="ColorWriteEnable" value="0" />
            <param name="ZWriteEnable"     value="False" />
            <param name="AlphaBlendEnable" value="False" />
            <param name="AlphaTestEnable"  value="False" />
            <param name="FogEnable"        value="False" />
            <param name="ZEnable"          value="True" />
            <param name="ZFunc"            value="LessEqual" />
            <param name="StencilEnable"    value="False" />
            <param name="CullMode"         value="Cw" />
        </pass>
    </technique>
</shader>
 