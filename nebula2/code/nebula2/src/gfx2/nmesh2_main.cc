//------------------------------------------------------------------------------
//  nmesh2_main.cc
//  (C) 2002 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gfx2/nmesh2.h"
#include "kernel/nkernelserver.h"

nNebulaClass(nMesh2, "nresource");

//------------------------------------------------------------------------------
/**
*/
nMesh2::nMesh2() :
    usage(WriteOnce),
    vertexComponentMask(0),
    vertexWidth(0),
    numVertices(0),
    numIndices(0),
    numGroups(0),
    groups(0),
    vertexBufferByteSize(0),
    indexBufferByteSize(0),
    refillBuffersMode(Disabled)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nMesh2::~nMesh2()
{
    if (this->IsValid())
    {
        this->Unload();
    }
}

//------------------------------------------------------------------------------
/**
    Unload everything.
*/
void
nMesh2::UnloadResource()
{
    if (this->groups)
    {
        delete[] this->groups;
        this->groups = 0;
    }

    // NOTE: do not clear numVertices, numIndices, vertexWidth, 
    // etc. Those values may be needed in a following call to Load()!
    this->vertexBufferByteSize = 0;
    this->indexBufferByteSize  = 0;
}

//------------------------------------------------------------------------------
/**
    Lock vertices in mesh. Return pointer to beginning of vertex buffer.
*/
float*
nMesh2::LockVertices()
{
    // empty
    return 0;
}

//------------------------------------------------------------------------------
/**
    Unlock vertices in mesh.
*/
void
nMesh2::UnlockVertices()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Lock indices in mesh. Return pointer to start of indices.
*/
ushort*
nMesh2::LockIndices()
{
    // empty
    return 0;
}

//------------------------------------------------------------------------------
/**
    Unlock indices in mesh.
*/
void
nMesh2::UnlockIndices()
{
    // empty
}
//------------------------------------------------------------------------------
/**
    Converts the flags into a string of the form
    "ReadOnly|NeedsVertexShader|"
    (includes a trailing '|')
*/

nString nMesh2::ConvertUsageFlagsToString( int usageFlags )
{
    nString usageString;
    if( usageFlags )
    {
        if( usageFlags & nMesh2::WriteOnce ) usageString += "WriteOnce|";
        else if( usageFlags & nMesh2::ReadOnly ) usageString += "ReadOnly|";
        else if( usageFlags & nMesh2::WriteOnly ) usageString += "WriteOnly|";
        else if( usageFlags & nMesh2::NPatch ) usageString += "NPatch|";
        else if( usageFlags & nMesh2::RTPatch ) usageString += "RTPatch|";
        else if( usageFlags & nMesh2::PointSprite ) usageString += "PointSprite|";
        else if( usageFlags & nMesh2::NeedsVertexShader ) usageString += "NeedsVertexShader|";
        else n_error( "Invalid usage flags %d in n_getmeshusage (nShapeNode)!", usageFlags );
    }
    return usageString;
}
//------------------------------------------------------------------------------
/**
    Converts a set of usage flags represented as a string of the form
    "ReadOnly|NeedsVertexShader"
    into the corresponding integer representation.
    Trailing '|'s are ignored, as is case.
*/
int nMesh2::ConvertUsageStringToFlags( const char* usageFlagsString )
{
    int usage = 0;
    nString workingString = usageFlagsString;
    if( !workingString.IsEmpty() )
    {
        workingString.ToLower();
        const char* flagString = workingString.GetFirstToken( "|" );
        while (flagString)
        {   
            if( !strcmp( flagString, "writeonce" ) ) usage |= nMesh2::WriteOnce;
            else if( !strcmp( flagString, "readonly" ) ) usage |= nMesh2::ReadOnly;
            else if( !strcmp( flagString, "writeonly" ) ) usage |= nMesh2::WriteOnly;
            else if( !strcmp( flagString, "npatch" ) ) usage |= nMesh2::NPatch;
            else if( !strcmp( flagString, "rtpatch" ) ) usage |= nMesh2::RTPatch;
            else if( !strcmp( flagString, "pointsprite" ) ) usage |= nMesh2::PointSprite;
            else if( !strcmp( flagString, "needsvertexshader" ) ) usage |= nMesh2::NeedsVertexShader;
            else n_error( "Invalid flag string '%s' in n_setmeshusage (nShapeNode)!", flagString );
            flagString = workingString.GetNextToken( "|" );
        }
    }
    return usage;
}