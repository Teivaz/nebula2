//------------------------------------------------------------------------------
//	nmesharray_main.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gfx2/nmesharray.h"
#include "kernel/nkernelserver.h"

nNebulaClass(nMeshArray, "nresource");

//------------------------------------------------------------------------------
/**
*/
nMeshArray::nMeshArray()
{
    this->usages.SetFixedSize(nGfxServer2::MaxVertexStreams);
    this->refMeshes.SetFixedSize(nGfxServer2::MaxVertexStreams);
    this->filenames.SetFixedSize(nGfxServer2::MaxVertexStreams);
}

//------------------------------------------------------------------------------
/**
*/
nMeshArray::~nMeshArray()
{
    if (this->IsValid())
    {
        this->Unload();
    }

    // unloads sub meshes
    int i;
    for (i = 0; i < nGfxServer2::MaxVertexStreams; i++)
    {
        if (this->refMeshes[i].isvalid())
        {
            this->refMeshes[i]->Release();
            this->refMeshes[i].invalidate();
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
nMeshArray::LoadResource()
{
    int i;
    bool success = true;
    for (i = 0; success && i < nGfxServer2::MaxVertexStreams; i++)
    {
        if ( (!this->filenames[i].IsEmpty()) )// load meshes if filename is provided
        {
            if ((this->refMeshes[i].isvalid()) && (this->refMeshes[i]->GetFilename() != this->filenames[i] || this->refMeshes[i]->GetUsage() != this->usages[i]))
            {
                //discharge mesh previous set mesh - filename or usage is not equal
                this->refMeshes[i]->Release();
                this->refMeshes[i].invalidate();
            }
            
            if (!this->refMeshes[i].isvalid())
            {
                this->refMeshes[i] = nGfxServer2::Instance()->NewMesh(this->filenames[i].Get());    
            }
            
            this->refMeshes[i]->SetFilename(this->filenames[i]);
            this->refMeshes[i]->SetUsage(this->usages[i]);
            success &= this->refMeshes[i]->Load();
        }
    }
    this->SetValid(success);
    return success;
}

//------------------------------------------------------------------------------
/**
*/
void
nMeshArray::UnloadResource()
{
    if (this == nGfxServer2::Instance()->GetMeshArray())
    {
        nGfxServer2::Instance()->SetMeshArray(0);
    }
    this->SetValid(false);
}

//------------------------------------------------------------------------------
/**
*/
void
nMeshArray::SetMeshAt(int index, nMesh2* mesh)
{
    n_assert(mesh);
    if (this->refMeshes[index].isvalid())
    {
        this->refMeshes[index]->Release();
        this->refMeshes[index].invalidate();
    }

    mesh->AddRef();
    this->refMeshes[index] = mesh;
}
