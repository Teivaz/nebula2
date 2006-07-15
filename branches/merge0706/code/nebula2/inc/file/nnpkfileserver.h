#ifndef N_NPKFILESERVER_H
#define N_NPKFILESERVER_H
//------------------------------------------------------------------------------
/**
    @class nNpkFileServer
    @ingroup NPKFile

    Implement reading from npk files.

    See also @ref N2ScriptInterface_nnpkfileserver

    npk file format:

    @verbatim
    # header
    block HEADER {
        uint32 'NPK0'       // magic number
        uint32 blockLen     // number of following bytes in block
        uint32 dataOffset   // byte offset of data block from beginning of file
    }

    # table of contents, sequence of blocks of 3 types:
    block DIR {
        uint32 'DIR_'               // magic number of directory block
        uint32 blockLen             // number of following bytes in block
        uint16 dirNameLength        // length of the following name
        char[] dirName              // name of directory
    }

    block FILE {
        uint32 'FILE'               // magic number of file block
        uint32 blockLen             // number of following bytes in block
        uint32 fileOffset           // start of file data inside data block
        uint32 fileLength           // length of file data in bytes
        uint16 fileNameLength       // length of the following name
        char[] fileName             // name of file
    }

    block ENDOFDIR {
        uint32 'DEND'               // magic number of end of dir block
        uint32 blockLen             // number of following bytes in block (0)
    }

    # the data block
    block DATA {
        uint32 'DATA'               // magic number of data block
        uint32 blockLen             // number of following bytes in block
        char[] data                 // the data
    }
    @endverbatim

    (C) 2002 RadonLabs GmbH
*/
#include "kernel/nfileserver2.h"

//------------------------------------------------------------------------------
class nNpkTocEntry;
class nNpkFileServer : public nFileServer2
{
    friend class nNpkFile;
    friend class nNpkDirectory;

public:
    /// constructor
    nNpkFileServer();
    /// destructor
    virtual ~nNpkFileServer();

    /// create new nNpkFile object
    virtual nFile* NewFileObject() const;
    /// create new nNpkDirectory object
    virtual nDirectory* NewDirectoryObject() const;
    /// parse one npk file
    virtual bool ParseNpkFile(const nString& path);
    /// parse the given directory for npk files
    virtual int ParseDirectory(const nString& dirName, const nString& extension);
    /// release npk file wrappers matching pattern
    virtual void ReleaseNpkFiles(const nString& pattern);

private:
    /// add one npk file to the internal list
    bool AddNpkFile(const nString& rootPath, const nString& filename);
    /// find a toc entry by name in the included npk file wrappers
    nNpkTocEntry* FindTocEntry(const nString& absPath);
    
    nList npkFiles;         // list of nNpkFileWrapper objects
};

//------------------------------------------------------------------------------
#endif
