#ifndef N_TCLSERVER_H
#define N_TCLSERVER_H
//--------------------------------------------------------------------
/**
    @class nTclServer
    @ingroup NebulaScriptServices

    Implements an nScriptServer that understands Tcl, extended
    by a few Nebula specific Tcl commands and the ability
    to route script cmds to Nebula C++ objects.
*/
#include "tcl.h"
#include "kernel/nkernelserver.h"
#include "kernel/nscriptserver.h"
#include "kernel/nautoref.h"

//--------------------------------------------------------------------
class nFileServer2;
class nTclServer : public nScriptServer 
{
public:
    /// constructor
    nTclServer();
    /// destructor
    virtual ~nTclServer();
    /// evaluate a string
    virtual bool Run(const char *, const char*&);
    /// invoke a script function by name, with no parameters
    virtual bool RunFunction(const char *, const char *&);
    /// evaluate a file
    virtual bool RunScript(const char *, const char*&);

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

    /// process the tcl event queue
    virtual bool Trigger();
    /// generate a prompt string
    virtual nString Prompt();

#ifndef __MICROTCL__
    /// register Nebula as a Tcl extension
    virtual void InitAsExtension(Tcl_Interp *);
#endif

private:    
    /// write a select object statement
    void WriteSelectStatement(nFile*, nRoot *, nRoot *);
    /// link to an existing Tcl interpreter (for Tcl extension stuff)
    void LinkToInterp(Tcl_Interp *, bool);
    /// unlink from a Tcl interpreter (for Tcl extension stuff)
    void UnlinkFromInterp(Tcl_Interp *, bool);
    /// generate an indentation string
    void Indent(int i, char* buf);

public:                         // note: public because C functions need to access this
    enum
    {
        MAXINDENT = 64,         // max hierarchy depth
    };
    Tcl_Interp* interp;
    bool redirectUnknown;
    int indentLevel;
    bool printError;
    bool isStandAloneTcl;
};
//--------------------------------------------------------------------
#endif
