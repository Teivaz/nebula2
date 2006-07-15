#ifdef __cplusplus
    extern "C" {
#endif

extern PyTypeObject Nebula_Type;

class nRoot; // forward

typedef struct    {
    PyObject_HEAD
    PyObject   *internal_dict; // Attributes dictionary 
    PyObject   *functionname;  // Storage for function call name for tp_call
    PyObject   *in_weakreflist; /* List of weak references */
    nRef<nRoot> *nebula_object;
} NebulaObject;

#define NebulaObject_Check(v) ((v)->ob_type == &Nebula_Type)


// functions to work with list of created objects
NebulaObject *CreatedObjectsList_GetObject(nRoot* real_neb_obj);
int CreatedObjectsList_AddObject(NebulaObject *nebula_object, nRoot* real_neb_obj);
  

  
NebulaObject * NebulaObject_New(nRoot *nebulaObj);
nRoot * NebulaObject_GetPointer(NebulaObject * self);
// Defined in npythoncmds.cc, but used outside of it:
PyObject* pythoncmd_Delete(PyObject *self, PyObject *args);

// forward dec needed:
int        NebulaObject_SetAttr(NebulaObject *self, char *name, PyObject *v);
PyObject * NebulaObject_GetAttr(NebulaObject *self, PyObject *name);

int NebulaObject_CallPObjectFunction(NebulaObject *self, char *name,
                                     PyObject * args,
                                     PyObject ** returndata);
int NebulaObject_CallNebulaFunction(nRoot* o, char * name,
                                    PyObject * commandArgs, 
                                    PyObject **returndata);

#ifdef __cplusplus
    }
#endif
