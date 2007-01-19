#define N_IMPLEMENTS nPythonServer
//--------------------------------------------------------------------
//  npythonrun.cc -- Command processing
//  Created by Jason Asbahr, 2001
//  Derived from ntclrun.cc by Andre Weissflog
//  Updates by Tivonenko Ivan (aka Dark Dragon), 2003
//--------------------------------------------------------------------

#include <stdlib.h>
#include <stdio.h>

#include "kernel/nfileserver2.h"
#include "kernel/nfile.h"
#include "python/npythonserver.h"

//--------------------------------------------------------------------
/**

    @return 
*/
nString
nPythonServer::Prompt()
{
    nString prompt = kernelServer->GetCwd()->GetFullName();
    prompt.Append("> ");
    return prompt;
}

//--------------------------------------------------------------------
/**
    Wraps Run() with mode

    @param cmdStr
    @param result
    @return 
*/
bool
nPythonServer::Run(const char *cmdStr, nString& result)
{
    return this->Run(cmdStr, Py_single_input, result);
}

PyObject*
NArg2PyObject(nArg *a) 
{
    switch (a->GetType())
    {
        case nArg::Void:
            return Py_None;
        case nArg::Int:
            return PyInt_FromLong(a->GetI());
        case nArg::Float:
            return PyFloat_FromDouble(a->GetF());
        case nArg::String:
            return PyString_FromString(a->GetS());
        case nArg::Bool:
            if (a->GetB())
                return PyInt_FromLong(1);
            else
                return PyInt_FromLong(0);
        case nArg::Object:
            return PyCObject_FromVoidPtr(a->GetO(), NULL);
    }
    return NULL;
}

//--------------------------------------------------------------------
/**
 
    @param c
    @return 
*/
bool
nPythonServer::RunCommand(nCmd *c) 
{
    c->Rewind();
    int len = c->GetNumInArgs();
    PyObject *module_dict = PyModule_GetDict(this->nmodule);
    PyObject *main_module_dict = PyModule_GetDict(this->main_module);
    char *funcName = const_cast<char *>(c->In()->GetS());
    PyObject *f = PyDict_GetItemString(main_module_dict, funcName);
    if (f)
    {
        PyObject *args = PyTuple_New(len-1);
        for (int i = 0; i < len-1; i++)
        {
            PyTuple_SET_ITEM(args, i, NArg2PyObject(c->In()));
        }
        if (PyObject_CallObject(f, args)) 
            return true;
    }
    else
    {
        f = PyDict_GetItemString(module_dict, funcName);
    }
    return false;  
}

//--------------------------------------------------------------------
/**
    Executes a given string as Python code

    @param cmdStr
    @param mode
    @param result
    @return 
*/
bool
nPythonServer::Run(const char *cmdStr, int mode, nString& result)
{
    PyObject *module_dict;
    PyObject *main_module_dict;
    PyObject *output = NULL;
    bool     retval = true;

    result.Clear();

    module_dict = PyModule_GetDict(this->nmodule);
    main_module_dict = PyModule_GetDict(this->main_module);

    if (strlen(cmdStr) > 0)
    {
        output = PyRun_String((char *)cmdStr, mode, main_module_dict, module_dict);
    }
    else
    {
        retval = true;
    }

    // Process output

    if (output)
    {
        if (output == Py_None)
        {
            Py_DECREF(output);
            result = "";
            retval = true;
        }
        else
        {
            PyObject* temp;
            temp = PyObject_Str(output);
            result = PyString_AsString(temp);
            Py_DECREF(temp);
            Py_DECREF(output);
            retval = true;
        }
    }
    else
    {
        // There was an exception.
        PyObject* exception = NULL;
        exception = PyErr_Occurred();
        if (exception)
        {
            PyErr_Print();
            result = "Problem running script! See the log file for details.";
        }
        retval = false;
    }

    return retval;
}

//--------------------------------------------------------------------
/**
    Executes the contents of a file as Python code
    
    @param filename
    @param result
    @return 
*/
bool
nPythonServer::RunScript(const char *filename, nString& result)
{
    result.Clear();

    // Initialize Python commands in __main__
    nPythonInitializeEnvironment();

    // Open file
    nFileServer2* file2 = nFileServer2::Instance();
    nString fname = file2->ManglePath(filename);
    nFile* file = file2->NewFileObject();

    if (file->Open(fname, "r")) 
    {
        // We only opened this to be able to report the error ourselves.
        // So, go on, close it now.
        file->Close();

        // We go through this madness of using execfile() because under
        // Windows, the FILE *fp is different between Debug and Release
        // builds.  Because of this, PyRun_SimpleFile() doesn't work
        // if you try to force a debug build of Nebula to use the Release
        // build of Python.  Instead of requiring everyone who has
        // a debug build of Nebula to build Python from source, we're using
        // this work around here.
        nString execString;
        execString.Format("execfile('%s')", fname.Get());
        int success = PyRun_SimpleString(execString.Get());
        if (success == -1) 
        {
            // interpreter exits due to an exception
            if (PyErr_Occurred())
            {
                PyErr_Print();
            }
            result = "Problem running script! See the log file for details.";
            return false;
        }
    }
    else 
    {
        file->Release();
        n_printf("Error unable to open file %s", filename);
        return false;
    }
    file->Release();
    return true;
}

//--------------------------------------------------------------------
/**
    @brief Invoke a Python function.
*/
bool
nPythonServer::RunFunction(const char *funcName, nString& result)
{
    nString cmdStr = funcName;
    cmdStr.Append("()");
    return this->Run(cmdStr.Get(), result);
}

//--------------------------------------------------------------------
/**
    Process events gathered by Python.  This function checks to see if
    a callback has been registered by the Python layer, and if so, it
    calls it.

    TODO: Add time delta calculation and pass the delta as an argument
*/
bool
nPythonServer::Trigger(void)
{
    PyObject *result = NULL;
    PyObject *delta  = NULL;

    if (PyCallable_Check(this->callback))
    {
        result = PyEval_CallObject(this->callback, delta);
        Py_XDECREF(delta);

        if (result == NULL)
        {
            // TODO: Report a traceback
            return false;
        }
        else
        {
            Py_XDECREF(result);
        }
    }

    // Let's see what the rest of the trigger system has to say
    return nScriptServer::Trigger();
}
//--------------------------------------------------------------------
//  EOF
//--------------------------------------------------------------------
