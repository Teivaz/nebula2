#ifndef N_PERSISTSERVER_H
#define N_PERSISTSERVER_H
//------------------------------------------------------------------------------
/**
    @class nPersistServer

    Interface to the file system for serializing objects.

    See also @ref N2ScriptInterface_npersistserver

    <h2>Nebula Persistent Object File Format</h2>

    The actual format of serialized objects is not specified.
    Instead script server objects are used to translate nCmd objects
    emitted by objects into actual script statements that will
    be saved to files. However, since the file server needs to
    know which script server to use for reading serialized object
    files, there has to be some meta information in the file
    itself which names the class of the script server which 
    understands it.

    Within the first 128 Bytes of a serialized object file the 
    following string must be embedded:

    @verbatim
    $parser:classname$
    @endverbatim

    Where 'classname' is the script server's class which has
    to be used to read that file. 

    <h2>Folded and unfolded objects</h2>

    *** NOTE *** UNFOLDED OBJECTS ARE NO LONGER SUPPORTED FOR
    WRITING, AND READING IS ONLY SUPPORTED FOR "FLAT" FILE
    HIERARCHIES (i.e. a "nKernelServer::Load("bla.n") is allowed
    if bla.n is a directory, and contains a _main file).

    Serialized objects can be saved in 2 ways, folded and unfolded.
    Folded means, the object itself and all of it's children will
    be saved into a single file. Unfolded means that for each
    object, a directory will be create in the host filesystem,
    so that a saved object hierarchy will result in a
    corresponding directory hierarchy.

    (C) 2002 A.Weissflog
*/
#include "kernel/ntypes.h"
#include "kernel/nroot.h"
#include "util/nstack.h"
#include "kernel/nautoref.h"
#include "kernel/ndynautoref.h"

//------------------------------------------------------------------------------
class nScriptServer;
class nFileServer2;
class nFile;
class nPersistServer : public nRoot 
{
public:
    /// Savemodes
    enum nSaveMode 
    {
        SAVEMODE_FOLD,  
        SAVEMODE_CLONE,  
    };

    /// constructor
    nPersistServer();
    /// destructor
    virtual ~nPersistServer();
    /// set the save mode
    void SetSaveMode(nSaveMode);
    /// get the save mode
    nSaveMode GetSaveMode(void);
    /// set the save level
    void SetSaveLevel(int);
    /// get the save level
    int GetSaveLevel(void);
    /// set the script server class which should be used for saving
    bool SetSaverClass(const char* saverClass);
    /// get the script server class which is currently used for saving
    const char* GetSaverClass();
    /// begin a persistent object
    bool BeginObject(nRoot *, const char *);
    /// begin a persistent object with constructor command
    bool BeginObjectWithCmd(nRoot *, nCmd *, const char *);
    /// create a nCmd object
    nCmd *GetCmd(nRoot *, uint id);
    /// put a cmd into persistency stream
    bool PutCmd(nCmd *);
    /// put a cmd into persistency stream if savelevel permits
    bool PutCmd(int, nCmd *);
    /// finish a persistent object
    bool EndObject(void);
    /// return cloned object
    nRoot *GetClone(void);
    /// load a object from persistency stream
    nRoot *LoadObject(const char *name);

    static nKernelServer* kernelServer;

private:   
    /// internal
    bool BeginFoldedObject(nRoot *obj, nCmd *, const char *name, bool sel_only);
    /// internal
    bool BeginCloneObject(nRoot *obj, const char *name);
    /// internal
    bool EndFoldedObject(void);
    /// internal
    bool EndCloneObject(void);
    /// internal
    nRoot *LoadFoldedObject(const char *fname, const char *obj_name);
    /// internal
    char *ReadEmbeddedString(const char *fname, const char *keyword, char *buf, int buf_size);
    /// internal
    nFile * PutFoldedObjectHeader(nScriptServer *saver, const char *fname, nRoot* obj);
    /// get a valid loader script server from class name
    nScriptServer* GetLoader(const char* loaderClass);

    nAutoRef<nFileServer2> refFileServer;
    nDynAutoRef<nScriptServer> refSaver;
    nFile *file;
    nStack<nRoot*> objectStack;
    nSaveMode saveMode;     
    
    nRoot *cloneTarget;
    nRoot *origCwd;

    int saveLevel;
};
//--------------------------------------------------------------------
#endif
