//------------------------------------------------------------------------------
//  ngameswfserver_render.cc
//  (C) 2003, 2004 George McBay, Kim, Hyoun Woo
//------------------------------------------------------------------------------
#include "kernel/nkernelserver.h"
#include "ngameswf/ngameswfserver.h"

//-----------------------------------------------------------------------------
/**
    Set up to render a full frame from a movie and fills the background.
    Sets up necessary transforms, to scale the movie to fit within the given 
    dimensions.  Call end_display() when you're done.

    @param bkgColor background color
    @param vp_x viewport x
    @param vp_y viewport y
    @param vp_w viewport width
    @param vp_h viewport height
    @param x0
    @param x1
    @param y0
    @param y1
*/
void nGameSwfServer::begin_display(gameswf::rgba bkgColor, 
                                   int vp_x, int vp_y, int vp_w, int vp_h, 
                                   float x0, float x1, float y0, float y1)
{
    nGfxServer2* gs = refGfxServer.get();

    // Set up modelview and projection, viewport matrices that will
    // be used in the shader to transform all gameswf coordinate to 
    // the screen.
    float gsWidthDiv  = 1.0f / (float)gs->GetDisplayMode().GetWidth();
    float gsHeightDiv = 1.0f / (float)gs->GetDisplayMode().GetHeight();
    
    modelViewMatrix.M11 = 2.0f / (x1 - x0);
    modelViewMatrix.M22 = -2.0f / (y1 - y0);
    modelViewMatrix.M41 = -((x1 + x0) / (x1 - x0));
    modelViewMatrix.M42 = ((y1 + y0) / (y1 - y0));

    projMatrix.M11 = vp_w * gsWidthDiv;
    projMatrix.M22 = vp_h * gsHeightDiv;
    projMatrix.M41 = -1.0f + vp_x * 2.0f * gsWidthDiv + vp_w * gsWidthDiv;
    projMatrix.M42 = 1.0f - vp_y * 2.0f * gsHeightDiv - vp_h * gsHeightDiv;

    if (!refShapeShader.isvalid())
    {
        this->InitResources();
    }

    if (bkgColor.m_a > 0)
    {
        float4 backgroundColor;

        backgroundColor[0] = bkgColor.m_r * ccByteToFloat;
        backgroundColor[1] = bkgColor.m_g * ccByteToFloat;
        backgroundColor[2] = bkgColor.m_b * ccByteToFloat;
        backgroundColor[3] = bkgColor.m_a * ccByteToFloat;

        matrix44 coordTransform = modelViewMatrix * projMatrix;

        refShapeShader->SetMatrix(nShader2::Parameter::ModelViewProjection, coordTransform);
        refShapeShader->SetFloatArray(nShader2::Parameter::MatDiffuse, backgroundColor, 4);

        float* verts = refBkgMesh->LockVertices();

        verts[0] = x0; verts[1]  = y0;
        verts[3] = x0; verts[4]  = y1;
        verts[6] = x1; verts[7]  = y0;
        verts[9] = x1; verts[10] = y1;

        refBkgMesh->UnlockVertices();

        gs->SetMesh(0, refBkgMesh.get());
        gs->SetShader(refShapeShader.get());
        gs->SetVertexRange(0, 4);
        gs->SetIndexRange(0, 6);
        gs->Draw(TriangleStrip);
    }
}

//-----------------------------------------------------------------------------
/**
*/
void nGameSwfServer::end_display()
{
    // This space intentionally left black.
}

//-----------------------------------------------------------------------------
/**
    Draw strip element.
*/
void nGameSwfServer::draw_mesh_strip(const void* coords, int vertex_count)
{
    nGfxServer2* gs = refGfxServer.get();

    if (vertex_count > refShapeMesh->GetNumVertices())
    {
        refShapeMesh->Unload();

        refShapeMesh->SetUsage(nMesh2::WriteOnly);
        refShapeMesh->SetVertexComponents(nMesh2::Coord);
        refShapeMesh->SetNumVertices(vertex_count);
        refShapeMesh->SetNumIndices(vertex_count);
        refShapeMesh->SetFilename("");
        refShapeMesh->Load();
        refShapeMesh->SetFilename("(not_empty)");

        ushort* indices = refShapeMesh->LockIndices();
        for (int i=0; i<vertex_count; ++i)
            indices[i] = i;
        refShapeMesh->UnlockIndices();
    }

    Sint16* coordArray = (Sint16*)coords;
    int coord_count = vertex_count * 2;
    float* vertices = refShapeMesh->LockVertices();
    for(int i=0; i<coord_count; i+=2)
    {
        *(vertices++) = coordArray[i];
        *(vertices++) = coordArray[i+1];
        *(vertices++);
        // z component are not initialized as they are unused
        // hardcorded to 0.0 in the shader.
    }
    refShapeMesh->UnlockVertices();

    if(current_styles[LEFT_STYLE].m_mode == fill_style::COLOR)
    {
        gs->SetShader(refShapeShader.get());
    }
    else
    {
        if(current_styles[LEFT_STYLE].m_mode == fill_style::BITMAP_WRAP)
            gs->SetShader(refShapeTexWrapShader.get());
        else
            gs->SetShader(refShapeTexClampShader.get());
    }

    gs->SetMesh(0, refShapeMesh.get());
    gs->SetVertexRange(0, vertex_count);
    gs->SetIndexRange(0, vertex_count);
    gs->Draw(TriangleStrip);
}

//-----------------------------------------------------------------------------
/**
    Draw a rectangle textured with the given bitmap, with the given color. 
    Apply given transform; ignore any currently set transforms.

*/
void nGameSwfServer::draw_bitmap(const gameswf::matrix& m,
                                 const gameswf::bitmap_info* bi,
                                 const gameswf::rect& coords,
                                 const gameswf::rect& uv_coords, 
                                 gameswf::rgba color)
{
    float4 backgroundColor;

    backgroundColor[0] = color.m_r * ccByteToFloat;
    backgroundColor[1] = color.m_g * ccByteToFloat;
    backgroundColor[2] = color.m_b * ccByteToFloat;
    backgroundColor[3] = color.m_a * ccByteToFloat;

    matrix44 coordTransform = modelViewMatrix * projMatrix;

    refBitmapShader->SetMatrix(nShader2::Parameter::ModelViewProjection, coordTransform);
    refBitmapShader->SetFloatArray(nShader2::Parameter::MatDiffuse, backgroundColor, 4);

    gameswf::point a, b, c, d;
    m.transform(&a, gameswf::point(coords.m_x_min, coords.m_y_min));
    m.transform(&b, gameswf::point(coords.m_x_max, coords.m_y_min));
    m.transform(&c, gameswf::point(coords.m_x_min, coords.m_y_max));
    d.m_x = b.m_x + c.m_x - a.m_x;
    d.m_y = b.m_y + c.m_y - a.m_y;

    float* verts = refBitmapMesh->LockVertices();
    verts[0] = a.m_x;
    verts[1] = a.m_y;
    verts[3] = uv_coords.m_x_min;
    verts[4] = uv_coords.m_y_min;
    
    verts[5] = b.m_x;
    verts[6] = b.m_y;
    verts[8] = uv_coords.m_x_max;
    verts[9] = uv_coords.m_y_min;
    
    verts[10] = c.m_x;
    verts[11] = c.m_y;
    verts[13] = uv_coords.m_x_min;
    verts[14] = uv_coords.m_y_max;
    
    verts[15] = d.m_x;
    verts[16] = d.m_y;
    verts[18] = uv_coords.m_x_max;
    verts[19] = uv_coords.m_y_max;
    refBitmapMesh->UnlockVertices();

    nGfxServer2* gs = refGfxServer.get ();
    gs->SetMesh (0, refBitmapMesh.get ());
    refBitmapShader->SetTexture (nShader2::Parameter::DiffMap0, ((bitmap_info_nebula*)bi)->ref_texture.get ());

    gs->SetShader (refBitmapShader.get ());
    gs->SetVertexRange (0, 4);
    gs->SetIndexRange (0, 6);
    gs->Draw (TriangleStrip);
}

//-----------------------------------------------------------------------------
/**
    Draw the line strip formed by the sequence of points.
*/
void nGameSwfServer::draw_line_strip(const void* coords, int vertex_count)
{
    nGfxServer2* gs = refGfxServer.get();

    if (vertex_count > refLineMesh->GetNumVertices ())
    {
        refLineMesh->Unload ();

        refLineMesh->SetUsage (nMesh2::WriteOnly);
        refLineMesh->SetVertexComponents (nMesh2::Coord);
        refLineMesh->SetNumVertices (vertex_count);
        refLineMesh->SetNumIndices (vertex_count);
        refLineMesh->Load ();

        ushort* indicies = refLineMesh->LockIndices ();

        for(int i=0; i<vertex_count; ++i)
            indicies[i] = i;
        refLineMesh->UnlockIndices ();
    }

    int coord_count = vertex_count * 2;

    Sint16* coordArray = (Sint16*)coords;

    float* vertices = refLineMesh->LockVertices ();
    for (int i=0; i<coord_count; i+=2)
    {
        *(vertices++) = coordArray[i];
        *(vertices++) = coordArray[i+1];
        *(vertices++);
    }
    refLineMesh->UnlockVertices ();

    gs->SetMesh (0, refLineMesh.get ());
    gs->SetShader (refLineShader.get ());
    gs->SetVertexRange (0, vertex_count);
    gs->SetIndexRange (0, vertex_count);
    gs->Draw(LineStrip);
}