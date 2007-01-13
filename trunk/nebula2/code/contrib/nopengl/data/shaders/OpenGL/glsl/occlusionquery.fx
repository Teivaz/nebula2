<?xml version="1.0" encoding="utf-8" ?>

<shader>
<!--
  occlusionquery.fx

  Simple shader for occlusion queries. Just renders the passed geometry
  with zwrite and color writes disabled.

  Cloned from RadonLabs occlusionquery.fx

  (C) 2006 Oleg Khryptul (Haron)
-->
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
