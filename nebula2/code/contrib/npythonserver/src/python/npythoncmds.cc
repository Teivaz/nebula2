#define N_IMPLEMENTS nPythonServer
//-----------------------------------------------------------------------------
//  npythoncmds.cc --  implements Python interpreter commands
//  Created by Jason Asbahr, 2001
//  Derived from ntclserver.cc by Andre Weissflog
//
//  Dec 2001, Flexible float and "object.command" support, Andy Miller
//  Jan 2002, Object layer and remote object support, Andy Miller
//  March 2003, Updates by Tivonenko Ivan - makes objects behave like dictionaries 
//-----------------------------------------------------------------------------
#include <stdlib.h>
#include <stdio.h>
#include "python/npythonserver.h"
#include "python/npythonobject.h"
#include "python/ncmdprotopython.h"
#include "kernel/nkernelserver.h"
#include "kernel/nroot.h"
#include "kernel/nroot.h"
#include "kernel/ndebug.h"    // gives us n_assert,n_error and n_warn

#ifdef __cplusplus
extern "C" {
#endif

  extern PyObject *CreatedObjectsList_;
  extern PyObject *CreatedObjectsList_weak_refs_;
  extern PyObject *CreatedObjectsList_weakref_callback_;
  
  PyObject* GetReturnableObject(NebulaObject* pyobj, nRoot* /* nebobj */ ) {
    return (PyObject *) pyobj;
  }
  
  PyObject* CreatedObjectsList_weakref_callback(PyObject * /*self*/, PyObject *args) {
    PyObject *results = NULL;
    PyObject *weak_ref = NULL;
    if (PyArg_ParseTuple(args, "O:ref", &weak_ref)) {

      PyObject *int_ptr = PyDict_GetItem(CreatedObjectsList_weak_refs_, weak_ref);
      if (!int_ptr)
        PyErr_SetString(PyExc_Exception, "weakref callbacks error 1");
      
      PyDict_DelItem(CreatedObjectsList_, int_ptr);
      PyDict_DelItem(CreatedObjectsList_weak_refs_, weak_ref);
      
      Py_INCREF(Py_None);
      results = Py_None;
      
    } else {
      PyErr_SetString(PyExc_Exception, "weakref callbacks error");
    }
    return results;
  }
    

  NebulaObject *CreatedObjectsList_GetObject(nRoot* real_neb_obj) {
    if (!real_neb_obj) return NULL;
    n_assert(real_neb_obj);
    
    PyObject *ptr_as_int = (PyObject *)PyInt_FromLong((long)real_neb_obj);
    PyObject *weak_ref = PyDict_GetItem(CreatedObjectsList_, ptr_as_int);
    if (weak_ref == NULL) {
      Py_DECREF(ptr_as_int);
      return NULL;
    }
    
    NebulaObject *nebula_wrapper = NULL;//, *arg_out;
    if ( (nebula_wrapper = (NebulaObject *)PyObject_CallObject(weak_ref, NULL)) == NULL) {
      n_assert("error getting wrapper nebula object from weakref\n");
    }

    if (!nebula_wrapper->nebula_object->isvalid()) { // need to delete weakref from dict
      PyDict_DelItem(CreatedObjectsList_, ptr_as_int);
      PyDict_DelItem(CreatedObjectsList_weak_refs_, weak_ref);
      Py_DECREF(ptr_as_int);
      return NULL;
    }
    
    Py_DECREF(ptr_as_int);
    return nebula_wrapper;
  }
  
  int CreatedObjectsList_AddObject(NebulaObject *nebula_object, nRoot* real_neb_obj) {
    n_assert(real_neb_obj);
    n_assert(nebula_object);
    
    PyObject *weak_ref = PyWeakref_NewRef((PyObject *)nebula_object, 
      CreatedObjectsList_weakref_callback_);
    n_assert(weak_ref);
    
    PyObject *ptr_as_int = (PyObject *)PyInt_FromLong((long)real_neb_obj);
    
    int res = PyDict_SetItem(CreatedObjectsList_, ptr_as_int, weak_ref);
        res = PyDict_SetItem(CreatedObjectsList_weak_refs_, weak_ref, ptr_as_int);
    
    Py_DECREF(ptr_as_int); // no need anymore
    
    return res;
  }
  
  int CreatedObjectsList_DeleteObject(nRoot* real_neb_obj) {
    return 0;
    if (!real_neb_obj) return 0;
    n_assert(real_neb_obj);
    PyObject *ptr_as_int = (PyObject *)PyInt_FromLong((long)real_neb_obj);
    if (PyDict_GetItem(CreatedObjectsList_, ptr_as_int)) {
      int res = PyDict_DelItem(CreatedObjectsList_, ptr_as_int);
      Py_DECREF(ptr_as_int);
      return res;
    }
    return -1;
  }

//-----------------------------------------------------------------------------
/**
    Create an object by class name, and name it by object name.
*/
PyObject* pythoncmd_New( PyObject * /*self*/, PyObject *args)
{
  char *class_name;
  char *object_name;
  NebulaObject * rv = NULL;
  nRoot *o;

  // Extract two strings from the Python args object
  // Also, ":<desc>" for a descriptive error if an exception is raised.
  if(PyArg_ParseTuple(args, "ss:New", &class_name, &object_name)) {
    // this function creates the new object and returns a pointer to it..
    o = nPythonServer::kernelServer->NewNoFail(class_name, object_name);
    if (o) {
      // Create the new python object and save some "interesting" info
      // in its dictionary
      rv = NebulaObject_New(o);
    }
    else {
      PyErr_Format(PyExc_Exception,
      "NEW: Unable to create a NebulaObject or type %s",
      class_name);
      return NULL;
    }
  }
  return GetReturnableObject(rv, o);
}

//-----------------------------------------------------------------------------
/**
    Delete an object by object name.
*/
PyObject* pythoncmd_Delete( PyObject * /*self*/, PyObject *args)
{
  char *object_name;
  PyObject *results = NULL;

  // Extract a string from the Python args object
  // Also, ":<desc>" for a descriptive error if an exception is raised.
  if(PyArg_ParseTuple(args, "s:del", &object_name)) {
    nRoot *o = nPythonServer::kernelServer->Lookup(object_name);
    
    if (o) {
      // Return "success" values
      o->Release();
      
      Py_INCREF(Py_None);
      results = Py_None;
    }
    else {
      // Report failure
      PyErr_Format(PyExc_Exception, "Could not lookup object '%s'.", object_name);
    }
  }
  return results;
}

//-----------------------------------------------------------------------------
/**
    Move within the current graph path.
*/
PyObject* pythoncmd_Sel_(PyObject *args, bool set_cwd) {
  char *object_name;
  nRoot *object;
  NebulaObject * res_neb = NULL;
  
  // Extract  a string from the Python args object
  // Also, ":<desc>" for  a descriptive error if an exception is raised.
  if (PyArg_ParseTuple(args, "")) {
    nRoot *o = nPythonServer::kernelServer->GetCwd();
    res_neb = NebulaObject_New(o);
    if (res_neb == NULL) {
      PyErr_SetString(PyExc_Exception,
        "SEL: Unable to create a NebulaObject");
      // XXX: No return NULL here? - NULL returned by the last return operator in function
      
    } else
      return GetReturnableObject(res_neb, o);
  } else if (PyArg_ParseTuple(args, "s:sel", &object_name)) {
    
    PyErr_Clear();
    nRoot *o = nPythonServer::kernelServer->Lookup(object_name);
    if (o) {
      // create a new object pointing to this selection and return it...
      if (set_cwd)
        nPythonServer::kernelServer->SetCwd(o);
      res_neb = NebulaObject_New(o);
      if (res_neb == NULL) {
        PyErr_SetString(PyExc_Exception,
          "SEL: Unable to create a NebulaObject");
        return NULL;
      }
    }
    else {
      PyErr_SetString(PyExc_Exception,
        "SEL: Unable to select path (perhaps it doesn't exist)");
      return NULL;
    }
    return GetReturnableObject(res_neb, o);
  }
  else {
    // wasn't a "string" argument - assume a nebulaobject
    PyErr_Clear();
    PyObject * tempo = NULL;
    if (PyArg_ParseTuple(args, "O:sel", &tempo)) {
      object = NebulaObject_GetPointer((NebulaObject*)tempo);
      if (object) {
        nRoot *o;
        if (set_cwd)
        {
          nPythonServer::kernelServer->SetCwd(object);
          o = nPythonServer::kernelServer->GetCwd();
        }
        else
          o = object;
        
        if (o) {
          Py_XINCREF(tempo);
          return GetReturnableObject((NebulaObject*)tempo, o);
        }
        else {
          PyErr_SetString(PyExc_Exception,
            "Couldn't CWD to object(nPythonServer::kernelServer->SetCwd)");
          // XXX: No return NULL here?
        }
      }
      else {
        PyErr_SetString(PyExc_Exception,
          "Object wasn't extracted (PyCObject_AsVoidPtr)");
      }
    }  // "o:sel"
    else {
      PyErr_SetString(PyExc_Exception,
        "Couldn't extract a pointer to the object (PyArg_ParseTuple(args,O:sel..)");
    }
  }// else
  return NULL;
}

PyObject* pythoncmd_Sel( PyObject * /*self*/, PyObject *args) {
  return pythoncmd_Sel_(args, true);
}
  
PyObject* pythoncmd_Lookup( PyObject * /*self*/, PyObject *args) {
  return pythoncmd_Sel_(args, false);
}

//-----------------------------------------------------------------------------
/**
    Display the current graph path.
*/
PyObject* pythoncmd_Psel( PyObject * /*self*/, PyObject *args)
{
    PyObject *results = NULL;
    {
        // No args, but ":<desc>" for a descriptive error if an exception is raised.
        if(PyArg_ParseTuple(args, ":psel")) {
        nRoot *o = nPythonServer::kernelServer->GetCwd();
        if(o) 
        {
                // Display path acquired from GetCwd()
                //char buf[N_MAXPATH];
                //o->GetFullName(buf, sizeof(buf));
                results = PyString_FromString(o->GetFullName().Get());
            }
            else
            {
                // Report failure
                PyErr_SetString(PyExc_AttributeError, "Could not acquire current working directory.");
            }
        }
    }

    return results;
}

//-----------------------------------------------------------------------------
/**
    Returns current object
*/
PyObject* pythoncmd_GetCwdObject( PyObject * /*self*/, PyObject *args)  {
  PyObject *results = NULL;
  
  // No args, but ":<desc>" for a descriptive error if an exception is raised.
  if(PyArg_ParseTuple(args, ":GetCwd")) {
    nRoot *o = nPythonServer::kernelServer->GetCwd();
    if (o) {
      NebulaObject *res_neb = NebulaObject_New(o);
      if (res_neb == NULL) {
        PyErr_SetString(PyExc_Exception,
          "SEL: Unable to create a NebulaObject");
        return NULL;
      }
      results = GetReturnableObject(res_neb, o);
    } else {
      // Report failure
      PyErr_SetString(PyExc_AttributeError,
        "Could not acquire current working directory.");
    }
  }
  return results;
}

//-----------------------------------------------------------------------------
/**
    Display objects in the current graph path.
*/
PyObject* pythoncmd_Dir( PyObject * /*self*/, PyObject *args)
{
  PyObject *results = NULL;

  {
    // No args, but ":<desc>" for a descriptive error if exception is raised.
    if(PyArg_ParseTuple(args, ":ndir")) {
      nRoot *cwd = nPythonServer::kernelServer->GetCwd();
      if (cwd) {
        // Display contents of path acquired from GetCwd()
        nRoot *o;
        PyObject *obj;
        results = PyList_New(0);
        for (o=cwd->GetHead(); o; o=o->GetSucc()) {
          obj = PyString_FromString(o->GetName());
          PyList_Append(results, obj);
          Py_DECREF(obj);
        }
      }
      else
        // Report failure
        PyErr_SetString(PyExc_AttributeError,
                        "Could not acquire current working directory.");
    }
  }

  // else
  return results;
}

//-----------------------------------------------------------------------------
/**
    Loads an object from disk.
*/
PyObject* pythoncmd_Get( PyObject * /*self*/, PyObject *args)
{
  char *filename;
  PyObject *results = NULL;

  {
    // Extract a filename string from the Python args object
    // Also, "s:<desc>" for a descriptive error if an exception is raised.
    if(PyArg_ParseTuple(args, "s:get", &filename)) {
      nObject *o = nPythonServer::kernelServer->Load(filename);
      if (o) {
        bool isRoot = o->IsA("nroot");
        n_assert(isRoot);
        if (isRoot) {
          // Return "success" values
          Py_INCREF(Py_None);
          results = Py_None;
        }
        else {
          // Report failure
          PyErr_Format(PyExc_IOError,
                       "Could not load object [name=%s], not an nRoot.", filename);
        }
      }
      else {
        // Report failure
        PyErr_Format(PyExc_IOError,
                     "Could not load object [name=%s].", filename);
      }
    }
  }

  // else, failure case
  return results;
}

//-----------------------------------------------------------------------------
/**
*/
bool _getInArgs(nCmd *cmd, PyObject *args)
{
  long num_args;
  PyObject *temp;

  // Get the number of args defined for this command
  num_args = cmd->GetNumInArgs();
  // Does the length of the args tuple match that number?
  if (num_args == PyTuple_Size(args)) {

    // read out arguments
    int i;
    nArg *arg;
    cmd->Rewind();
    for (i=0; i<num_args; i++) {
      bool arg_ok = false;
      arg = cmd->In();
      switch(arg->GetType()) {
        case nArg::Int:
        {
          int n;
          temp = PyTuple_GetItem(args, i);

          // NOTE: I need more forgiving conversion code here.
          //       Perhaps ParseTuple()?
          if (PyInt_Check(temp)) {
            n = (int)PyInt_AsLong(temp);
            arg->SetI(n);
            arg_ok = true;
          }
        }
        break;

        case nArg::Float:
        {
          int n;
          float f;
          double d;
          temp = PyTuple_GetItem(args, i);
          if (PyFloat_Check(temp)) {
            d = PyFloat_AsDouble(temp);
            f = (float) d;
            arg->SetF(f);
            arg_ok = true;
          }
          else if (PyInt_Check(temp)) {
            n = PyInt_AsLong(temp);
            f = float(n);
            arg->SetF(f);
            arg_ok = true;
          }
        }
        break;

        case nArg::String:
        {
          temp = PyTuple_GetItem(args, i);
          if (PyString_Check(temp)) {
            const char *str = PyString_AsString(temp);
            // SetS() duplicates the string internally
            arg->SetS(str);
            arg_ok = true;
          }
        }
        break;

        case nArg::Bool:
        {
          int bi;
          temp = PyTuple_GetItem(args, i);
          if (PyInt_Check(temp)) {
            bi = (int)PyInt_AsLong(temp);
            bool b = bi ? true : false;
            arg->SetB(b);
            arg_ok = true;
          }
        }
        break;

        case nArg::Object:
        {
          nRoot *o;

          temp = PyTuple_GetItem(args, i);
          if (PyString_Check(temp)) {
            char *o_name = PyString_AsString(temp);
            o = nPythonServer::kernelServer->Lookup(o_name);
            if (o)
              arg_ok = true;
            else
              n_printf("could not lookup '%s' as object!\n", o_name);
              // o is NULL
            arg->SetO(o);
          }
          else if (NebulaObject_Check(temp)) {
            o = NebulaObject_GetPointer((NebulaObject*)temp);
            if (o)
              arg_ok = true;
            else
              n_printf("Got a NULL within a NebulaObject!\n");
            arg->SetO(o);
          }
          else if (temp == Py_None) {
            arg->SetO(NULL);
            arg_ok = true;
          }
        }
        break;

        case nArg::List:
        {
          n_printf("List values aren't acceptable in arguments.");
          arg_ok = false;
        }
        break;

        case nArg::Void:
          break;

      }
      if (!arg_ok) return false;
    }

    return true;
  }
  return false;

}

//-----------------------------------------------------------------------------
/**
*/
PyObject* _putOutSingleArg(nArg *arg)
{
  PyObject *result = NULL;

  switch (arg->GetType()) {
    case nArg::Int:
      result = PyInt_FromLong(arg->GetI());
      break;

    case nArg::Float:
      result = PyFloat_FromDouble(arg->GetF());
      break;

    case nArg::String:
      {
         const char *s = arg->GetS();
         // Hmmm...
         if (!s) {
           s = ":null:";
         }
         result = PyString_FromString((char *)s);
      }
      break;

    case nArg::Bool:
      {
         result = PyInt_FromLong(arg->GetB());
      }
      break;

    case nArg::Object:
      {
         nRoot *o = (nRoot *) arg->GetO();
         if (o) {
           result = (PyObject*)NebulaObject_New(o);
         } else {
           Py_INCREF(Py_None);
           result = Py_None;
         }
      }
      break;

    case nArg::List:
      {
         nArg *args, *a;
         int i, count;

         count = arg->GetL(args);
         result = PyTuple_New(count);
         a = args;

         for (i = 0; i < count; i++) {
           PyTuple_SetItem(result, i, _putOutSingleArg(a));
           a++;
         }
      }
      break;

    case nArg::Void:
      break;
  }
  return result;
}

//-----------------------------------------------------------------------------
/**
*/
PyObject* _putOutArgs(nCmd *cmd)
{
  nArg *arg;
  PyObject *result = NULL;

  int num_args = cmd->GetNumOutArgs();
  cmd->Rewind();

  // handle single return args (no need to create list)
  if (1 == num_args) {
    arg = cmd->Out();
    result = _putOutSingleArg(arg);
  } else {

    // more then one output arg, create a tuple
    result = PyTuple_New(num_args);

    int i;
    for (i=0; i<num_args; i++) {
      arg = cmd->Out();
      PyTuple_SetItem(result, i, _putOutSingleArg(arg));
    }
  }

  return result;
}

//-----------------------------------------------------------------------------
/**
    Sets attributes on the currently selected graph node

    Python will be invoking this command in the form of
    Set('CommandName', arg1, arg2, arg3, ....)

    This code gets the command name from the first element of the args
    tuple, forms a Nebula Cmd object from it, and passes it and the remaining
    tuple arguments to _getInArgs.
*/
PyObject* pythoncmd_Set(PyObject * /*self*/, PyObject *args)
{
  char cmd[N_MAXPATH];
  char *command_name;
  char *obj_name;
  char *dot;
 
  PyObject *commandString;
  PyObject *commandArgs = NULL;
  PyObject *results = NULL;

  {
    // Get the command name from the first element of the args tuple
    commandString = PyTuple_GetItem(args, 0);
    if (commandString && PyString_Check(commandString)) {
      // Extract the command string and as we MAY be modifing the string
      // (if it contains a 'dot).  We need to make a copy (the
      // PyString_AsString function returns a pointer to the internal
      // string structure and hence must not be modified...

      // Get string pointer from Python string object
      char * tempcmd = PyString_AsString(commandString);
      strcpy(cmd, tempcmd);
      dot = strchr(cmd,'.');

      // special case handle path components
      while (dot && ((dot[1] == '.') || (dot[1] == '/')))
        dot = strchr(++dot, '.');

      if (dot) {
        *dot = '\0';
        obj_name = cmd;
        if (obj_name == dot)
          obj_name = NULL;
        command_name = dot + 1;
      }
      else {
        obj_name = NULL;
        command_name = cmd;
      }

      if (*command_name != '\0') {
        // find object to invoke command on
        nObject *o;
        if (obj_name)
          o = nPythonServer::kernelServer->Lookup(obj_name);
        else {
          o = nScriptServer::GetCurrentTargetObject();
          if (!o)
            o = nPythonServer::kernelServer->GetCwd();
        }

        if (o) {
          // Form a Nebula Cmd object from command string
          nClass *cl = o->GetClass();
          nCmdProto *cmd_proto = (nCmdProto *) cl->FindCmdByName(command_name);

          if (cmd_proto) {
            // Invoke the command
            nCmd *ncmd = cmd_proto->NewCmd();
            n_assert(ncmd);

            // Create a new tuple that has just the arguments needed for
            // the command
            commandArgs = PyTuple_GetSlice(args, 1, PyTuple_Size(args));
 
            // Pass Cmd object and the remaining tuple arguments to _getInArgs.
            // Retrieve input args (skip the 'unknown' and cmd statement)
            if (!_getInArgs(ncmd, commandArgs)) {
              if (o->IsA("nroot")) {
                PyErr_Format(PyExc_Exception,
                             "Broken input args, object '%s' of class '%s', command '%s'",
                             ((nRoot *)o)->GetName(), 
                             o->GetClass()->GetName(), 
                             command_name);
              }
              else {
                PyErr_Format(PyExc_Exception,
                             "Broken input args, object of class '%s', command '%s'",
                             o->GetClass()->GetName(), command_name);
              }
              results = NULL;
            }
            else if (o->Dispatch(ncmd)) {
              // let object handle the command
              results = _putOutArgs(ncmd);
              // either returns a single PyObject of the appropriate return
              // type OR a tuple containing results
              // a null return is done as a null Tuple..
            }
            else {
              if (o->IsA("nroot")) {
                PyErr_Format(PyExc_Exception,
                             "Dispatch error, object '%s' of class '%s', command '%s'",
                             ((nRoot *)o)->GetName(), 
                             o->GetClass()->GetName(), 
                             command_name);
              }
              else {
                PyErr_Format(PyExc_Exception,
                             "Dispatch error, object of class '%s', command '%s'",
                             o->GetClass()->GetName(), command_name);
              }
              results = NULL;
            }
            // In any case, cleanup the cmd object
            cmd_proto->RelCmd(ncmd);
          }
          else {
            // Set exception, the object doesn't know about the command!
            if (o->IsA("nroot")) {
              PyErr_Format(PyExc_AttributeError,
                           "Unknown command, object '%s' of class '%s', command '%s'",
                           ((nRoot *)o)->GetName(), o->GetClass()->GetName(), command_name);
            }
            else {
              PyErr_Format(PyExc_AttributeError,
                           "Unknown command, object of class '%s', command '%s'",
                           o->GetClass()->GetName(), command_name);
            }
            results = NULL;
          }
        }
        else {
          // Unable to acquire current object
          // Set appropriate exception
          PyErr_SetString(PyExc_Exception, "Unable to acquire current object.");
          results = NULL;
        }
      }
      else {
        // command is NULL
        PyErr_SetString(PyExc_Exception, "No command.");
        results = NULL;
      }
    }
    else {
      // Unable to acquire command string
      // Set appropriate exception
      PyErr_SetString(PyExc_Exception, "Unable to acquire command string.");
      results = NULL;
    }
  }
  // Clean up
  Py_XDECREF(commandArgs);
  return results;
}

//-----------------------------------------------------------------------------
/**
*/
PyObject* pythoncmd_Exit( PyObject * /*self*/, PyObject *args)
{
  PyObject* results = NULL;

  {
    // No args, but set ":<desc>" for a descriptive error if an exception
    // is raised.
    if(PyArg_ParseTuple(args, ":exit")) {

      // if we are in server mode and receive an exit, will exit the
      // server mode before shutting down

      Py_INCREF(Py_None);
      results = Py_None;

      // Turn off the interactive mode and set the quit requested flag,
      //nPythonServer::Instance->is_interactive = false;
      nPythonServer::Instance->SetQuitRequested(true);
    }
  }

  return results;
}

//-----------------------------------------------------------------------------
/**
    Outputs text
    Unlike the TclServer Puts(), this function doesn't check for options
    TODO: Should I pass the string on to Python's stdout?
*/
PyObject* pythoncmd_Puts( PyObject * /*self*/, PyObject *args)
{
  PyObject *results = NULL;
  char     *outputString;

  {
     if(PyArg_ParseTuple(args, "s:exists", &outputString)) {
        nPythonServer::kernelServer->Print(outputString);
        Py_INCREF(Py_None);
        results = Py_None;
     }
   }

   return results;
}

//-----------------------------------------------------------------------------
/**
*/
PyObject* pythoncmd_Exists( PyObject * /*self*/, PyObject *args)
{
  PyObject *results = NULL;
  char     *objectName;

  {
    if(PyArg_ParseTuple(args, "s:exists", &objectName)) {
      nRoot *o = nPythonServer::kernelServer->Lookup(objectName);
      if (o)
        results = PyInt_FromLong(1L);
      else
        results = PyInt_FromLong(0L);
    }
  }

  return results;
}

//-----------------------------------------------------------------------------
/**
    Helper functions for directing output to Nebula's log system
*/
PyObject* pythoncmd_Nprint( PyObject * /*self*/, PyObject *args)
{
  PyObject *results = NULL;
  char     *text;

  if(PyArg_ParseTuple(args, "s:nprint", &text)) {
    n_printf("%s", text);
    Py_INCREF(Py_None);
    results = Py_None;
  }
  else
    // Set exception.  Obviously, this situation could pose a problem.  :)
    PyErr_SetString(PyExc_IOError, "Unable to output text!");

  return results;
}

//-----------------------------------------------------------------------------
/**
    Utility function for setting a Python method to be called every
    frame, via the Trigger() system
*/
PyObject* pythoncmd_SetTrigger( PyObject * /*self*/, PyObject *args)
{
  PyObject *results = NULL;
  PyObject *temp;

  if(PyArg_ParseTuple(args, "O:setTrigger", &temp)) {
    if (!PyCallable_Check(temp)) {
      PyErr_SetString(PyExc_TypeError, "parameter must be callable");
      return NULL;
    }
    Py_XINCREF(temp);  // Add a reference to the new callback
    Py_XDECREF(nPythonServer::Instance->callback);  // And dispose of the old
    nPythonServer::Instance->callback = temp;  // Store it

    Py_INCREF(Py_None);
    results = Py_None;
  }

  return results;
}

//-----------------------------------------------------------------------------
/**
    Implementation of 'begincmds' command for nCmdProtoPython.

    06-Sep-04   Kim, H.W.    created
*/
PyObject* pythoncmd_BeginCmds(PyObject* /*self*/, PyObject *args)
{
    char* className;
    int numArgs;
    PyObject *result = NULL;

    if(!PyArg_ParseTuple(args, "si:begincmds", &className, &numArgs))
    {
        n_message("Usage: begincmd('classname', num_args)");
        return result;
    }

    nClass* clazz = nKernelServer::Instance()->FindClass(className);
    if (clazz)
    {
        clazz->BeginScriptCmds(numArgs);
 
        Py_INCREF(Py_None);
        result = Py_None;
    }
    else
        n_error("Failed to open class %s!\n", className);

    return result;    
}

//-----------------------------------------------------------------------------
/**
    Implementation of 'addcmds' command for nCmdProtoPython.

    06-Sep-04   Kim, H.W.    created
*/
PyObject* pythoncmd_AddCmds(PyObject* /*self*/, PyObject *args)
{
    char* className;
    char* cmdName;
    PyObject *result = NULL;

    if(!PyArg_ParseTuple(args, "ss:addcmds", &className, &cmdName))
    {
        n_message("Usage: addcmd('classname', 'cmd def')");
        return result;
    }

    nCmdProtoPython* cmdProto = n_new(nCmdProtoPython(cmdName));
    nClass* clazz = nKernelServer::Instance()->FindClass(className);
    if (clazz)
    {
        clazz->AddScriptCmd((nCmdProto*)cmdProto);

        Py_INCREF(Py_None);
        result = Py_None;
    }
    else
        n_error("Failed to find class %s!\n", className);

    return result;
}

//-----------------------------------------------------------------------------
/**
    Implementation of 'endcmds' command for nCmdProtoPython.

    06-Sep-04   Kim, H.W.    created
*/
PyObject* pythoncmd_EndCmds(PyObject* /*self*/, PyObject *args)
{
    char *className;
    PyObject *result = NULL;

    if(!PyArg_ParseTuple(args, "s:endcmds", &className))
    {
        n_message("Usage: endcmd('classname')");
        return result;
    }

    nClass* clazz = nKernelServer::Instance()->FindClass(className);
    if (clazz)
    {
        clazz->EndScriptCmds();

        Py_INCREF(Py_None);
        result = Py_None;
    }
    else
        n_error("Failed to find class %s!\n", className);

    return result;
}

#ifdef __cplusplus
}
#endif
//-----------------------------------------------------------------------------
//  EOF
//-----------------------------------------------------------------------------
