#ifndef N_NPKFILESERVER_H
#define N_NPKFILESERVER_H
//------------------------------------------------------------------------------
/**
    @class nNpkFileServer
    @ingroup NebulaNPKFileManagement

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
    virtual nFile* NewFileObject();
    /// create new nNpkDirectory object
    virtual nDirectory* NewDirectoryObject();
    /// parse the given directory for npk files
    virtual int ParseDirectory(const char* dir);

    /// pointer to kernel server
    static nKernelServer* kernelServer;

private:
    /// parse the root dir (hardwired to "home:") for npk files
    bool ParseNpkFile(const char* rootPath, const char* filename);
    /// check extension of a filename
    bool CheckExtension(const char* path, const char* ext);
    /// find a toc entry by name in the included npk file wrappers
    nNpkTocEntry* FindTocEntry(const char* absPath);
    
    nList npkFiles;         // list of nNpkFileWrapper objects
};

//------------------------------------------------------------------------------
#endif
