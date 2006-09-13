//------------------------------------------------------------------------------
/**
    @page NebulaToolsnmeshtool nmeshtool

    nmeshtool

    Command line tool for mesh manipulation.

     @par -in
       input mesh file (obj, n3d, n3d2, nvx or nvx2 file)
     @par -out
       output mesh file (n3d, n3d2, nvx2)
     @par -oldn3d2
        use the the old (files saved before Dec-2003) n3d2 loader code
     @par -clean
       clean the mesh before anything else
     @par -tangent
       generate tangents
     @par -tangentsplit
       generate tangents using an alternative technique that may split vertices
     @par -edge
       generate edges
     @par -append
       name of mesh to append (note: duplicate group names are renamed)
     @par -group
       optional group name, if exists, all operations only affect this group
     @par -grename
       rename the group defined by group
     @par -tx
       translate x
     @par -ty
       translate y
     @par -tz
       translate z
     @par -rx
       rotate x
     @par -ry
       rotate y
     @par -rz
       rotate z
     @par -sx
       scale x
     @par -sy
       scale y
     @par -sz
       scale z
     @par -scale
       uniform scale

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
    nString inFileArg          = args.GetStringArg("-in", 0);
    nString outFileArg         = args.GetStringArg("-out", 0);
    bool oldN3d2Loader         = args.GetBoolArg("-oldn3d2");
    nString appendFileArg      = args.GetStringArg("-append", 0);
    bool cleanArg              = args.GetBoolArg("-clean");
    bool tangentNoSplitArg     = args.GetBoolArg("-tangent");
    bool tangentSplitArg       = args.GetBoolArg("-tangentsplit");
    bool edgeArg               = args.GetBoolArg("-edge");
    nString groupArg           = args.GetStringArg("-group", 0);
    nString groupRenameArg     = args.GetStringArg("-grename", 0);
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
    nString delComponentArg    = args.GetStringArg("-delcomponent", 0);

    // show help?
    if (helpArg)
    {
        printf("(C) 2003 RadonLabs GmbH\n"
                 "nmeshtool - Nebula2 mesh file tool\n\n"
                 "Command line args:\n"
                 "------------------\n"
                 "-help                 show this help\n"
                 "-in [filename]        input mesh file (.obj, .n3d, .n3d2, .nvx or .nvx2 extension)\n"
                 "-out [filename]       output mesh file (.n3d2 or .nvx2 extension)\n"
                 "-oldn3d2              use the the old n3d2 loader code\n"
                 "-append [filename]    optional mesh file to append to input mesh\n"
                 "-clean                clean up mesh (removes redundant vertices)\n"
                 "-tangent              generate vertex tangents for per pixel lighting using\n"
                 "                      a technique that will not split vertices\n"
                 "-tangentsplit         generate vertex tangents for per pixel lighting using\n"
                 "                      a technique that may split vertices\n"
                 "-edge                 generate edge data\n"
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
                 "-scale [float]        uniformly scale vertices\n"
                 "-delcomponent [c]     delete a vertex component from all vertices\n");
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

    if ((translate.len() > 0.0f) || (rotate.len() > 0.0f) ||
        !(scale.x == 1.0f && scale.y == 1.0f && scale.z == 1.0f))
    {
        doTransform = true;
    }

    // startup Nebula
    nKernelServer* kernelServer = new nKernelServer;

    // read input mesh
    n_printf("-> loading mesh '%s'\n", inFileArg.Get());
    nString filename = inFileArg;
    if (oldN3d2Loader && (0 == strcmp(filename.GetExtension(), "n3d2")))
    {
        n_printf("-> using old n3d2 loader code\n");
        if (!mesh.LoadOldN3d2(kernelServer->GetFileServer(), inFileArg.Get()))
        {
            n_printf("nmeshtool error: Could not load '%s'\n", inFileArg.Get());
            delete kernelServer;
            return 5;
        }
    }
    else
    {
        if (!mesh.Load(kernelServer->GetFileServer(), inFileArg.Get()))
        {
            n_printf("nmeshtool error: Could not load '%s'\n", inFileArg.Get());
            delete kernelServer;
            return 5;
        }
    }

    // read optional append mesh
    if (appendFileArg.IsValid())
    {
        n_printf("-> loading append mesh '%s'\n", appendFileArg.Get());
        if (!appendMesh.Load(kernelServer->GetFileServer(), appendFileArg.Get()))
        {
            n_printf("nmeshtool error: Could not load append mesh '%s'\n", appendFileArg.Get());
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
    if (tangentNoSplitArg || tangentSplitArg)
    {
        n_printf("-> generating tangents...\n");
        mesh.BuildTriangleNormals();
        mesh.BuildVertexTangents(tangentSplitArg);
    }

    // generate edges?
    if (edgeArg)
    {
        n_printf("-> generating edges...\n");
        mesh.CreateEdges();
    }

    if (delComponentArg.IsValid())
    {
        nMeshBuilder::Vertex::Component c = nMeshBuilder::Vertex::NUM_VERTEX_COMPONENTS;
        if (delComponentArg == "coord")
        {
            c = nMeshBuilder::Vertex::COORD;
        }
        else if (delComponentArg == "normal")
        {
            c = nMeshBuilder::Vertex::NORMAL;
        }
        else if (delComponentArg == "uv0")
        {
            c = nMeshBuilder::Vertex::UV0;
        }
        else if (delComponentArg == "uv1")
        {
            c = nMeshBuilder::Vertex::UV1;
        }
        else if (delComponentArg == "uv2")
        {
            c = nMeshBuilder::Vertex::UV2;
        }
        else if (delComponentArg == "uv3")
        {
            c = nMeshBuilder::Vertex::UV3;
        }
        else if (delComponentArg == "color")
        {
            c = nMeshBuilder::Vertex::COLOR;
        }
        else if (delComponentArg == "tangent")
        {
            c = nMeshBuilder::Vertex::TANGENT;
        }
        else if (delComponentArg == "binormal")
        {
            c = nMeshBuilder::Vertex::BINORMAL;
        }
        else if (delComponentArg == "weights")
        {
            c = nMeshBuilder::Vertex::WEIGHTS;
        }
        else if (delComponentArg == "jindices")
        {
            c = nMeshBuilder::Vertex::JINDICES;
        }
        if (c != nMeshBuilder::Vertex::NUM_VERTEX_COMPONENTS)
        {
            n_printf("-> deleting vertex component %s...\n", delComponentArg.Get());
            mesh.DelVertexComponent(c);
        }
    }

    // FIXME: group renaming

    // save output mesh
    if (outFileArg.IsValid())
    {
        n_printf("-> saving '%s'\n", outFileArg.Get());
        if (!mesh.Save(kernelServer->GetFileServer(), outFileArg.Get()))
        {
            n_printf("Error: Could not save '%s'\n", outFileArg.Get());
            delete kernelServer;
            return 5;
        }
    }

    // success
    delete kernelServer;
    return 0;
}





