//------------------------------------------------------------------------------
//	nd3d9mesharray_main.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gfx2/nd3d9mesharray.h"
#include "kernel/nkernelserver.h"
#include "gfx2/nd3d9server.h"
#include "gfx2/nd3d9mesh.h"

nNebulaClass(nD3D9MeshArray, "nmesharray");

//------------------------------------------------------------------------------
/**
*/
nD3D9MeshArray::nD3D9MeshArray() :
    vertexDeclarationDirty(true),
    vertexDeclaration(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nD3D9MeshArray::~nD3D9MeshArray()
{
    if (this->IsValid())
    {
        this->Unload();
    }
    
    if (0 != this->vertexDeclaration)
    {
        this->vertexDeclaration->Release();
        this->vertexDeclaration = 0;
    }   
}

//------------------------------------------------------------------------------
/**
    Creates a vertex declaration based on the vertex components of all valid
    meshes.
*/
void
nD3D9MeshArray::CreateVertexDeclaration()
{
    nD3D9Server* gfxServer = (nD3D9Server*) nGfxServer2::Instance();
    n_assert(gfxServer && gfxServer->d3d9Device);
    
    const int maxElements = 11; //the maximum number of vertex compontents
    D3DVERTEXELEMENT9 decl[maxElements * nGfxServer2::MaxVertexStreams];

    int meshIndex;
    int curElement = 0;
    int postionUsageIndex = 0;
    int normalUsageIndex = 0;
    int tangentUsageIndex = 0;
    int binormalUsageIndex = 0;
    int colorUsageIndex = 0;
    int weightsUsageIndex = 0;
    int jindciesUsageIndex = 0;
    for (meshIndex = 0; meshIndex < nGfxServer2::MaxVertexStreams; meshIndex++)
    {
        nMesh2* mesh = this->GetMeshAt(meshIndex);
        if (0 != mesh)
        {
            int curOffset  = 0;
            int vertexComponentMask = ((nD3D9Mesh*) mesh)->GetVertexComponents();
            int index;            
            for (index = 0; index < maxElements; index++)
            {
                int mask = (1<<index);
                if (vertexComponentMask & mask)
                {
                    bool ignoreElement = false;
                    decl[curElement].Stream = meshIndex; // to which vertex stream it referres
                    decl[curElement].Offset = curOffset;
                    decl[curElement].Method = D3DDECLMETHOD_DEFAULT;
                    switch (mask)
                    {
                        case nMesh2::VertexComponent::Coord:
                            decl[curElement].Type       = D3DDECLTYPE_FLOAT3;
                            decl[curElement].Usage      = D3DDECLUSAGE_POSITION;
                            // usageIndex specifies the index in the vertex shader semantic, i.e.
                            // UsageIndex = 3 results in POSITION3
                            decl[curElement].UsageIndex = postionUsageIndex++;
                            curOffset += 3 * sizeof(float);
                            break;
                                      
                        case nMesh2::VertexComponent::Normal:
                            decl[curElement].Type       = D3DDECLTYPE_FLOAT3;
                            decl[curElement].Usage      = D3DDECLUSAGE_NORMAL;
                            decl[curElement].UsageIndex = normalUsageIndex++;
                            curOffset += 3 * sizeof(float);
                            break;
                                                           
                        case nMesh2::VertexComponent::Tangent:
                            decl[curElement].Type       = D3DDECLTYPE_FLOAT3;
                            decl[curElement].Usage      = D3DDECLUSAGE_TANGENT;
                            decl[curElement].UsageIndex = tangentUsageIndex++;
                            curOffset += 3 * sizeof(float);
                            break;                           
                                                         
                        case nMesh2::VertexComponent::Binormal:
                            decl[curElement].Type       = D3DDECLTYPE_FLOAT3;
                            decl[curElement].Usage      = D3DDECLUSAGE_BINORMAL;
                            decl[curElement].UsageIndex = binormalUsageIndex++;
                            curOffset += 3 * sizeof(float);
                            break;
                                                           
                        case nMesh2::VertexComponent::Color:
                            decl[curElement].Type       = D3DDECLTYPE_FLOAT4;
                            decl[curElement].Usage      = D3DDECLUSAGE_COLOR;
                            decl[curElement].UsageIndex = colorUsageIndex++;
                            curOffset += 4 * sizeof(float);
                            break;  

                        case nMesh2::VertexComponent::Uv0:
                            if (meshIndex == 0)
                            {
                                decl[curElement].Type       = D3DDECLTYPE_FLOAT2;
                                decl[curElement].Usage      = D3DDECLUSAGE_TEXCOORD;
                                decl[curElement].UsageIndex = 0;
                                curOffset += 2 * sizeof(float);
                            }
                            else
                            {
                                ignoreElement = true;
                            }
                            break;
                                      
                        case nMesh2::VertexComponent::Uv1:
                            if (meshIndex == 0)
                            {
                                decl[curElement].Type       = D3DDECLTYPE_FLOAT2;
                                decl[curElement].Usage      = D3DDECLUSAGE_TEXCOORD;
                                decl[curElement].UsageIndex = 1;
                                curOffset += 2 * sizeof(float);
                            }
                            else
                            {
                                ignoreElement = true;
                            }
                            break;

                        case nMesh2::VertexComponent::Uv2:
                            if (meshIndex == 0)
                            {
                                decl[curElement].Type       = D3DDECLTYPE_FLOAT2;
                                decl[curElement].Usage      = D3DDECLUSAGE_TEXCOORD;
                                decl[curElement].UsageIndex = 2;
                                curOffset += 2 * sizeof(float);
                            }
                            else
                            {
                                ignoreElement = true;
                            }
                            break;

                        case nMesh2::VertexComponent::Uv3:
                            if (meshIndex == 0)
                            {
                                decl[curElement].Type       = D3DDECLTYPE_FLOAT2;
                                decl[curElement].Usage      = D3DDECLUSAGE_TEXCOORD;
                                decl[curElement].UsageIndex = 3;
                                curOffset += 2 * sizeof(float);
                            }
                            else
                            {
                                ignoreElement = true;
                            }
                            break;  
                        
                        case nMesh2::VertexComponent::Weights:
                            decl[curElement].Type       = D3DDECLTYPE_FLOAT4;
                            decl[curElement].Usage      = D3DDECLUSAGE_BLENDWEIGHT;
                            decl[curElement].UsageIndex = weightsUsageIndex++;
                            curOffset += 4 * sizeof(float);
                            break;

                        case nMesh2::VertexComponent::JIndices:
                            decl[curElement].Type       = D3DDECLTYPE_FLOAT4;
                            decl[curElement].Usage      = D3DDECLUSAGE_BLENDINDICES;
                            decl[curElement].UsageIndex = jindciesUsageIndex++;
                            curOffset += 4 * sizeof(float);
                            break;

                        default:
                            n_error("Unknown vertex component in vertex component mask");
                            break;
                    }
                    if (!ignoreElement)
                    {
                        curElement++;
                    }
                }
            }
        }
    }

    // write vertex declaration terminator element, see D3DDECL_END() macro in d3d9types.h for details
    decl[curElement].Stream = 0xff;
    decl[curElement].Offset = 0;
    decl[curElement].Type   = D3DDECLTYPE_UNUSED;
    decl[curElement].Method = 0;
    decl[curElement].Usage  = 0;
    decl[curElement].UsageIndex = 0;

    if (0 != this->vertexDeclaration)
    {
        this->vertexDeclaration->Release();
        this->vertexDeclaration = 0;
    }
    
    HRESULT hr = gfxServer->d3d9Device->CreateVertexDeclaration(decl, &(this->vertexDeclaration));
    n_dxtrace(hr, "CreateVertexDeclaration() failed in nD3D9MeshArray");
    n_assert(this->vertexDeclaration);
    this->vertexDeclarationDirty = false;
}
