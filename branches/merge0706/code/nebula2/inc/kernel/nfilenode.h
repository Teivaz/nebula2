#ifndef N_FILENODE_H
#define N_FILENODE_H
//------------------------------------------------------------------------------
/**
    @class nFileNode
    @ingroup File

    An nFileNode wraps an nFile object into an nRoot subclass, and offers 
    a script interface to the file. This is useful for script languages
    which don't have file system access built in (like MicroTcl).
    nFileNode should not be used from C++, use nFile directly for this.
    
    (C) 2003 RadonLabs GmbH
*/
#include "kernel/nroot.h"

class nFileServer2;
class nFile;

//------------------------------------------------------------------------------
class nFileNode : public nRoot
{
public:
    /// constructor
    nFileNode();
    /// destructor
    virtual ~nFileNode();
    /// open a file
    bool Open(const nString& fileName, const nString& accessMode);
    /// close file
    void Close();
    /// is file open?
    bool IsOpen() const;
    /// write string to file
    bool PutS(const nString& buffer);

private:
    nFile* file;
};

//------------------------------------------------------------------------------
#endif    
    
