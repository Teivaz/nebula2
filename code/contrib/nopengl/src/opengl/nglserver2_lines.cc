//------------------------------------------------------------------------------
//  nglserver2_lines.cc
//------------------------------------------------------------------------------
#include "opengl/nglserver2.h"

//------------------------------------------------------------------------------
/**
    Begin rendering lines.
*/
void
nGLServer2::BeginLines()
{
    nGfxServer2::BeginLines();
}

//------------------------------------------------------------------------------
/**
    Render a 3d line strip using the current transformations.
*/
void
nGLServer2::DrawLines3d(const vector3* vertexList, int numVertices, const vector4& color)
{
    n_assert(vertexList);
    n_assert(numVertices > 0);
    n_assert(this->inBeginLines);

    glVertexPointer(3, GL_FLOAT, sizeof(vector3), vertexList);
    glColor4f(color.x, color.y, color.z, color.w);
    glDrawArrays(GL_LINES, 0, numVertices);

    n_gltrace("nGLServer2::DrawLines3d().");
}

//------------------------------------------------------------------------------
/**
    Render a 3d line strip using the current transformations. Careful:
    Clipping doesn't work correctly!
*/
void
nGLServer2::DrawLines2d(const vector2* vertexList, int numVertices, const vector4& color)
{
    n_assert(vertexList);
    n_assert(numVertices > 0);
    n_assert(this->inBeginLines);

    glVertexPointer(2, GL_FLOAT, sizeof(vector2), vertexList);
    glColor4f(color.x, color.y, color.z, color.w);
    glDrawArrays(GL_LINES, 0, numVertices);

    n_gltrace("nGLServer2::DrawLines2d().");
}

//------------------------------------------------------------------------------
/**
    Finish rendering lines.
*/
void
nGLServer2::EndLines()
{
    nGfxServer2::EndLines();
}

