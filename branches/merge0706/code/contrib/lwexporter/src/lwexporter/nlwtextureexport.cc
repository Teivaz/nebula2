//----------------------------------------------------------------------------
// (c) 2006    Vadim Macagon
//----------------------------------------------------------------------------
#include "lwexporter/nlwtextureexport.h"
#include "scene/nmaterialnode.h"
#include "lwexporter/nlwshaderexportsettings.h"
#include "kernel/nkernelserver.h"
#include "kernel/nfileserver2.h"

//----------------------------------------------------------------------------
/**
*/
nLWTextureExport::nLWTextureExport(nLWShaderExportSettings* shaderSettings) :
    textures(0, 0)
{
    this->textures.SetFixedSize(shaderSettings->GetNumTextures());
    for (int i = 0; i < shaderSettings->GetNumTextures(); i++)
    {
        this->textures[i].param = shaderSettings->GetTextureParamAt(i);
        this->textures[i].sourceTexture = shaderSettings->GetTextureAt(i);
    }
}

//----------------------------------------------------------------------------
/**
*/
nLWTextureExport::~nLWTextureExport()
{
    this->textures.Clear();
}

//----------------------------------------------------------------------------
/**
    @return true if all textures were exported successfully, false otherwise.
*/
bool 
nLWTextureExport::Run()
{
    bool retVal = true;
    nFileServer2* fs = nKernelServer::Instance()->GetFileServer();
    for (int i = 0; i < this->textures.Size(); i++)
    {
        // this path is relative to the images sub-dir in the Lightwave
        // content dir
        nString origSrcFile(this->textures[i].sourceTexture);
        // if the pathname starts with the textures assign we assume the
        // texture has already been exported previously and we don't need
        // to do anything further
        if (-1 == origSrcFile.FindStringIndex("textures:", 0))
        {
            // convert the relative path to an absolute one
            nString srcFile("proj:work/images/");
            srcFile += origSrcFile;
            nString destFile("textures:");
            // figure out what the category dir is and append it to the filename
            nString categoryDir(origSrcFile.ExtractToLastSlash());
            if (!categoryDir.IsEmpty())
            {
                categoryDir.StripTrailingSlash();
                destFile += categoryDir;
                destFile += "/";
            }
            destFile += srcFile.ExtractFileName();

            // copy the source texture to the export location
            if (fs->CopyFile(srcFile, destFile))
            {
                this->textures[i].exportedTexture = destFile;
            }
            else
            {
                n_printf("Failed to copy %s to %s", srcFile.Get(), destFile.Get());
                retVal = false;
            }
        }
        else
        {
            this->textures[i].exportedTexture = this->textures[i].sourceTexture;
        }
    }

    return retVal;
}

//----------------------------------------------------------------------------
/**
*/
void 
nLWTextureExport::CopyTextureParamsTo(nMaterialNode* node)
{
    for (int i = 0; i < this->textures.Size(); i++)
    {
        n_assert2(!this->textures[i].exportedTexture.IsEmpty(),
                  "Must call nLWTextureExport::Run() first!");
        node->SetTexture(this->textures[i].param, 
                         this->textures[i].exportedTexture.Get());
    }
}

//----------------------------------------------------------------------------
// EOF
//----------------------------------------------------------------------------
