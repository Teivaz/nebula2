#ifndef N_GUIDIRLISTER_H
#define N_GUIDIRLISTER_H
//------------------------------------------------------------------------------
/**
    @class nGuiDirLister
    @ingroup Gui
    @brief A complete directory listing widget.

    (C) 2004 RadonLabs GmbH
*/
#include "gui/nguitextview.h"

//------------------------------------------------------------------------------
class nGuiDirLister : public nGuiTextView
{
public:
    /// constructor
    nGuiDirLister();
    /// destructor
    virtual ~nGuiDirLister();
    /// set the directory path
    void SetDirectory(const char* dir);
    /// get the directory path
    const char* GetDirectory() const;
    /// ignore sub directories?
    void SetIgnoreSubDirs(bool b);
    /// get ignore sub dirs flag
    bool GetIgnoreSubDirs() const;
    /// ignore files?
    void SetIgnoreFiles(bool b);
    /// get ignore files flag
    bool GetIgnoreFiles() const;
    /// set strip extension mode
    void SetStripExtension(bool b);
    /// get strip extension mode
    bool GetStripExtension() const;
    /// set optional pattern
    void SetPattern(const char* pattern);
    /// get optional pattern
    const char* GetPattern() const;
    /// called when widget is becoming visible
    virtual void OnShow();
    /// called per frame when parent widget is visible
    virtual void OnFrame();

protected:
    /// update text list from directory content
    void UpdateContent();

    nString dirPath;
    nString pattern;
    bool stripExtension;
    bool ignoreSubDirs;
    bool ignoreFiles;
    bool dirty;
};

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiDirLister::SetDirectory(const char* dir)
{
    n_assert(dir);
    this->dirPath = dir;
    this->dirty = true;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nGuiDirLister::GetDirectory() const
{
    return this->dirPath.Get();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiDirLister::SetIgnoreSubDirs(bool b)
{
    this->ignoreSubDirs = b;
    this->dirty = true;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nGuiDirLister::GetIgnoreSubDirs() const
{
    return this->ignoreSubDirs;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiDirLister::SetIgnoreFiles(bool b)
{
    this->ignoreFiles = b;
    this->dirty = true;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nGuiDirLister::GetIgnoreFiles() const
{
    return this->ignoreFiles;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiDirLister::SetPattern(const char* p)
{
    n_assert(p);
    this->pattern = p;
    this->dirty = true;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nGuiDirLister::GetPattern() const
{
    return this->pattern.Get();
}

//------------------------------------------------------------------------------
/**
    Enable disable strip extension.
*/
inline
void
nGuiDirLister::SetStripExtension(bool b)
{
    this->stripExtension = b;
    this->dirty = true;
}

//------------------------------------------------------------------------------
/**
    Get strip extension mode.
*/
inline
bool
nGuiDirLister::GetStripExtension() const
{
    return this->stripExtension;
}

//------------------------------------------------------------------------------
#endif
