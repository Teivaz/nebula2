#define N_IMPLEMENTS nPythonServer
//-----------------------------------------------------------------------------
//  npythonobject.cc --  implements Python/Nebula Objects
//  Created by Andy Miller, 2002
//  Based upon work by Jason Asbahr
//  Updates by Tivonenko Ivan (aka Dark Dragon), 2003
//
// Note that the 'delete' function deletes a Nebula object AND DOESN'T
// delete any python objects that happen to be pointing to it - the reverse
// is true for "deleting" a python object (happens automatically most of
// the time) UNLESS you call the "_SetAutoDel_" function on the python object
// before it is deleted, in which case the Nebula object will be deleted
// along with the Python one..
//
//-----------------------------------------------------------------------------
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include "python/npythonserver.h"
#include "kernel/nkernelserver.h"
#include "python/npythonobject.h"
#include "kernel/nroot.h"
#include "kernel/ndebug.h"    // gives us n_assert, n_error and n_printf
#include "signals/nsignalserver.h"

#ifdef __cplusplus
    extern "C" {
#endif


extern PyObject *CreatedObjectsList_;

PyObject* _putOutArgs(nCmd *cmd);
bool _getInArgs(nCmd *cmd, PyObject *args);

PyObject *Npy_ErrorObject;

//-----------------------------------------------------------------------------
/**
    @brief Create a new type of nebula object - that works like obj.func(..)
*/
NebulaObject*
NebulaObject_New(nRoot * nebulaObj)
{
    NebulaObject *self;
    PyObject *cObject;

    // now lookup if we created this object previously...
    if ((self = CreatedObjectsList_GetObject(nebulaObj)))
    {
        return self;
    }
    // nope, we didn't...

    self = PyObject_New(NebulaObject, &Nebula_Type);   //new reference
    if (NULL == self)
    {
        return NULL;
    }
    self->internal_dict = NULL;
    self->in_weakreflist = NULL;
    self->nebula_object = n_new(nRef<nRoot>);
    n_assert(self->nebula_object);
    //self->nebula_object->initialize(NULL);
    self->nebula_object->SetPtr(NULL);
    self->nebula_object->set(nebulaObj);

    cObject = PyCObject_FromVoidPtr((void *) nebulaObj, NULL);
    NebulaObject_SetAttr(self, "__nebobject__", cObject);
    Py_XDECREF(cObject);

    // and now save this object for later lookup
    CreatedObjectsList_AddObject(self, nebulaObj);
    return self;
}

//-----------------------------------------------------------------------------
/**
    @brief Retrieve a path to the Nebula object from within the Python object.
*/
const char *
NebulaObject_GetPath(NebulaObject * self)
{
    n_assert(NULL != self);
    if (self->internal_dict != NULL)
    {
        PyObject * path = PyDict_GetItemString(self->internal_dict,
                                                "__nebobject_path__");
        if (path != NULL && PyString_Check(path))
        {
            return PyString_AS_STRING(path);
        }
        else
        {
            n_printf("__nebobject_path__ didn't return a valid path\n");
        }
    }
    else
    {
        n_printf("internal dictionary is NULL\n");
    }

    return NULL;
}

//-----------------------------------------------------------------------------
/**
    @brief Retrieve a pointer to the Nebula object from within the Python
    Object.
*/
nRoot *
NebulaObject_GetPointer(NebulaObject * self)
{
    n_assert(NULL != self);

    if (!self->nebula_object->isvalid())
    {
        PyErr_SetString(PyExc_Exception, "Invalid pointer no nebula object!\n");
        return NULL;
    }

    n_assert(self->nebula_object->isvalid());
    return self->nebula_object->get();
}

//-----------------------------------------------------------------------------
/**
    @brief Delete the real Nebula object pointed to by the python object
*/
PyObject *
NebulaObject_Delete(NebulaObject* self, PyObject* /*args*/)
{
    PyObject * result = NULL;
    n_assert(NULL != self);

    nRoot * o = NebulaObject_GetPointer(self);
    if (o)
    {
        o->Release();
        // delete the object from the dict now !!
        PyDict_DelItemString(self->internal_dict, "__nebobject__");
        Py_INCREF(Py_None);
        result = Py_None;
    }
    else
    {
        n_printf("NebulaObject_GetPointer returned NULL\n");
    }

    return result;
}

//-----------------------------------------------------------------------------
/**
    @brief Called when the python object goes out of existance.
*/
void
NebulaObject_Dealloc(NebulaObject *self)
{
    n_assert(NULL!=self);

    if (self->in_weakreflist != NULL)
    {
        PyObject_ClearWeakRefs((PyObject *) self);
    }

    // check for the existance of "deletenebula" in the dictionary
    if (self->internal_dict != NULL)
    {
        PyObject *v = PyDict_GetItemString(self->internal_dict,
                                           "__deletenebula__");
        if (v != NULL)
        {
            NebulaObject_Delete(self, NULL);
        }
        Py_XDECREF(self->internal_dict);
    }
    else
    {
        n_printf("Internal Dictionary is NULL\n");
    }

    if (self->nebula_object)
    {
        n_delete(self->nebula_object);
    }

    PyObject_Del(self);
}

//-----------------------------------------------------------------------------
/**
    @brief Call this function to flag the python object to delete the
    nebula object as well.
*/
PyObject *
NebulaObject_SetAutoDel(NebulaObject *self, PyObject * /*args*/)
{
    PyObject * result = NULL;
    n_assert(NULL!=self);
    // Set an entry in the dictionary as a flag to delete the Nebula object
    // when the python object goes out of scope (gets deleted)
    if (self->internal_dict != NULL)
    {
        PyObject *obj = PyInt_FromLong(1l);  // this could be anything
        NebulaObject_SetAttr(self, "__deletenebula__", obj);
        Py_XDECREF(obj);
        Py_INCREF(Py_None);
        result = Py_None;
    }
    else
    {
        n_printf("Internal Dictionary is NULL\n");
    }

    return result;
}

//-----------------------------------------------------------------------------
/**
    @brief Emit a signal from the emitter object
*/
PyObject *
NebulaObject_Emit(NebulaObject *self, PyObject * args)
{
    PyObject * result = NULL;
    n_assert(NULL!=self);
    char * usageMsg = "Usage: object.emit(signalname,arg1,arg2,...)";

    // check args python object is a tuple
    if (0 == PyTuple_Check(args))
    {
        n_message(usageMsg);
        return result;
    }

    // get the emitter object
    nObject * o = NebulaObject_GetPointer((NebulaObject*)self);
    if (0 == o)
    {
        PyErr_SetString(PyExc_Exception, "Emitter object not valid");
        return result;
    }

    // get the class of the object
    nClass * objClass = o->GetClass();
    n_assert(objClass);

    // get first item of the tuple (the signal name / fourcc)
    PyObject * arg0 = PyTuple_GET_ITEM(args, 0);
    n_assert(arg0);
    nSignal * signal = 0;
    if (PyString_Check(arg0))
    {
        char * signalName = PyString_AsString(arg0);
        signal = objClass->FindSignalByName(signalName);
        if (0 == signal && strlen(signalName) == 4)
        {
            nFourCC fourcc = MAKE_FOURCC(signalName[0], signalName[1], signalName[2], signalName[3]);
            signal = objClass->FindSignalById(fourcc);
        }
    }
    if (0 == signal)
    {
        PyErr_SetString(PyExc_Exception, "Signal name not valid");
        return result;
    }

    // get new nCmd for the signal information
    nCmd *cmd = signal->NewCmd();
    n_assert(cmd);

    // Create a new tuple that has just the arguments needed for the command
    PyObject * commandArgs = PyTuple_GetSlice(args, 1, PyTuple_Size(args));

    // Pass Cmd object and the remaining tuple arguments to _getInArgs.
    // Retrieve input args (skip the 'unknown' and cmd statement)
    if (!_getInArgs(cmd, commandArgs))
    {
        if (o->IsA("nroot"))
        {
            PyErr_Format(PyExc_Exception,
                    "Broken input args, object '%s' of class '%s', signal '%s'",
                    ((nRoot *)o)->GetName(),
                    o->GetClass()->GetName(),
                    signal->GetName());
        }
        else
        {
            PyErr_Format(PyExc_Exception,
                    "Broken input args, object of class '%s', signal '%s'",
                    o->GetClass()->GetName(), signal->GetName());
        }
        result = NULL;
    }
    else if (o->Dispatch(cmd))
    {
        // let object handle the command
        result = _putOutArgs(cmd);
        // either returns a single PyObject of the appropriate
        // return type OR a tuple containing results
        // a null return is done as a null Tuple..
    }
    else
    {
        if (o->IsA("nroot"))
        {
            PyErr_Format(PyExc_Exception,
                "Dispatch error, object '%s' of class '%s', signal '%s'",
                ((nRoot *)o)->GetName(),
                o->GetClass()->GetName(),
                signal->GetName());
        }
        else
        {
            PyErr_Format(PyExc_Exception,
                "Dispatch error, object of class '%s', signal '%s'",
                o->GetClass()->GetName(), signal->GetName());
        }
        result = NULL;
    }
    // In any case, cleanup the cmd object
    signal->RelCmd(cmd);

    // Clean up
    Py_XDECREF(commandArgs);
    return result;
}

//-----------------------------------------------------------------------------
/**
    @brief Post a signal/command from the emitter object
*/
PyObject *
NebulaObject_Post(NebulaObject *self, PyObject * args)
{
    PyObject * result = NULL;
    n_assert(NULL!=self);
    char * usageMsg = "Usage: object.post(reltime,cmdSignalName,arg1,arg2,...)";

    // check args python object is a tuple
    if (0 == PyTuple_Check(args))
    {
        n_message(usageMsg);
        return result;
    }

    // get the emitter object
    nObject * o = NebulaObject_GetPointer((NebulaObject*)self);
    if (0 == o)
    {
        PyErr_SetString(PyExc_Exception, "Object not valid");
        return result;
    }

    // get the class of the object
    nClass * objClass = o->GetClass();
    n_assert(objClass);

    // get fist item of the tuple (the relative time)
    nTime relT = 0;
    PyObject * arg0 = PyTuple_GET_ITEM(args, 0);
    n_assert(arg0);
    if (PyFloat_Check(arg0))
    {
        relT = PyFloat_AsDouble(arg0);
    }
    else if (PyInt_Check(arg0))
    {
        relT = (double) PyInt_AsLong(arg0);
    }
    else
    {
        PyErr_SetString(PyExc_Exception, "Relative time not valid");
        return result;
    }

    // get second item of the tuple (the signal name / fourcc)
    PyObject * arg1 = PyTuple_GET_ITEM(args, 1);
    n_assert(arg1);
    nCmdProto * cmdsig = 0;
    if (PyString_Check(arg1))
    {
        char * cmdSignalName = PyString_AsString(arg1);
        cmdsig = objClass->FindSignalByName(cmdSignalName);
        if (0 == cmdsig && strlen(cmdSignalName) == 4)
        {
            nFourCC fourcc = MAKE_FOURCC(cmdSignalName[0], cmdSignalName[1], cmdSignalName[2], cmdSignalName[3]);
            cmdsig = objClass->FindSignalById(fourcc);
        }
        if (0 == cmdsig)
        {
            cmdsig = objClass->FindCmdByName(cmdSignalName);
        }
    }
    if (0 == cmdsig)
    {
        PyErr_SetString(PyExc_Exception, "Signal/Command name not valid");
        return result;
    }

    // get new nCmd for the signal/command
    nCmd *cmd = cmdsig->NewCmd();
    n_assert(cmd);

    // Create a new tuple that has just the arguments needed for the command
    PyObject * commandArgs = PyTuple_GetSlice(args, 2, PyTuple_Size(args));

    // Pass Cmd object and the remaining tuple arguments to _getInArgs.
    // Retrieve input args (skip the 'unknown' and cmd statement)
    if (!_getInArgs(cmd, commandArgs))
    {
        if (o->IsA("nroot"))
        {
            PyErr_Format(PyExc_Exception,
                    "Broken input args, object '%s' of class '%s', signal/command '%s'",
                    ((nRoot *)o)->GetName(),
                    o->GetClass()->GetName(),
                    cmdsig->GetName());
        }
        else
        {
            PyErr_Format(PyExc_Exception,
                    "Broken input args, object of class '%s', signal/command '%s'",
                    o->GetClass()->GetName(), cmdsig->GetName());
        }
        result = NULL;
    }
    else
    {
        // let object handle the command
        nSignalServer * signalServer = nSignalServer::Instance();
        if (0 == signalServer)
        {
            PyErr_Format(PyExc_Exception,"Signal server not available");
            result = NULL;
        }
        else if (signalServer->PostCmd(relT, o, cmd))
        {
            Py_INCREF(Py_None);
            result = Py_None;
        }
        else
        {
            PyErr_Format(PyExc_Exception,"Signal server post failed");
            result = NULL;
        }
    }

    if (result == NULL)
    {
        cmdsig->RelCmd(cmd);
    }

    // Clean up
    Py_XDECREF(commandArgs);
    return result;
}


// only one method defined...
PyMethodDef Nebula_methods[] = {
    {"_SetAutoDel_", (PyCFunction)NebulaObject_SetAutoDel, METH_VARARGS, NULL},
    {"emit",         (PyCFunction)NebulaObject_Emit,       METH_VARARGS, NULL},
    {"post",         (PyCFunction)NebulaObject_Post,       METH_VARARGS, NULL},
    {NULL,      NULL, 0, NULL}    /* sentinel */
};

//-----------------------------------------------------------------------------
/**
    @brief Handle a call to the object.

    If functionname =! NULL then we know we are really inside an OPCODE loop
    in the interpreter and need to call into Nebula. Makes the object
    stateful for one internal interpreter cycle.
*/
PyObject*
NebulaObject_Call(NebulaObject *self,
                            PyObject *args,
                            PyObject * /*kw*/)
{
    PyObject * result;
    n_assert(NULL != self);
    // This should have been set previously by getattro call:
    if (self->functionname != NULL)
    {
        int res;
        res = NebulaObject_CallPObjectFunction(self,
                                               PyString_AsString(self->functionname),
                                               args, &result);
        if (res == 0)
        {
            Py_XDECREF(self->functionname);
            self->functionname = NULL;
            return result;
        }
        else
        {
            n_printf("CallPObjectFunction failed in Nebula_Call");
        }
        Py_DECREF(self->functionname);
        self->functionname = NULL;
    }
    else
    {
        n_error("functionname is NULL (should have been set previously)\n");
    }

    return NULL;
}

//-----------------------------------------------------------------------------
/**
    @brief Trap the function call request (or an attribute call for
    that matter).

    Set the internal functionname so we know what function is really
    wanted and return a pointer to ourselves.

    When executing object.function(..) the Python interpreter first
    does a GETATTR call (handled by the objects tp_getattro func) on
    the object using the 'function' as it's key - in the case of a
    function call it expects that the return from the GETATTR is a
    callable object which the interpreter then calls on it's next
    OPCODE (handled by the tp_call func) - hence we need to store
    the function  name in the object and pass a pointer back.
*/
PyObject*
NebulaObject_GetAttrO(NebulaObject *self, PyObject *name)
{
    n_assert(NULL != self);
    n_assert(NULL != name);
    self->functionname = name;
    Py_INCREF(self);  // needed as we are using ourselves for the function call
    Py_INCREF(self->functionname);   // need to do this one..
    return (PyObject *)self;
}

//-----------------------------------------------------------------------------
/**
    @brief Call the actual Nebula function.
*/
int
NebulaObject_CallNebulaFunction(nRoot* o, char * name,
                                PyObject * commandArgs,
                                PyObject **returndata)
{
    int result = 0;
    if (o)
    {
        nClass *cl = o->GetClass();
        nCmdProto *cmdProto = (nCmdProto *) cl->FindCmdByName(name);
        if (cmdProto)
        {
            // Invoke the command
            nCmd *cmd = cmdProto->NewCmd();
            n_assert(cmd);
            // Create a new tuple that has just the arguments needed for the command
            // Passes Cmd object and the remaining tuple arguments to _getInArgs.
            // retrieve input args (skip the 'unknown' and cmd statement)
            if (!_getInArgs(cmd, commandArgs))
            {
                PyErr_Format(PyExc_Exception,
                            "Broken input args, object '%s', command '%s'",
                            o->GetName(), name);
                result = -1;
            }
            else if (o->Dispatch(cmd))
            {
                // let object handle the command
                if (returndata)
                {   // the caller wants return data
                    *returndata = _putOutArgs(cmd);   // get the return data
                }
                result = 0;  // set successful return
            }
            else
            {
                PyErr_Format(PyExc_Exception,
                            "Dispatch error, object '%s', command '%s'",
                            o->GetName(), name);
                result = -1;
            }
            // In any case, cleanup the cmd object
            cmdProto->RelCmd(cmd);
        }
        else
        {
            // Set exception, the object doesn't know about the command!
            PyErr_Format(PyExc_AttributeError,
                         "Unknown command, object '%s', command '%s'",
                         o->GetName(), name);
            result = -1;
        }
    }
    else
    {
        // Unable to acquire current object
        // Set appropriate exception
        PyErr_SetString(Npy_ErrorObject,
                        "Unable to get a pointer to the Nebula object - probably Nebula object is deleted");
        result = -1;
    }
    return result;
}

//-----------------------------------------------------------------------------
/**
    @brief The main function to make a 'function' call on an object - used
    by 'Set', etc.
*/
int
NebulaObject_CallPObjectFunction(NebulaObject *self, char *name,
                                 PyObject * args,
                                 PyObject ** returndata)
{
    n_assert(NULL != self);
    n_assert(NULL != name);

    PyObject * commandArgs;
    int result = -1;

    /* First lets go through and check if the 'function' is really an
       attribute or internal method */

    // first we look for the attribute in the internal dictionary
    if (self->internal_dict != NULL)
    {
        // If we've already saved this somewhere along the way...
        PyObject *v = PyDict_GetItemString(self->internal_dict, name);
        if (v != NULL)
        {
            *returndata = PyTuple_New(1);
            Py_INCREF(v); // need to keep access to temp for a while
            PyTuple_SetItem(*returndata, 0, v);
            Py_DECREF(v);
            // need to increment attribute reference as GetItemString just
            // borrows the reference
            return 0;
        }
    }
    else
    {
        PyErr_SetString(PyExc_Exception,
                        ("Problem: internal dictionary is NULL"));
        n_printf("Internal dictionary is NULL");
        return -1; // we should already have a dictionary
    }

    // not in dictionary so call method as defined in methods listed above...
    PyObject *v = Py_FindMethod(Nebula_methods, (PyObject *)self, name);
    //returns new reference
    if (v != NULL)
    {
        *returndata = PyObject_CallObject(v, args);   //lets call it
        Py_DECREF(v);
        return 0;
    }
    PyErr_Clear();

    // finally we assume it's a Nebula attribute and continue

    // lets lookup the attribute to see if we have already saved a pointer to it
    if (NULL == args)
    { // just a function call so create a new empty tuple
        commandArgs = PyTuple_New(0);
    }
    else
    {
        if (!PyTuple_Check(args))
        {
            // assume a single argument
            commandArgs = PyTuple_New(1);
            Py_INCREF(args);   // need to keep access to args for a while
            PyTuple_SetItem(commandArgs, 0, args);   // note a stolen reference to args
        }
        else
        {
            // finally args is already a tuple
            commandArgs = PyTuple_GetSlice(args, 0, PyTuple_Size(args)); // the entire tuple NR
        }
    }

    // retrieve the stored pointer to the nebula object
    nRoot *o = NebulaObject_GetPointer(self);
    result = NebulaObject_CallNebulaFunction(o, name, commandArgs, returndata);

    Py_XDECREF(commandArgs);
    return result;
}

//-----------------------------------------------------------------------------
/**
    @brief Internal setattribute function to REALLY set the Python attribute

    Needed as the "default" setattr calls the nebula object.
*/
int
NebulaObject_SetAttr(NebulaObject *self, char *name, PyObject *v)
{
    n_assert(NULL != self);
    n_assert(NULL != name);

    if (NULL == self->internal_dict)
    {
        self->internal_dict = PyDict_New();
        if (NULL == self->internal_dict)
        {
            n_error("Couldn't create a dictionary\n");
        }
    }
    if (NULL == v)
    {
        int rv = PyDict_DelItemString(self->internal_dict, name);
        if (rv < 0)
        {
            PyErr_SetString(PyExc_AttributeError,
                            "Tried to delete a nonexistent NebulaObject attribute");
        }
        return rv;
    }
    else
    {
        return PyDict_SetItemString(self->internal_dict, name, v);
    }
}

static int
NebulaObject_Dict_Length(NebulaObject *mp)
{
    n_assert(mp->internal_dict != NULL);
    nRoot * o = NebulaObject_GetPointer(mp);
    if (o)
    {
        nRoot *self = (nRoot *) o;

        int num_children = 0;

        nRoot *child;
        for (child = (nRoot *) self->GetHead(); child; child = (nRoot *) child->GetSucc())
        {
            num_children++;
        }
        return num_children;
    }
    else
    {
        n_printf("NebulaObject_GetPointer returned NULL\n");
        return 0;
    }
    return 0;
}

static PyObject*
NebulaObject_Dict_Subscript(NebulaObject *mp, register PyObject *key)
{
    n_assert(mp->internal_dict != NULL);
    n_assert(key);
    if (!PyString_Check(key))
    {
        PyErr_Format(PyExc_KeyError, "key must be string type and not %s", key->ob_type->tp_name);
        return NULL;
    }

    char * keystr = PyString_AS_STRING(key);
    nRoot * o = NebulaObject_GetPointer(mp);
    if (o)
    {
        nRoot *self;
        if ((self = o->Find(keystr)))
        {
            // TODO maybe check if we created??
            return (PyObject *)NebulaObject_New(self);
        }
        return PyErr_Format(PyExc_KeyError, "key error ('%s' not found)", keystr);
    }
    else
    {
        n_printf("NebulaObject_GetPointer returned NULL\n");
        return 0;
    }
}

staticforward PyObject* NebulaObjectIter_New(NebulaObject *nebulaObject);

static PyMappingMethods nebula_as_mapping = {
    (inquiry)NebulaObject_Dict_Length,              /*mp_length*/
    (binaryfunc)NebulaObject_Dict_Subscript,        /*mp_subscript*/
    (objobjargproc)0,                               /*mp_ass_subscript*/
};


static char nebula_object_doc[] =
"python wrapper for nebula object\n";

// now to actually define the new python type to represent a Nebula Object

PyTypeObject Nebula_Type = {
    /* The ob_type field must be initialized in the module init function
     * to be portable to Windows without using C++. */
    PyObject_HEAD_INIT(NULL)
    0,                                              /*ob_size*/
    "Nebula",                                       /*tp_name*/
    sizeof(NebulaObject),                           /*tp_basicsize*/
    0,                                              /*tp_itemsize*/
    /* methods */
    (destructor)NebulaObject_Dealloc,               /*tp_dealloc*/
    0,                                              /*tp_print*/
    0,                                              /*tp_getattr*/
    0,                                              /*tp_setattr*/
    0,                                              /*tp_compare*/
    0,                                              /*tp_repr*/
    0,                                              /*tp_as_number*/
    0,                                              /*tp_as_sequence*/
    &nebula_as_mapping,                             /*tp_as_mapping*/
    0,                                              /*tp_hash*/
    (ternaryfunc)NebulaObject_Call,                 /*tp_call*/
    0,                                              /*tp_str*/
    (getattrofunc)NebulaObject_GetAttrO,            /*tp_getattro*/
    0,                                              /*tp_setattro*/
    0,                                              /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_HAVE_WEAKREFS,  /*tp_flags*/
    nebula_object_doc,                              /*tp_doc*/
    0,                                              /*tp_traverse*/
    0,                                              /*tp_clear*/
    0,                                              /*tp_richcompare*/
    offsetof(NebulaObject, in_weakreflist),         /*tp_weaklistoffset*/
    (getiterfunc)NebulaObjectIter_New,              /*tp_iter*/
    0,                                              /*tp_iternext*/
    0,                                              /*tp_methods*/
    0,                                              /*tp_members*/
    0,                                              /*tp_getset*/
    0,                                              /*tp_base*/
    0,                                              /*tp_dict*/
    0,                                              /*tp_descr_get*/
    0,                                              /*tp_descr_set*/
    0,                                              /*tp_dictoffset*/
    0,                                              /*tp_init*/
    0,                                              /*tp_alloc*/
    0,                                              /*tp_new*/
    0,                                              /*tp_free*/
    0,                                              /*tp_is_gc*/
};

/* NebulaObjectIter - iterate over the children of a NebulaObject */

extern PyTypeObject PyNebIter_Type; /* Forward */

typedef struct {
    PyObject_HEAD
    NebulaObject *ni_nebulaobject;
    int currentPosition;
    nRoot *currentObject;
} NebulaObjectIter;

static PyObject*
NebulaObjectIter_New(NebulaObject *nebulaObject)
{
    NebulaObjectIter *iter = PyObject_NEW(NebulaObjectIter, &PyNebIter_Type);
    if (NULL == iter)
    {
        return NULL;
    }
    Py_INCREF(nebulaObject);
    iter->ni_nebulaobject = nebulaObject;
    iter->currentPosition = 0;

    iter->currentObject = NULL;
    nRoot *temp = NebulaObject_GetPointer(nebulaObject);
    if (NULL == temp)
    {
        PyErr_SetString(PyExc_Exception, "pointer to nebula object is null!");
        return NULL;
    }
    iter->currentObject = temp->GetHead();
    return (PyObject *)iter;
}

static void
NebulaObjectIter_Dealloc(NebulaObjectIter* iter)
{
    Py_DECREF(iter->ni_nebulaobject);
    PyObject_DEL(iter);
}

static PyObject*
NebulaObjectIter_Next(NebulaObjectIter* iter, PyObject* /*args*/)
{
    if (NULL == iter->currentObject)
    {
        PyErr_SetObject(PyExc_StopIteration, Py_None);
        return NULL;
    }
    else
    {
        if (iter->currentPosition == 0)
        {
            iter->currentPosition++;
            return (PyObject *)NebulaObject_New(iter->currentObject);
        }
        iter->currentObject = iter->currentObject->GetSucc();
        if (NULL == iter->currentObject)
        {
            PyErr_SetObject(PyExc_StopIteration, Py_None);
            return NULL;
        }
        iter->currentPosition++;
        return (PyObject *)NebulaObject_New(iter->currentObject);
    }
    PyErr_SetObject(PyExc_StopIteration, Py_None);
    return NULL;
}

static PyObject*
NebulaObjectIter_GetIter(PyObject *it) {
    Py_INCREF(it);
    return it;
}

static PyMethodDef NebulaObjectIter_Methods[] = {
  {"next",  (PyCFunction)NebulaObjectIter_Next, METH_VARARGS,
   "it.next() -- get the next value, or raise StopIteration"},
  {NULL,    NULL}   /* sentinel */
};

static PyObject*
NebulaObjectIter_IterNext(NebulaObjectIter *iter)
{
    if (NULL == iter->currentObject)
    {
        PyErr_SetObject(PyExc_StopIteration, Py_None);
        return NULL;
    }
    else
    {
        if (iter->currentPosition == 0)
        {
            iter->currentPosition++;
            return (PyObject *)NebulaObject_New(iter->currentObject);
        }
        iter->currentObject = iter->currentObject->GetSucc();
        if (NULL == iter->currentObject)
        {
            PyErr_SetObject(PyExc_StopIteration, Py_None);
            return NULL;
        }
        iter->currentPosition++;
        return (PyObject *)NebulaObject_New(iter->currentObject);
    }
    PyErr_SetObject(PyExc_StopIteration, Py_None);
    return NULL;
}


// Nebula Iterator type
PyTypeObject PyNebIter_Type = {
    PyObject_HEAD_INIT(&PyType_Type)
    0,                                          /* ob_size */
    "nebula-iterator",                          /* tp_name */
    sizeof(NebulaObjectIter),                   /* tp_basicsize */
    0,                                          /* tp_itemsize */
    /* methods */
    (destructor)NebulaObjectIter_Dealloc,       /* tp_dealloc */
    0,                                          /* tp_print */
    0,                                          /* tp_getattr */
    0,                                          /* tp_setattr */
    0,                                          /* tp_compare */
    0,                                          /* tp_repr */
    0,                                          /* tp_as_number */
    0,                                          /* tp_as_sequence */
    0,                                          /* tp_as_mapping */
    0,                                          /* tp_hash */
    0,                                          /* tp_call */
    0,                                          /* tp_str */
    PyObject_GenericGetAttr,                    /* tp_getattro */
    0,                                          /* tp_setattro */
    0,                                          /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT,                         /* tp_flags */
    0,                                          /* tp_doc */
    0,                                          /* tp_traverse */
    0,                                          /* tp_clear */
    0,                                          /* tp_richcompare */
    0,                                          /* tp_weaklistoffset */
    (getiterfunc)NebulaObjectIter_GetIter,      /* tp_iter */
    (iternextfunc)NebulaObjectIter_IterNext,    /* tp_iternext */
    NebulaObjectIter_Methods,                   /* tp_methods */
    0,                                          /* tp_members */
    0,                                          /* tp_getset */
    0,                                          /* tp_base */
    0,                                          /* tp_dict */
    0,                                          /* tp_descr_get */
    0,                                          /* tp_descr_set */
};

#ifdef __cplusplus
}
#endif

//--------------------------------------------------------------------
//  EOF
//--------------------------------------------------------------------
