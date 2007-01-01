#ifndef N_BINSCRIPTSERVER_H
#define N_BINSCRIPTSERVER_H
//------------------------------------------------------------------------------
/**
    @class nBinScriptServer
    @ingroup NebulaScriptServices

    A script server which provides a binary 'script syntax' for saving and
    loading object files.

    (C) 2002 RadonLabs GmbH
*/
#include "kernel/nscriptserver.h"
#include "kernel/nautoref.h"

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
    virtual bool RunScript(const char *filename, nString& result);
    /// write header of a persistent object file
    virtual nFile* BeginWrite(const char* filename, nObject* obj);
    /// begin a new object in a persistent object file
    virtual bool WriteBeginNewObject(nFile*, nRoot*, nRoot*);
    /// begin a new object with custom constructor
    virtual bool WriteBeginNewObjectCmd(nFile*, nRoot*, nRoot*, nCmd*);
    /// begin a new object with custom constructor and selection
    virtual bool WriteBeginSelObject(nFile*, nRoot*, nRoot*);
    /// write a cmd to a persistent object file
    virtual bool WriteCmd(nFile*, nCmd*);
    /// finish a persistent object
    virtual bool WriteEndObject(nFile*, nRoot*, nRoot*);
    /// finish a persistent object file
    virtual bool EndWrite(nFile*);

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
    /// get an object handle from the file
    bool GetObj(nFile* file, nRoot*& val);

    /// write a select statement
    void WriteSelect(nFile* file, nRoot* obj0, nRoot* obj1, SelectMethod selMethod);
    /// read input args and fill cmd object
    bool GetInArgs(nFile* file, nCmd* cmd);
    /// get byte length of in args in file
    int GetArgLength(nCmd* cmd);

};
//------------------------------------------------------------------------------
#endif
