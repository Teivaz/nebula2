# ---
# $parser:nrubyserver$ $class:ntransformnode$
# ---
include Nebula

addanimator "rot"
setposition 0.000000, 0.000000, 0.000000
seteuler 5.662079, 11.324158, 0.000000
setscale 1.000000, 1.000000, 1.000000
new 'ntransformanimator', 'rot'
    sel 'rot'
    setchannel "time"
    setlooptype "loop"
    addeulerkey 0.000000, 0.000000, 0.000000, 0.000000
    addeulerkey 50.000000, 360.000000, 720.000000, 0.000000
sel '..'
new 'nshapenode', 'shape0'
    sel 'shape0'
    setlocalbox -0.000001, 0.000000, -0.000001, 3.499848, 0.999848, 3.499849
    addanimator "diff"
    setposition 0.000000, 0.000000, 0.000000
    seteuler 0.000000, 0.000000, 0.000000
    setscale 1.000000, 1.000000, 1.000000
    setvector "MatAmbient", 0.300000, 0.300000, 0.300000, 1.000000
    setvector "MatDiffuse", 1.000000, 1.000000, 0.539320, 0.539320
    setvector "MatSpecular", 0.800000, 0.800000, 0.800000, 1.000000
    setfloat "MatSpecularPower", 32.000000
    settexture "DiffMap0", "textures:examples/brick.bmp"
    settexture "BumpMap0", "textures:examples/bump.tga"
    setshader "colr", "shaders:default.fx"
    setmesh "meshes:examples/torus.n3d2"
    setgroupindex 0
    new 'nvectoranimator', 'diff'
        sel 'diff'
        setchannel "time"
        setlooptype "loop"
        setvectorname "MatDiffuse"
        addkey 0.000000, 1.000000, 1.000000, 1.000000, 1.000000
        addkey 20.000000, 0.000000, 1.000000, 1.000000, 1.000000
        addkey 40.000000, 1.000000, 1.000000, 0.000000, 0.000000
        addkey 60.000000, 1.000000, 1.000000, 1.000000, 1.000000
    sel '..'
sel '..'
# ---
# Eof
