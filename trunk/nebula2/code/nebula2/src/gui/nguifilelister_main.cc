//------------------------------------------------------------------------------
//  nguifilelister_main.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gui/nguifilelister.h"
#include "kernel/ndirectory.h"
#include "util/npathstring.h"

nNebulaScriptClass(nGuiFileLister, "nguilister");

//------------------------------------------------------------------------------
/**
*/
nGuiFileLister::nGuiFileLister()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nGuiFileLister::~nGuiFileLister()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiFileLister::SetDirectory(const char* dir)
{
    n_assert(dir);
    this->dirName = dir;
}

//------------------------------------------------------------------------------
/**
*/
const char*
nGuiFileLister::GetDirectory() const
{
    return this->dirName.Get();
}

//------------------------------------------------------------------------------
/**
*/
const char*
nGuiFileLister::GetFilename() const
{
    return this->GetText();
}

//------------------------------------------------------------------------------
/**
    This parses the directory.
*/
void
nGuiFileLister::OnShow()
{
    nGuiLister::OnShow();

    // parse the filesystem directory
    nFileServer2* fileServer = kernelServer->GetFileServer();
    nDirectory* dir = fileServer->NewDirectoryObject();
    if (dir->Open(this->dirName.Get()))
    {
        if (dir->SetToFirstEntry()) do
        {
            if (nDirectory::FILE == dir->GetEntryType())
            {
                nPathString curFile(dir->GetEntryName());
                this->AppendText(curFile.ExtractFileName().Get());
            }
        } while (dir->SetToNextEntry());
        dir->Close();
    }
    else
    {
        n_error("nGuiFileLister: Could not open directory '%s'!\n", this->dirName.Get());
    }
    delete dir;
}
