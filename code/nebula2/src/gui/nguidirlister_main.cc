//------------------------------------------------------------------------------
//  nguidirlister_main.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gui/nguidirlister.h"
#include "kernel/nfileserver2.h"
#include "kernel/ndirectory.h"

nNebulaScriptClass(nGuiDirLister, "nguitextview");

//------------------------------------------------------------------------------
/**
*/
nGuiDirLister::nGuiDirLister() :
    dirPath("xxx:"),
    pattern("*"),
    ignoreSubDirs(false),
    ignoreFiles(false),
    dirty(true),
    stripExtension(false)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nGuiDirLister::~nGuiDirLister()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiDirLister::OnShow()
{
    this->UpdateContent();
    nGuiTextView::OnShow();
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiDirLister::OnFrame()
{
    if (this->dirty)
    {
        this->UpdateContent();
    }
    nGuiTextView::OnFrame();
}

//------------------------------------------------------------------------------
/**
    Update the text view with content of directory.
*/
void
nGuiDirLister::UpdateContent()
{
    nDirectory* dir = kernelServer->GetFileServer()->NewDirectoryObject();
    n_assert(dir);
    this->BeginAppend();
    if (dir->Open(this->GetDirectory()))
    {
        if (dir->SetToFirstEntry()) do
        {
            nString entryStr = dir->GetEntryName();
            nString entryName = entryStr.ExtractFileName();
            nDirectory::EntryType entryType = dir->GetEntryType();
            if ((nDirectory::DIRECTORY == entryType) && (this->ignoreSubDirs))
            {
                continue;
            }
            if ((nDirectory::FILE == entryType) && (this->ignoreFiles))
            {
                continue;
            }
            if (!n_strmatch(entryName.Get(), this->pattern.Get()))
            {
                continue;
            }
            if ((0 == strcmp(entryName.Get(), ".")) ||
                (0 == strcmp(entryName.Get(), "..")) ||
                (0 == strcmp(entryName.Get(), "CVS")) ||
                (0 == strcmp(entryName.Get(), ".svn")))
            {
                continue;
            }

            // if in strip extension mode, strip the extension from the file
            if (this->stripExtension)
            {
                entryName.StripExtension();
            }
            this->AppendLine(entryName.Get());
        }
        while (dir->SetToNextEntry());

        dir->Close();
    }
    n_delete(dir);
    this->EndAppend();
    this->SetLineOffset(0);
    this->dirty = false;
}
