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
    new nspotlightnode l
        sel l
        .setposition -50 5 50
        .setscale 1000 1000 1000
        .setvector LightDiffuse 0.5 0.5 0.5 0.5
        .setvector LightSpecular 0.5 0.5 0.5 0.5
        .setvector LightAmbient 1 1 1 0.5
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
    .settexture "DiffMap0" "tg_tex.tga"
    .settexture "BumpMap0" "detailmap.tga"
    .setvector "MatDiffuse" 1.000000 1.000000 1.000000 1.000000
    .setvector "MatSpecular" 0.800000 0.800000 0.800000 1.000000
    .setvector "MatAmbient" 0.300000 0.300000 0.300000 1.000000
    .setshader "colr" "shaders:default.fx"
sel ..

/sys/servers/console.watch *
