//------------------------------------------------------------------------------
/**
    @page NebulaToolsnmeshtool nmeshtool

    nmeshtool
    
    Command line tool for mesh manipulation.

    <dl>
     <dt>-in</dt>
       <dd>input mesh file (n3d, n3d2, nvx or nvx2 file)</dd>
     <dt>-out</dt>
       <dd>output mesh file (file extension defines output format)</dd>
     <dt>-oldn3d2</dt>
        <dd>use the the old (files saved before Dec-2003) n3d2 loader code</dd>
     <dt>-clean</dt>
       <dd>clean the mesh before anything else</dd>
     <dt>-tangent</dt>
       <dd>generate tangents</dd>
     <dt>-append</dt>
       <dd>name of mesh to append (note: duplicate group names are renamed)</dd>
     <dt>-group</dt>
       <dd>optional group name, if exists, all operations only affect this group</dd>
     <dt>-grename</dt>
       <dd>rename the group defined by group</dd>
     <dt>-tx</dt>
       <dd>translate x</dd>
     <dt>-ty</dt>
       <dd>translate y</dd>
     <dt>-tz</dt>
       <dd>translate z</dd>
     <dt>-rx</dt>
       <dd>rotate x</dd>
     <dt>-ry</dt>
       <dd>rotate y</dd>
     <dt>-rz</dt>
       <dd>rotate z</dd>
     <dt>-sx</dt>
       <dd>scale x</dd>
     <dt>-sy</dt>
       <dd>scale y</dd>
     <dt>-sz</dt>
       <dd>scale z</dd>
     <dt>-scale</dt>
       <dd>uniform scale</dd>
    </dl>

    (C) 2003 RadonLabs GmbH
*/
#include "kernel/nkernelserver.h"
#include "tools/ncmdlineargs.h"
#include "tools/nmeshbuilder.h"

//------------------------------------------------------------------------------
int
main(int argc, const char** argv)
{
    nCmdLineArgs args(argc, argv);
    nMeshBuilder mesh;
    nMeshBuilder appendMesh;

    // get cmd line args
    bool helpArg               = args.GetBoolArg("-help");
    const char* inFileArg      = args.GetStringArg("-in", 0);
    const char* outFileArg     = args.GetStringArg("-out", 0);
    bool oldN3d2Loader         = args.GetBoolArg("-oldn3d2");
    const char* appendFileArg  = args.GetStringArg("-append", 0);
    bool cleanArg              = args.GetBoolArg("-clean");
    bool tangentArg            = args.GetBoolArg("-tangent");
    const char* groupArg       = args.GetStringArg("-group", 0);
    const char* groupRenameArg = args.GetStringArg("-grename", 0);
    float txArg                = args.GetFloatArg("-tx", 0.0f);
    float tyArg                = args.GetFloatArg("-ty", 0.0f);
    float tzArg                = args.GetFloatArg("-tz", 0.0f);
    float rxArg                = args.GetFloatArg("-rx", 0.0f);
    float ryArg                = args.GetFloatArg("-ry", 0.0f);
    float rzArg                = args.GetFloatArg("-rz", 0.0f);
    float sxArg                = args.GetFloatArg("-sx", 1.0f);
    float syArg                = args.GetFloatArg("-sy", 1.0f);
    float szArg                = args.GetFloatArg("-sz", 1.0f);
    float scaleArg             = args.GetFloatArg("-scale", 0.0f);

    // show help?
    if (helpArg)
    {
        printf("(C) 2003 RadonLabs GmbH\n"
                 "nmeshtool - Nebula2 mesh file tool\n\n"
                 "Command line args:\n"
                 "------------------\n"
                 "-help                 show this help\n"
                 "-in [filename]        input mesh file (.n3d, .n3d2, .nvx or .nvx2 extension)\n"
                 "-out [filename]       output mesh file (.n3d2 or .nvx2 extension)\n"
                 "-oldn3d2              use the the old n3d2 loader code"
                 "-append [filename]    optional mesh file to append to input mesh\n"
                 "-clean                clean up mesh (removes redundant vertices)\n"
                 "-tangent              generate vertex tangents for per pixel lighting\n"
                 "-group [groupname]    select a group inside the mesh\n"
                 "-grename [newname]    rename the selected group\n"
                 "-tx [float]           translate vertices along x\n"
                 "-ty [float]           translate vertices along y\n"
                 "-tz [float]           translate vertices along z\n"
                 "-rx [float]           rotate vertices around x\n"
                 "-ry [float]           rotate vertices around y\n"
                 "-rz [float]           rotate vertices around z\n"
                 "-sx [float]           scale vertices along x\n"
                 "-sy [float]           scale vertices along y\n"
                 "-sz [float]           scale vertices along z\n"
                 "-scale [float]        uniformly scale vertices\n");
        return 5;
    }

    // error if no input file given
    if (0 == inFileArg)
    {
        printf("nmeshtool error: No input file! (type 'nmeshtool -help' for help)\n");
        return 5;
    }

    // decide if we should transform at all
    bool doTransform = false;
    vector3 translate(txArg, tyArg, tzArg);
    vector3 rotate(rxArg, ryArg, rzArg);
    vector3 scale(sxArg, syArg, szArg);
    if (scaleArg != 0.0f)
    {
        scale.set(scaleArg, scaleArg, scaleArg);
    }
/*
    if ((translate.len() > 0.0f) || (rotate.len() > 0.0f) || (scale.len() != 1.0f))
    {
        doTransform = true;
    }
*/

    // startup Nebula
    nKernelServer* kernelServer = new nKernelServer;

    // read input mesh
    n_printf("-> loading mesh '%s'\n", inFileArg);
    nPathString filename = inFileArg;
    if (oldN3d2Loader && (0 == strcmp(filename.GetExtension(), "n3d2")))
    {
        n_printf("-> using load n3d2 loader code\n");
        if (!mesh.LoadOldN3d2(kernelServer->GetFileServer(), inFileArg))
        {
            n_printf("nmeshtool error: Could not load '%s'\n", inFileArg);
            delete kernelServer;
            return 5;
        }
    }
    else
    {
        if (!mesh.Load(kernelServer->GetFileServer(), inFileArg))
        {
            n_printf("nmeshtool error: Could not load '%s'\n", inFileArg);
            delete kernelServer;
            return 5;
        }
    }

    // read optional append mesh
    if (appendFileArg)
    {
        n_printf("-> loading append mesh '%s'\n", appendFileArg);
        if (!appendMesh.Load(kernelServer->GetFileServer(), appendFileArg))
        {
            n_printf("nmeshtool error: Could not load append mesh '%s'\n", appendFileArg);
            delete kernelServer;
            return 5;
        }

        // append mesh
        mesh.Append(appendMesh);
    }

    // cleanup?
    if (cleanArg)
    {
        n_printf("-> cleaning...\n");
        mesh.Cleanup(0);
    }

    // transform?
    if (doTransform)
    {
        n_printf("-> transforming...\n");
        matrix44 m;
        if (scale.len() != 1.0f)
        {
            m.scale(scale);
        }
        if (rotate.len() > 0.0f)
        {
            m.rotate_x(rotate.x);
            m.rotate_y(rotate.y);
            m.rotate_z(rotate.z);
        }
        if (translate.len() > 0.0f)
        {
            m.translate(translate);
        }
        mesh.Transform(m);
    }

    // generate tangents?
    if (tangentArg)
    {
        n_printf("-> generating tangents...\n");
        mesh.BuildTriangleNormals();
        mesh.BuildVertexTangents();
    }

    // FIXME: group renaming

    // save output mesh
    if (outFileArg)
    {
        n_printf("-> saving '%s'\n", outFileArg);
        if (!mesh.Save(kernelServer->GetFileServer(), outFileArg))
        {
            n_printf("Error: Could not save '%s'\n", outFileArg);
            delete kernelServer;
            return 5;
        }
    }

    // success
    delete kernelServer;
    return 0;
}




    
