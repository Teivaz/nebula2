#ifndef N_GUIFILELISTER_H
#define N_GUIFILELISTER_H
//------------------------------------------------------------------------------
/**
    A filelister widget is derived from the lister widget and displays
    the filenames of a given directory. The user can select a file or type in
    a different filename.

    (C) 2003 RadonLabs GmbH
*/
#include "gui/nguilister.h"

//------------------------------------------------------------------------------
class nGuiFileLister : public nGuiLister
{
public:
    /// constructor
    nGuiFileLister();
    /// destructor
    virtual ~nGuiFileLister();
    /// called when widget is becoming visible
    virtual void OnShow();
    /// set filesystem directory
    void SetDirectory(const char* dir);
    /// get filesystem directory
    const char* GetDirectory() const;
    /// get selected filename
    const char* GetFilename() const;

protected:
    nString dirName;
};
//------------------------------------------------------------------------------
#endif

