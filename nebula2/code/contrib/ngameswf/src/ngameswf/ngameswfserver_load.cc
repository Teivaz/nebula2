//------------------------------------------------------------------------------
//  ngameswfserver_load.cc
//  (C) 2003, 2004 George McBay, Kim, Hyoun Woo
//------------------------------------------------------------------------------
#include "kernel/nkernelserver.h"
#include "ngameswf/ngameswfserver.h"

//-----------------------------------------------------------------------------
/**
    Load .fx shader files and create correspond meshes.
*/
void nGameSwfServer::InitResources()
{
    char resourceLoaderPath[N_MAXPATH];

    // copy the full path of nGameSwfServer.
    // this path will be used when load resources.
    GetFullName(resourceLoaderPath, sizeof(resourceLoaderPath));

    // Setup the shape shader, used for untextured shapes and the background rect.
    nString strShaderShape = this->strShaderRoot;
    strShaderShape.Append ("gswf_shape.fx");
    this->refShapeShader = this->refGfxServer->NewShader("gswf_shape_shader");
    this->refShapeShader->SetFilename(strShaderShape);
    if (!this->refShapeShader->Load())
    {
        n_printf ("nGameSwfServer::InitResource: unable to load ngameswf shape shader.\n");
    }

    // Setup the bitmap shader, used for textured bitmaps(small text)
    nString strShaderBmp = this->strShaderRoot;
    strShaderBmp.Append ("gswf_bitmap.fx");
    this->refBitmapShader = this->refGfxServer->NewShader("gswf_bitmap_shader");
    this->refBitmapShader->SetFilename(strShaderBmp);
    if (!refBitmapShader->Load())
    {
        n_printf("nGameSwfServer::InitResource: unable to load ngameswf bitmap shader.\n");
    }
    
    // Setup the shape-textured-clamp shader, used for textured shapes.
    // (shapes will fills, etc)
    nString strShaderClamp = this->strShaderRoot;
    strShaderClamp.Append ("gswf_shapetexclamp.fx");
    this->refShapeTexClampShader = this->refGfxServer->NewShader("gswf_shapetexclamp_shader");
    this->refShapeTexClampShader->SetFilename(strShaderClamp);
    if (!this->refShapeTexClampShader->Load())
    {
        n_printf("nGameSwfServer::InitResource: unable to load ngameswf shape-textured-clamp shader shader.\n");
    }

    // Setup the shape-textured-wrap shader, used for textured shapes.
    // (shapes will fills, etc)
    nString strShaderWrap = this->strShaderRoot;
    strShaderWrap.Append ("gswf_shapetexwrap.fx");
    this->refShapeTexWrapShader = this->refGfxServer->NewShader("gswf_shapetexwrap_shader");
    this->refShapeTexWrapShader->SetFilename(strShaderWrap);
    if (!refShapeTexWrapShader->Load())
    {
        n_printf("nGameSwfServer::InitResource: unable to load ngameswf shape-textured-wrap shader.\n");
    }

    // Setup the line shader, used for line drawing
    nString strShaderLine = this->strShaderRoot;
    strShaderLine.Append ("gswf_line.fx");
    this->refLineShader = this->refGfxServer->NewShader("gswf_line_shader");
    this->refLineShader->SetFilename(strShaderLine);
    if (!refLineShader->Load())
    {
        n_printf("nGameSwfServer::InitResource: unable to load ngameswf line shader.\n");
    }

    //create correspond meshes.

    this->refBkgMesh = this->refGfxServer->NewMesh("gswf_background_mesh");
    this->refBkgMesh->SetFilename("(not_empty)");
    this->refBkgMesh->SetResourceLoader(resourceLoaderPath);
    this->refBkgMesh->Load ();

    this->refBitmapMesh = this->refGfxServer->NewMesh("gswf_bitmap_mesh");
    this->refBitmapMesh->SetFilename("(not_empty)");
    this->refBitmapMesh->SetResourceLoader(resourceLoaderPath);
    this->refBitmapMesh->Load();

    this->refShapeMesh = refGfxServer->NewMesh("gswf_shape_mesh");
    this->refShapeMesh->SetFilename("(not_empty)");
    this->refShapeMesh->SetResourceLoader(resourceLoaderPath);
    this->refShapeMesh->Load ();

    this->refLineMesh = refGfxServer->NewMesh("gswf_line_mesh");
    this->refLineMesh->SetFilename("(not_empty)");
    this->refLineMesh->SetResourceLoader(resourceLoaderPath);
    this->refLineMesh->Load ();
}

//-----------------------------------------------------------------------------
/**

    @param filename
    @param callingResource

    @return 
*/
bool nGameSwfServer::Load(const char* filename, nResource* callingResource)
{
    ushort* indices;
    int i;

    if (callingResource == refBkgMesh.get())
    {
        refBkgMesh->SetUsage (nMesh2::WriteOnly);
        refBkgMesh->SetVertexComponents (nMesh2::Coord);
        refBkgMesh->SetNumVertices(4);
        refBkgMesh->SetNumIndices (6);

        refBkgMesh->SetFilename ("");
        if (!refBkgMesh->Load ())
            n_printf ("nGameSwfServer::Load: failed to create refBkgMesh.\n");
        refBkgMesh->SetFilename ("(not_empty)");

        indices = refBkgMesh->LockIndices ();
        indices[0] = 0; indices[1] = 1; indices[2] = 2;
        indices[3] = 2; indices[4] = 1; indices[5] = 3;
        refBkgMesh->UnlockIndices ();
    }
    else
    if (callingResource == refBitmapMesh.get())
    {
        refBitmapMesh->SetUsage (nMesh2::WriteOnly);
        refBitmapMesh->SetVertexComponents (nMesh2::Coord|nMesh2::Uv0);
        refBitmapMesh->SetNumVertices(4);
        refBitmapMesh->SetNumIndices (6);

        refBitmapMesh->SetFilename ("");
        if (!refBitmapMesh->Load ())
            n_printf ("nGameSwfServer::Load: failed to create refBitmapMesh.\n");
        refBitmapMesh->SetFilename ("(not_empty)");

        indices = refBitmapMesh->LockIndices();
        indices[0] = 0; indices[1] = 1; indices[2] = 2;
        indices[3] = 2; indices[4] = 1; indices[5] = 3;
        refBitmapMesh->UnlockIndices();
    }
    else
    if (callingResource == refShapeMesh.get())
    {
        refShapeMesh->SetUsage (nMesh2::WriteOnly);
        refShapeMesh->SetVertexComponents (nMesh2::Coord);
        refShapeMesh->SetNumVertices(VERTEX_BUFFER_INITIAL_SIZE);
        refShapeMesh->SetNumIndices (VERTEX_BUFFER_INITIAL_SIZE);

        refShapeMesh->SetFilename ("");
        if (!refShapeMesh->Load ())
            n_printf ("nGameSwfServer::Load: failed to create refShapeMesh.\n");
        refShapeMesh->SetFilename ("(not_empty)");

        indices = refShapeMesh->LockIndices ();
        for (i=0; i<VERTEX_BUFFER_INITIAL_SIZE; ++i)
            indices[i] = i;
        refShapeMesh->UnlockIndices ();
    }
    else
    if (callingResource == refLineMesh.get())
    {
        refLineMesh->SetUsage (nMesh2::WriteOnly);
        refLineMesh->SetVertexComponents (nMesh2::Coord);
        refLineMesh->SetNumVertices(VERTEX_BUFFER_INITIAL_SIZE);
        refLineMesh->SetNumIndices (VERTEX_BUFFER_INITIAL_SIZE);

        refLineMesh->SetFilename ("");
        if (!refLineMesh->Load ())
            n_printf ("nGameSwfServer::Load: failed to create refLineMesh.\n");
        refLineMesh->SetFilename ("(not_empty)");

        indices = refLineMesh->LockIndices ();
        for (i=0; i<VERTEX_BUFFER_INITIAL_SIZE; ++i)
            indices[i] = i;
        refLineMesh->UnlockIndices ();
    }
    else
    {
        // Not paticularly efficient, consider using a hash?
        // Not really  a speed critical path though.
        for (int i=0; i<bitmap_info_array.Size(); ++i)
        {
            bitmap_info_nebula* bi = dynamic_cast<bitmap_info_nebula*>(bitmap_info_array[i]);

            if (bi != NULL && bi->ref_texture.get() == callingResource)
            {
                bi->load(refGfxServer.get());
                return true;
            }
        }
    }

    return true;
}
