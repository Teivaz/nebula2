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
//  Prompt()
//--------------------------------------------------------------------
char *nPythonServer::Prompt(char *buf, int size)
{
    char buffer[1024];
    const char* result;
    buffer[0] = 0;

    this->Run("psel()", Py_eval_input, result);

    if (result)
      strcat(buffer, result);

    strcat(buffer, ">");
    n_strncpy2(buf, buffer, size);
    return buf;
}


//--------------------------------------------------------------------
//  Run(const char *cmd_str, const char *& result)
//  Wraps Run() with mode
//--------------------------------------------------------------------

bool nPythonServer::Run(const char *cmd_str, const char*& result)
{
  return this->Run(cmd_str, Py_single_input, result);
}

PyObject *NArg2PyObject(nArg *a) 
{
    switch (a->GetType()) {
        case nArg::Type::Void:
            return Py_None;
        case nArg::Type::Int:
            return PyInt_FromLong(a->GetI());
        case nArg::Type::Float:
            return PyFloat_FromDouble(a->GetF());
        case nArg::Type::String:
            return PyString_FromString(a->GetS());
        case nArg::Type::Bool:
            if (a->GetB())
                return PyInt_FromLong(1);
            else
                return PyInt_FromLong(0);
        case nArg::Type::Object:
            return PyCObject_FromVoidPtr(a->GetO(), NULL);
    }
    return NULL;
}

bool nPythonServer::RunCommand(nCmd *c) 
{
    c->Rewind();
    int len = c->GetNumInArgs();
    PyObject *module_dict = PyModule_GetDict(this->nmodule);
    PyObject *main_module_dict = PyModule_GetDict(this->main_module);
    char *funcName = const_cast<char *>(c->In()->GetS());
    PyObject *f = PyDict_GetItemString(main_module_dict, funcName);
    if (f) {
        PyObject *args = PyTuple_New(len-1);
        for (int i = 0; i < len-1; i++) {
            PyTuple_SET_ITEM(args, i, NArg2PyObject(c->In()));
        }
        if (PyObject_CallObject(f, args)) 
            return true;
    } else {
        f = PyDict_GetItemString(module_dict, funcName);
    }
    return false;  
}

//--------------------------------------------------------------------
//  Run(const char *cmd_str, const char *& result)
//  Executes a given string as Python code
//--------------------------------------------------------------------

bool nPythonServer::Run(const char *cmd_str, int mode, const char*& result)
{
    // buffer was to small :(
    char buffer[1<<12];
    PyObject *module_dict;
    PyObject *main_module_dict;
    PyObject *output = NULL;
    bool     retval = true;

    result = NULL;

    module_dict = PyModule_GetDict(this->nmodule);
    main_module_dict = PyModule_GetDict(this->main_module);

    if (strlen(cmd_str) > 0) {
      output = PyRun_String((char *)cmd_str, mode, main_module_dict, module_dict);
    }
    else {
      retval = true;
    }

    // Process output

    if (output) {
      if (output == Py_None) {
        Py_DECREF(output);
        strcpy(&buffer[0], "");   // TODO: More elegant solution here
        result = (const char*)&buffer[0];
        retval = true;
      }
      else {
        // Copy the string, because it will be freed when output is decref'ed.
        PyObject* temp;
        temp = PyObject_Str(output);
        strcpy(&buffer[0], PyString_AsString(temp));
        Py_DECREF(temp);
        Py_DECREF(output);
        result = (const char*)&buffer[0];
        retval = true;
      }
    }
    else {
      // There was an exception.
      PyObject* exception = NULL;
      exception = PyErr_Occurred();
      if (exception) {
        PyObject* temp;
        temp = PyObject_Str(exception);
        strcpy(&buffer[0], PyString_AsString(temp));
        result = (const char*)&buffer[0];
        Py_DECREF(temp);
        PyErr_Print();
      }
      retval = false;
    }

    return retval;
}


//--------------------------------------------------------------------
//  RunScript()
//  Executes the contents of a file as Python code
//--------------------------------------------------------------------

bool nPythonServer::RunScript(const char *fname, const char*& result)
{
    char fname_buf[N_MAXPATH];
    char buffer[1024];
    char *exec_str;
    int success = 0;
    result = NULL;

    // Initialize Python commands in __main__
    // Note, this could be done via the Python C API, but it would
    //       require many more lines.  :-)
    PyRun_SimpleString("from npython import *");

    // Open file
    nFileServer2* file2 = kernelServer->GetFileServer();
    file2->ManglePath(fname, fname_buf, sizeof(fname_buf));
    nFile* file = file2->NewFileObject();

    if (file->Open(fname_buf, "r")) {
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
      exec_str = (char*)malloc(strlen(fname_buf) + 13);
      sprintf(exec_str, "execfile('%s')", fname_buf);
      success = PyRun_SimpleString(exec_str);
      free(exec_str);

      if (success == -1) {
         // interpreter exits due to an exception
         strcpy(&buffer[0], "Problem running script!");
         if(PyErr_Occurred())
           PyErr_Print();
         result = (const char*)&buffer[0];
         return false;
      }
    }
    else {
      file->Release();
      n_printf("Error unable to open file %s", fname);
      return false;
    }
    file->Release();
    return true;
}


//--------------------------------------------------------------------
//  Trigger()
//
//  Process events gathered by Python.  This function checks to see if
//  a callback has been registered by the Python layer, and if so, it
//  calls it.
//
//  TODO: Add time delta calculation and pass the delta as an argument
//--------------------------------------------------------------------

bool nPythonServer::Trigger(void)
{
    PyObject *result = NULL;
    PyObject *delta  = NULL;

    if (PyCallable_Check(this->callback)) {
        result = PyEval_CallObject(this->callback, delta);
        Py_XDECREF(delta);

        if (result == NULL)
          // TODO: Report a traceback
          return false;
        else
          Py_XDECREF(result);

    }

    // Let's see what the rest of the trigger system has to say
    return nScriptServer::Trigger();
}
//--------------------------------------------------------------------
//  EOF
//--------------------------------------------------------------------
