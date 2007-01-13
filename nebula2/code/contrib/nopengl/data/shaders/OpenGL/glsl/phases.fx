<?xml version="1.0" encoding="utf-8" ?>

<shader>
<!--
  phases.fx

  Contains renderpath phase shaders.

  Cloned from RadonLabs phases.fx

  (C) 2006 Oleg Khryptul (Haron)
-->
    <technique name="tPhaseDepth">
        <pass name="p0">
            <param name="ColorWriteEnable"    value="RED" />
            <param name="AlphaTestEnable"     value="False" />
            <param name="ZFunc"               value="LessEqual" />
        </pass>
    </technique>

    <technique name="tPhaseATestDepth">
        <pass name="p0">
            <param name="ColorWriteEnable"    value="RED" />
            <param name="AlphaTestEnable"     value="False" />
            <param name="ZFunc"               value="LessEqual" />
        </pass>
    </technique>

    <technique name="tPhaseSky">
        <pass name="p0">
            <param name="ZWriteEnable"       value="False" />
        </pass>
    </technique>

    <technique name="tPhaseTerrain">
        <pass name="p0">
            <param name="AlphaTestEnable"     value="False" />
            <param name="ZFunc"               value="LessEqual" />
        </pass>
    </technique>

    <technique name="tPhaseOpaque">
        <pass name="p0">
            <param name="AlphaTestEnable"     value="False" />
            <param name="ZFunc"               value="LessEqual" />
        </pass>
    </technique>

    <technique name="tPhaseOpaqueATest">
        <pass name="p0">
            <param name="AlphaTestEnable"     value="True" />
            <param name="ZFunc"               value="LessEqual" />
        </pass>
    </technique>

    <technique name="tPhaseNoLight">
        <pass name="p0">
            <param name="AlphaBlendEnable"    value="False" />
            <param name="AlphaTestEnable"     value="False" />
            <param name="ZFunc"               value="LessEqual" />
        </pass>
    </technique>

    <technique name="tPhaseAlpha">
        <pass name="p0">
            <param name="AlphaBlendEnable"    value="True" />
            <param name="AlphaTestEnable"     value="False" />
            <param name="ZFunc"               value="LessEqual" />
        </pass>
    </technique>

    <technique name="tPhasePointSprites">
        <pass name="p0">
            <param name="AlphaBlendEnable"    value="True" />
            <param name="PointSpriteEnable"   value="True" />
            <param name="PointScaleEnable"    value="False" />
            <param name="AlphaTestEnable"     value="False" />
            <param name="ZFunc"               value="LessEqual" />
        </pass>
    </technique>

    <technique name="tPhaseGui3D">
        <pass name="p0">
            <param name="ZFunc"               value="LessEqual" />
        </pass>
    </technique>
</shader>
