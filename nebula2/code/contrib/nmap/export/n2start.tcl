sel /usr/scene
.setactive true

new nmap /data/map
    sel /data/map
    .setheightmap "tg_map.bmp"
    .setgridinterval 1
    .setheightrange -5 20
sel ..

#define new light

sel /usr/scene

new ntransformnode light1
    sel light1
    .seteuler 0 -45 45
    new nvolumelightnode l
        sel l
        .setposition -50 5 50
        .setscale 1000 1000 1000
        .setvector lightDiffuse 1 1 1 1
        .setvector lightSpecular 1 1 1 1
        .setvector lightAmbient 1 1 1 1
    sel ..    
sel ..    

sel /usr/scene
new nmapnode map
    sel map
    .setmap "/data/map"
    .setblocksize 33
    .seterror 4
    .setdetailsize 1
    .setposition -150 -20 -100
    .seteuler 0 0 0

    .setactive true
    .settexture "diffMap" "tg_tex.tga"
    .settexture "normMap" "detailmap.tga"
    .setvector "matDiffuse" 1.000000 1.000000 1.000000 1.000000
    .setvector "matSpecular" 0.800000 0.800000 0.800000 1.000000
    .setvector "matAmbient" 0.300000 0.300000 0.300000 1.000000
    .setshader "dept" "shaders:depth.fx"
    .setshader "colr" "shaders:color1.fx"
    .setshader "diff" "shaders:diffuse.fx"
sel ..

/sys/servers/console.watch *
