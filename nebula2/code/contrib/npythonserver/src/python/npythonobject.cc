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
#include "kernel/ndebug.h"    // gives us n_assert,n_error and n_printf

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
NebulaObject* NebulaObject_New(nRoot * nebulaObj)
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
    if (self == NULL)
        return NULL;
    self->internal_dict = NULL;
    self->in_weakreflist = NULL; 
    self->nebula_object = n_new nRef<nRoot>;
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
const char * NebulaObject_GetPath(NebulaObject * self)
{
    n_assert(NULL != self);
    if (self->internal_dict != NULL)
    {
        PyObject * path = PyDict_GetItemString(self->internal_dict,
                                                "__nebobject_path__");
        if ( path != NULL && PyString_Check(path) )
            return ( PyString_AS_STRING(path) );
        else
            n_printf("__nebobject_path__ didn't return a valid path\n");
    }
    else
        n_printf("internal dictionary is NULL\n");

    return NULL;
}

//-----------------------------------------------------------------------------
/**
    @brief Retrieve a pointer to the Nebula object from within the Python
    Object.
*/
nRoot * NebulaObject_GetPointer( NebulaObject * self )
{
    n_assert(NULL != self);

    if (!self->nebula_object->isvalid())
    {
        PyErr_SetString(PyExc_Exception, "Invalid pointer no nebula object!\n");
        return NULL;
    }
    
    n_assert(self->nebula_object->isvalid());
    return self->nebula_object->get();
  
    return NULL;
}

//-----------------------------------------------------------------------------
/**
    @brief Delete the real Nebula object pointed to by the python object
*/
PyObject * NebulaObject_Delete(NebulaObject *self, PyObject *args)
{
    PyObject * results = NULL;
    n_assert(NULL != self);

    nRoot * o = NebulaObject_GetPointer(self);
    if (o)
    {
        o->Release();
        // delete the object from the dict now !!
        PyDict_DelItemString(self->internal_dict, "__nebobject__");
        Py_INCREF(Py_None);
        results =   Py_None;
    }
    else
        n_printf("NebulaObject_GetPointer returned NULL\n");
 
    return results;
}

//-----------------------------------------------------------------------------
/**
    @brief Called when the python object goes out of existance.
*/
void NebulaObject_Dealloc(NebulaObject *self)
{
    n_assert(NULL!=self);

    if (self->in_weakreflist != NULL)
        PyObject_ClearWeakRefs((PyObject *) self);

    nRoot *neb_object = NebulaObject_GetPointer(self);

    // check for the existance of "deletenebula" in the dictionary
    if (self->internal_dict != NULL)
    {
        PyObject *v = PyDict_GetItemString(self->internal_dict,
                                         "__deletenebula__");
        if (v != NULL)
            NebulaObject_Delete(self, NULL);
        Py_XDECREF(self->internal_dict);
    }
    else
        n_printf("Internal Dictionary is NULL\n");
     
    if(self->nebula_object)
        n_delete self->nebula_object;

    PyObject_Del(self);
}

//-----------------------------------------------------------------------------
/**
    @brief Call this function to flag the python object to delete the
    nebula object as well.
*/
PyObject * NebulaObject_SetAutoDel(NebulaObject *self, PyObject * /*args*/)
{
    PyObject * results = NULL;
    n_assert(NULL!=self);
    // Set an entry in the dictionary as a flag to delete the Nebula object
    // when the python object goes out of scope (gets deleted)
    if (self->internal_dict != NULL)
    {
        PyObject *obj = PyInt_FromLong(1l);  // this could be anything
        NebulaObject_SetAttr(self,"__deletenebula__",obj);
        Py_XDECREF(obj);
        Py_INCREF(Py_None);
        results = Py_None;
    }
    else
        n_printf("Internal Dictionary is NULL\n");

    return results;
}

// only one method defined...
PyMethodDef Nebula_methods[] = {
    {"_SetAutoDel_",  (PyCFunction)NebulaObject_SetAutoDel,  METH_VARARGS, NULL},
    {NULL,      NULL, 0, NULL}    /* sentinel */
};

//-----------------------------------------------------------------------------
/**
    @brief Handle a call to the object.

    If functionname =! NULL then we know we are really inside an OPCODE loop
    in the interpreter and need to call into Nebula. Makes the object
    stateful for one internal interpreter cycle.
*/
PyObject* NebulaObject_Call(NebulaObject *self,
                            PyObject *args,
                            PyObject * /*kw*/)
{
    PyObject * results;
    n_assert(NULL!=self);
    // This should have been set previously by getattro call:
    if ( self->functionname != NULL )
    {
        int res;
        res = NebulaObject_CallPObjectFunction(self,
                                               PyString_AsString(self->functionname),
                                               args, &results);
        if (res == 0)
        {
            Py_XDECREF(self->functionname);
            self->functionname = NULL;
            return results;
        }
        else
        {
            n_printf("CallPObjectFunction failed in Nebula_Call");
        }
        Py_DECREF(self->functionname);
        self->functionname = NULL;
    }
    else
        n_error("functionname is NULL (should have been set previously)\n");

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
PyObject * NebulaObject_GetAttrO(NebulaObject *self, PyObject *name)
{
    n_assert (NULL != self);
    n_assert (NULL != name);
    self->functionname = name;
    Py_INCREF(self);  // needed as we are using ourselves for the function call
    Py_INCREF(self->functionname);   // need to do this one..
    return (PyObject *)self;
}

//-----------------------------------------------------------------------------
/**
    @brief Call the actual Nebula function.
*/
int NebulaObject_CallNebulaFunction( nRoot* o, char * name,
                                     PyObject * commandArgs,
                                     PyObject **returndata)
{
    int results = 0;
    if (o)
    {
        nClass *cl = o->GetClass();
        nCmdProto *cmd_proto = (nCmdProto *) cl->FindCmdByName(name);
        if (cmd_proto)
        {
            // Invoke the command
            nCmd *ncmd = cmd_proto->NewCmd();
            n_assert(ncmd);
            // Create a new tuple that has just the arguments needed for the command
            // Passes Cmd object and the remaining tuple arguments to _getInArgs.
            // retrieve input args (skip the 'unknown' and cmd statement)
            if (!_getInArgs(ncmd, commandArgs))
            {
                PyErr_Format(PyExc_Exception,
                            "Broken input args, object '%s', command '%s'",
                            o->GetName(), name);
                results = -1;
            }
            else if (o->Dispatch(ncmd))
            {
                // let object handle the command
                if (returndata)
                {   // the caller wants return data
                    *returndata = _putOutArgs(ncmd);   // get the return data
                }
                results = 0;  // set successful return
            }
            else
            {
                PyErr_Format(PyExc_Exception,
                            "Dispatch error, object '%s', command '%s'",
                            o->GetName(), name);
                results = -1;
            }
            // In any case, cleanup the cmd object
            cmd_proto->RelCmd(ncmd);
        }
        else
        {
            // Set exception, the object doesn't know about the command!
            PyErr_Format(PyExc_AttributeError,
                         "Unknown command, object '%s', command '%s'",
                         o->GetName(), name);
            results = -1;
        }
    }
    else
    {
        // Unable to acquire current object
        // Set appropriate exception
        PyErr_SetString(Npy_ErrorObject,
                        "Unable to get a pointer to the Nebula object - probably Nebula object is deleted");
        results = -1;
    }
    return results;
}

//-----------------------------------------------------------------------------
/**
    @brief The main function to make a 'function' call on an object - used
    by 'Set', etc.
*/
int NebulaObject_CallPObjectFunction(NebulaObject *self, char *name,
                                     PyObject * args,
                                     PyObject ** returndata)
{
    n_assert (NULL != self);
    n_assert (NULL != name);
 
    PyObject * commandArgs;
    int  results = -1;
 
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
            Py_INCREF (v); // need to keep access to temp for a while
            PyTuple_SetItem(*returndata, 0, v);
            Py_DECREF (v);
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
        *returndata = PyObject_CallObject(v,args);   //lets call it
        Py_DECREF(v);
        return 0;
    }
    PyErr_Clear();
 
    // finally we assume it's a Nebula attribute and continue
 
    // lets lookup the attribute to see if we have already saved a pointer to it
    if (args == NULL)
    { // just a function call so create a new empty tuple
        commandArgs = PyTuple_New(0);
    }
    else
    {
        if (!PyTuple_Check (args))
        {
            // assume a single argument
            commandArgs = PyTuple_New(1);
            Py_INCREF ( args);   // need to keep access to args for a while
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
    results = NebulaObject_CallNebulaFunction(o, name, commandArgs, returndata);

    Py_XDECREF(commandArgs);
    return   results;
}

//-----------------------------------------------------------------------------
/**
    @brief Internal setattribute function to REALLY set the Python attribute

    Needed as the "default" setattr calls the nebula object.
*/
extern int  NebulaObject_SetAttr(NebulaObject *self, char *name, PyObject *v)
{
    n_assert ( NULL != self);
    n_assert ( NULL != name);

    if (self->internal_dict == NULL)
    {
        self->internal_dict = PyDict_New();
        if (self->internal_dict == NULL)
            n_error ("Couldn't create a dictionary\n");
    }
    if (v == NULL)
    {
        int rv = PyDict_DelItemString(self->internal_dict, name);
        if (rv < 0)
            PyErr_SetString(PyExc_AttributeError, 
                            "Tried to delete a nonexistent NebulaObject attribute");
        return rv;
    }
    else
    {
        return PyDict_SetItemString(self->internal_dict, name, v);
    }
}

static int neb_dict_length(NebulaObject *mp)
{
    assert(mp->internal_dict != NULL);
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

static PyObject *neb_dict_subscript(NebulaObject *mp, register PyObject *key)
{
    assert(mp->internal_dict != NULL);
    assert(key);
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
        return PyErr_Format(PyExc_KeyError, "key error ( '%s' not found )", keystr);
    }
    else
    {
        n_printf("NebulaObject_GetPointer returned NULL\n");
        return 0;
    }
}

staticforward PyObject *nebiter_new(NebulaObject *nebobj);

static PyObject *neb_iter(NebulaObject *neb_obj)
{
    return nebiter_new(neb_obj);
}


static PyMappingMethods nebula_as_mapping = {
    (inquiry)neb_dict_length, /*mp_length*/
    (binaryfunc)neb_dict_subscript, /*mp_subscript*/
    (objobjargproc)0, /*mp_ass_subscript*/
};


static char nebula_object_doc[] =
"python wrapper for nebula object\n";

// now to actually define the new python type to represent a Nebula Object

PyTypeObject Nebula_Type = {
    /* The ob_type field must be initialized in the module init function
     * to be portable to Windows without using C++. */
    PyObject_HEAD_INIT(NULL)
    0,       /*ob_size*/
    "Nebula",      /*tp_name*/
    sizeof(NebulaObject),   /*tp_basicsize*/
    0,       /*tp_itemsize*/
    /* methods */
    (destructor)NebulaObject_Dealloc, /*tp_dealloc*/
    0,       /*tp_print*/
    0, /*tp_getattr*/
    0, /*tp_setattr*/
    0,       /*tp_compare*/
    0,       /*tp_repr*/
    0,       /*tp_as_number*/
    0,       /*tp_as_sequence*/
    &nebula_as_mapping,       /*tp_as_mapping*/
    0,       /*tp_hash*/
    (ternaryfunc)NebulaObject_Call,  /*tp_call*/  // **** FUNCTIONCALL CATCHER
    0,                      /*tp_str*/
    (getattrofunc)NebulaObject_GetAttrO,  /*tp_getattro*/
    0,                      /*tp_setattro*/
    0,                      /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_HAVE_WEAKREFS,     /*tp_flags*/
    nebula_object_doc,      /*tp_doc*/
    0,                      /*tp_traverse*/
    0,                      /*tp_clear*/
    0,                      /*tp_richcompare*/
    offsetof(NebulaObject, in_weakreflist), /*tp_weaklistoffset*/
    (getiterfunc)neb_iter,                      /*tp_iter*/
    0,                      /*tp_iternext*/
    0,                      /*tp_methods*/
    0,                      /*tp_members*/
    0,                      /*tp_getset*/
    0,                      /*tp_base*/
    0,                      /*tp_dict*/
    0,                      /*tp_descr_get*/
    0,                      /*tp_descr_set*/
    0,                      /*tp_dictoffset*/
    0,                      /*tp_init*/
    0,                      /*tp_alloc*/
    0,                      /*tp_new*/
    0,                      /*tp_free*/
    0,                      /*tp_is_gc*/
};

/* Nebula - childrens iterator type */

extern PyTypeObject PyNebIter_Type; /* Forward */

typedef struct {
    PyObject_HEAD
    NebulaObject *ni_nebulaobject;
    int current_pos;
    nRoot *cur_obj;
} nebiterobject;

static PyObject *nebiter_new(NebulaObject *nebobj)
{
    nebiterobject *nebi;
    nebi = PyObject_NEW(nebiterobject, &PyNebIter_Type);
    if (nebi == NULL)
        return NULL;
    Py_INCREF(nebobj);
    nebi->ni_nebulaobject = nebobj;
    nebi->current_pos = 0;
      
    nebi->cur_obj = NULL;
    nRoot *temp = NebulaObject_GetPointer(nebobj);
    if (!temp)
    {
        PyErr_SetString(PyExc_Exception, "pointer to nebula object is null!");
        return NULL;
    }
    nebi->cur_obj = temp->GetHead();
    return (PyObject *)nebi;
}

static void nebiter_dealloc(nebiterobject *nebi) {
    Py_DECREF(nebi->ni_nebulaobject);
    PyObject_DEL(nebi);
}

static PyObject *nebiter_next(nebiterobject *nebi, PyObject *args)
{
    if (!nebi->cur_obj)
    {
        PyErr_SetObject(PyExc_StopIteration, Py_None);
        return NULL;
    }
    else
    {
        if (nebi->current_pos == 0)
        {
            nebi->current_pos++;
            return (PyObject *)NebulaObject_New(nebi->cur_obj);
        }
        nebi->cur_obj = nebi->cur_obj->GetSucc();
        if (!nebi->cur_obj )
        {
            PyErr_SetObject(PyExc_StopIteration, Py_None);
            return NULL;
        }
        nebi->current_pos++;
        return (PyObject *)NebulaObject_New(nebi->cur_obj);
    }
    PyErr_SetObject(PyExc_StopIteration, Py_None);
    return NULL;
}

static PyObject *nebiter_getiter(PyObject *it) {
    Py_INCREF(it);
    return it;
}

static PyMethodDef nebiter_methods[] = {
  {"next",  (PyCFunction)nebiter_next, METH_VARARGS,
   "it.next() -- get the next value, or raise StopIteration"},
  {NULL,    NULL}   /* sentinel */
};

static PyObject *nebiter_iternext(nebiterobject *nebi)
{
    if (!nebi->cur_obj)
    {
        PyErr_SetObject(PyExc_StopIteration, Py_None);
        return NULL;
    }
    else
    {
        if (nebi->current_pos == 0)
        {
            nebi->current_pos++;
            return (PyObject *)NebulaObject_New(nebi->cur_obj);
        }
        nebi->cur_obj = nebi->cur_obj->GetSucc();
        if (!nebi->cur_obj )
        {
            PyErr_SetObject(PyExc_StopIteration, Py_None);
            return NULL;
        }
        nebi->current_pos++;
        return (PyObject *)NebulaObject_New(nebi->cur_obj);
    }
    PyErr_SetObject(PyExc_StopIteration, Py_None);
    return NULL;
}


// Nebula Iterator type
PyTypeObject PyNebIter_Type = {
    PyObject_HEAD_INIT(&PyType_Type)
    0,                          /* ob_size */
    "nebula-iterator",          /* tp_name */
    sizeof(nebiterobject),      /* tp_basicsize */
    0,                          /* tp_itemsize */
    /* methods */
    (destructor)nebiter_dealloc,     /* tp_dealloc */
    0,          /* tp_print */
    0,          /* tp_getattr */
    0,          /* tp_setattr */
    0,          /* tp_compare */
    0,          /* tp_repr */
    0,          /* tp_as_number */
    0,          /* tp_as_sequence */
    0,          /* tp_as_mapping */
    0,          /* tp_hash */
    0,          /* tp_call */
    0,          /* tp_str */
    PyObject_GenericGetAttr,    /* tp_getattro */
    0,          /* tp_setattro */
    0,          /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT,     /* tp_flags */
    0,          /* tp_doc */
    0,          /* tp_traverse */
    0,          /* tp_clear */
    0,          /* tp_richcompare */
    0,          /* tp_weaklistoffset */
    (getiterfunc)nebiter_getiter,    /* tp_iter */
    (iternextfunc)nebiter_iternext,  /* tp_iternext */
    nebiter_methods,     /* tp_methods */
    0,          /* tp_members */
    0,          /* tp_getset */
    0,          /* tp_base */
    0,          /* tp_dict */
    0,          /* tp_descr_get */
    0,          /* tp_descr_set */
};

#ifdef __cplusplus
}
#endif

//--------------------------------------------------------------------
//  EOF
//--------------------------------------------------------------------