#ifndef N_TCLSERVER_H
#define N_TCLSERVER_H
//--------------------------------------------------------------------
/**
    @class nTclServer
    @ingroup NebulaScriptServices

    Implements an nScriptServer that understands Tcl, extended
    by a few Nebula specific Tcl commands and the ability
    to route script cmds to Nebula C++ objects.

    By default, nTclServer will be using MicroTcl, a custom
    Radon Labs interpretter for TCL with a stripped-down selection
    of Tcl version 8.4.  The removed commands include the file i/o,
    networking, background jobs, event handling, etc - these
    operations are done by calling to the appropriate portions
    of Nebula.

    MicroTCL does not need any external runtime environment files.
    It can be linked statically into the application and has a code
    size of about 160KB (vs. 660 KB @c tcl84.dll). Radon Labs derived
    this version for the Nebula Xbox port, but it should prove excellent
    for general use in Nebula 2 as well.

    Nebula will compile with MicroTcl as default, but can be configured to
    run under the classic Tcl environment as well (undef the
    @c __MICROTCL__ define in <tt>kernel/system.h</tt>).

    Note that MicroTCL takes advantage of byte-code compilation, and
    will automatically compile any defined script procedures to byte
    code the first time they are executed.  However, script outside
    of a procedure will be parsed and executed through the normal
    text fashion.  Given that bytecode-compiled script should be
    substantially faster to execute, it is advised that you arrange
    your code into procedures if your profiling suggests script
    execution as a bottleneck.
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
    virtual bool Run(const char*, nString&);
    /// invoke a script function by name, with no parameters
    virtual bool RunFunction(const char*, nString&);
    /// evaluate a file
    virtual bool RunScript(const char*, nString&);

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

    /// process the tcl event queue
    virtual bool Trigger();
    /// generate a prompt string
    virtual nString Prompt();

    /// get the pointer of the tcl-interpreter
    Tcl_Interp* GetInterp();

#ifndef __MICROTCL__
    /// register Nebula as a Tcl extension
    virtual void InitAsExtension(Tcl_Interp*);
#endif

private:
    /// write a select object statement
    void WriteSelectStatement(nFile*, nRoot*, nRoot*);
    /// link to an existing Tcl interpreter (for Tcl extension stuff)
    void LinkToInterp(Tcl_Interp*, bool);
    /// unlink from a Tcl interpreter (for Tcl extension stuff)
    void UnlinkFromInterp(Tcl_Interp*, bool);
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
