#ifndef N_PYTHONSERVER_H
#define N_PYTHONSERVER_H
//--------------------------------------------------------------------
/**
    @class nPythonServer
    @ingroup PythonScriptServices
    @brief wraps around Python interpreter

    Implements an nScriptServer that understands Python, extended
    by a few Nebula specific Python commands and the ability
    to route script cmds to Nebula C++ objects.
*/
//--------------------------------------------------------------------
#ifdef _DEBUG
#undef _DEBUG
#include <Python.h>
#define _DEBUG 1
#else
#include <Python.h>
#endif
#include <errcode.h>

#include "kernel/nroot.h"
#include "kernel/nkernelserver.h"
#include "kernel/nscriptserver.h"
#include "kernel/nautoref.h"
#include "util/nstring.h"

// Package registration hook
void nPythonRegisterPackages(nKernelServer *);
// Module name hook
const char * nPythonModuleName();
// Environment initialization hook
void nPythonInitializeEnvironment();

/**
    Initialization hook that takes the module name.  This must be
    called by the embedder/extender in their module initialization
    function.
*/
void nPythonInitialize(const char *);

//--------------------------------------------------------------------
class nFileServer2;
class nPythonServer : public nScriptServer 
{
public:
    static nPythonServer *Instance;

public:
    nPythonServer();
    virtual ~nPythonServer();
 
    virtual bool Run(const char *, nString&);
    virtual bool Run(const char *, int mode, nString&);
    virtual bool RunScript(const char *, nString&);
    virtual bool RunFunction(const char *, nString&);
    virtual bool RunCommand(nCmd *);
    virtual nString Prompt();
 
    virtual nFile* BeginWrite(const char* filename, nObject* obj);  
    virtual bool WriteComment(nFile *, const char *);
    virtual bool WriteBeginNewObject(nFile *, nRoot *, nRoot *);
    virtual bool WriteBeginNewObjectCmd(nFile *, nRoot *, nRoot *, nCmd *);
    virtual bool WriteBeginSelObject(nFile *, nRoot *, nRoot *);
    virtual bool WriteCmd(nFile *, nCmd *);
    virtual bool WriteEndObject(nFile *, nRoot *, nRoot *);
    virtual bool EndWrite(nFile *);
 
    virtual bool Trigger(void);

    bool is_standalone_python;
    PyObject *callback;   // Callback for Trigger() function

private:
    void write_select_statement(nFile *, nRoot *, nRoot *);

    long indent_level;
    char indent_buf[N_MAXPATH];
    bool print_error;
    PyObject *nmodule;
    PyObject *main_module;
};
//--------------------------------------------------------------------
#endif
