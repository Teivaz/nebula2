#ifndef N_BINSCRIPTSERVER_H
#define N_BINSCRIPTSERVER_H
//------------------------------------------------------------------------------
/**
    Save/load objects as binary files.

    (C) 2002 RadonLabs GmbH
*/
#ifndef N_SCRIPTSERVER_H
#include "kernel/nscriptserver.h"
#endif

#ifndef N_AUTOREF_H
#include "kernel/nautoref.h"
#endif

#undef N_DEFINES
#define N_DEFINES nBinScriptServer
#include "kernel/ndefdllclass.h"

#ifdef __WIN32__
#ifdef GetObject
#undef GetObject
#endif
#endif

//------------------------------------------------------------------------------
class nString;
class nFileServer2;
class nBinScriptServer : public nScriptServer
{
public:
    /// constructor
    nBinScriptServer();
    /// destructor
    virtual ~nBinScriptServer();
    /// evaluate a file
    virtual bool RunScript(const char *filename, const char*& result);
    /// write header of a persistent object file
    virtual nFile* BeginWrite(const char* filename, nRoot* obj);
    /// begin a new object in a persistent object file
    virtual bool WriteBeginNewObject(nFile*, nRoot *, nRoot *);
    /// begin a new object with custom constructor
    virtual bool WriteBeginNewObjectCmd(nFile*, nRoot *, nRoot *, nCmd *);
    /// begin a new object with custom constructor and selection
    virtual bool WriteBeginSelObject(nFile*, nRoot *, nRoot *);
    /// write a cmd to a persistent object file
    virtual bool WriteCmd(nFile*, nCmd *);
    /// finish a persistent object
    virtual bool WriteEndObject(nFile*, nRoot *, nRoot *);
    /// finish a persistent object file
    virtual bool EndWrite(nFile*);

    /// pointer to nKernelServer
    static nKernelServer* kernelServer;

private:
    /// write a 32 int to the file
    void PutInt(nFile* file, int val);
    /// write a 16 bit int to the file
    void PutShort(nFile* file, short val);
    /// write a float to the file
    void PutFloat(nFile* file, float val);
    /// write a string to the file
    void PutString(nFile* file, const char* str);
    /// write a bool to the file
    void PutBool(nFile* file, bool b);
    /// write a code sequence to the file
    void PutCode(nFile* file, const char* str);
    /// write an object handle to the file
    void PutObject(nFile* file, nRoot* obj);

    /// verify file format and skip file header
    bool GetHeader(nFile* file);
    /// read and execute the next block from file
    bool ReadBlock(nFile* file);
    /// get a 32 int from file
    bool GetInt(nFile* file, int& val);
    /// get a 16 bit int from the file
    bool GetShort(nFile* file, short& val);
    /// get a float from the file
    bool GetFloat(nFile* file, float& val);
    /// get a string from the file
    bool GetString(nFile* file, nString& val);
    /// get a bool from the file
    bool GetBool(nFile* file, bool& val);
    /// get a code sequence from the file
    bool GetCode(nFile* file, nString& val);
    /// get an object handle from the file
    bool GetObject(nFile* file, nRoot*& val);

    /// write a select statement
    void WriteSelect(nFile* file, nRoot* obj0, nRoot* obj1, SelectMethod selMethod);
    /// read input args and fill cmd object
    bool GetInArgs(nFile* file, nCmd* cmd);
    /// get byte length of in args in file
    int GetArgLength(nCmd* cmd);

    nAutoRef<nFileServer2> refFileServer;
};
//------------------------------------------------------------------------------
#endif
