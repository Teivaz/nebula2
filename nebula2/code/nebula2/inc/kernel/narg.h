#ifndef N_ARG_H
#define N_ARG_H
//------------------------------------------------------------------------------
/**
    @class nArg

    nArg encapsulates a typed argument (float, int, bool, string
    or object pointer) into a uniform C++ class.

    (C) 2002 RadonLabs GmbH
*/
#ifndef N_TYPES_H
#include "kernel/ntypes.h"
#endif

#include <string.h>

//------------------------------------------------------------------------------
class nArg 
{
public:
    enum ArgType 
    { 
        ARGTYPE_VOID,
        ARGTYPE_INT,
        ARGTYPE_FLOAT,
        ARGTYPE_STRING,
        ARGTYPE_BOOL,
        ARGTYPE_OBJECT,
        ARGTYPE_CODE,
        ARGTYPE_LIST,
    };

    /// the default constructor
    nArg();
    /// the copy constructor
    nArg(const nArg&);
    /// the destructor
    ~nArg();
    /// the equals operator
    bool operator==(const nArg& rhs) const;

    /// set with contents of other arg
    void Set(const nArg&);
    /// set content to an integer
    void SetI(int i);
    /// set content to a bool
    void SetB(bool b);
    /// set content to a float
    void SetF(float f);
    /// set content to a string
    void SetS(const char* s);
    /// set content to an (object) pointer
    void SetO(void*);
    /// set content to a chunk of script code
    void SetC(const char* c);
    /// Set content to an array of nArgs
    void SetL(nArg* _l, int len);

    /// get the content data type
    nArg::ArgType GetType(void) const;
    /// get int content
    int   GetI(void) const;
    /// get bool content
    bool  GetB(void) const;
    /// get float content
    float GetF(void) const;
    /// get string content
    const char* GetS(void) const;
    /// get object pointer content
    void* GetO(void) const;
    /// get chunk of script code
    const char* GetC(void) const;
    /// Get an array of nArgs
    int GetL(nArg*&) const;

private:
    nArg::ArgType type;
    union 
    {
        int i;
        bool b;
        float f;
        char *s;
        void *o;
        nArg *l;
    };

    // Number of nArgs in array.
    // Only used for 'list' type
    int listLen;

};
//-----------------------------------------------------------------------------
/**
    The default constructor initialized the arg type to ARGTYPE_VOID.
*/
inline nArg::nArg()
{
    this->type = ARGTYPE_VOID;
    this->s = NULL;
    this->listLen = 0;
}

//-----------------------------------------------------------------------------
/**
    Sets nArg object to content of another nArg object.

    @param arg reference to nArg object to be copied
*/
inline void nArg::Set(const nArg& arg)
{
    this->type = arg.type;
    switch(this->type) 
    {
        case ARGTYPE_INT: 
            this->i = arg.i; 
            break;
        case ARGTYPE_FLOAT:    
            this->f = arg.f; 
            break;
        case ARGTYPE_STRING:
        case ARGTYPE_CODE:
            if (this->s) 
            {
                n_free(this->s);
                this->s = 0;
            }
            if (arg.s) 
            {   
                this->s = n_strdup((char *) arg.s);
            } 
            else 
            {
                this->s = 0;
            }
            break;
        case ARGTYPE_BOOL:     
            this->b = arg.b; 
            break;
        case ARGTYPE_OBJECT:
            this->o = arg.o;
            break;
        case ARGTYPE_VOID:
            break;
        case ARGTYPE_LIST:
            if (this->l)
            {
                n_delete[] this->l;
                this->l = 0;
                this->listLen = 0;
            }
            if (arg.l)
            {
                this->l = n_new nArg[arg.listLen];
                for (int i = 0; i < arg.listLen; i++)
                {
                    this->l[i].Set(arg.l[i]);
                }
                this->listLen = arg.listLen;
            }
            else
            {
                this->l = 0;
                this->listLen = 0;
            }
            break;
    }
}

//-----------------------------------------------------------------------------
/**
    The copy constructor simply calls nArg::Set()

    @param arg reference to nArg object to be copied
*/
inline nArg::nArg(const nArg& arg)
{
    nArg::Set(arg);
}    
        
//-----------------------------------------------------------------------------
/**
*/
inline nArg::~nArg()
{
    if (ARGTYPE_STRING == this->type)
    {
        if (this->s) n_free(this->s);
    }
    else if (ARGTYPE_LIST == this->type)
    {
        if (this->l)
        {
            n_delete[] this->l;
            this->l = 0;
        }
    }
}   

//-----------------------------------------------------------------------------
/**
    Checks whether data type and contents of the object are identical.
*/
inline
bool
nArg::operator==(const nArg& rhs) const
{
    if (this->type == rhs.type)
    {
        switch (this->type)
        {
            case ARGTYPE_INT: 
                return (this->i == rhs.i); 
            case ARGTYPE_FLOAT:    
                return (this->f == rhs.f); 
            case ARGTYPE_STRING:
            case ARGTYPE_CODE:
                if (this->s && rhs.s)
                {
                    return (0 == strcmp(this->s, rhs.s));
                }
                else
                {
                    return false;
                }
            case ARGTYPE_BOOL:     
                return (this->b == rhs.b); 
            case ARGTYPE_OBJECT:
                return (this->o == rhs.o);
            case ARGTYPE_VOID:
                return true;
            case ARGTYPE_LIST:
                if ((this->l && rhs.l) && (this->listLen == rhs.listLen))
                {
                    for(int i = 0; i < this->listLen; i++)
                    {
                        if (!(this->l[i] == rhs.l[i]))
                            return false;
                    }
                    return true;
                }
        }        
    }
    return false;
}    
 
//-----------------------------------------------------------------------------
/**
    Sets the contents to an integer, and sets the arg type to ARGTYPE_INTEGER.

    @param _i the integer
*/
inline void nArg::SetI(int _i)
{
    n_assert((ARGTYPE_VOID==this->type)||(ARGTYPE_INT==this->type));
    this->type = ARGTYPE_INT;
    this->i = _i;
}
//-----------------------------------------------------------------------------
/**
    Sets the contents to a bool value, and set the arg type to ARGTYPE_BOOL.

    @param _b the bool value
*/
inline void nArg::SetB(bool _b)
{
    n_assert((ARGTYPE_VOID==this->type)||(ARGTYPE_BOOL==this->type));
    this->type = ARGTYPE_BOOL;
    this->b = _b;
}

//-----------------------------------------------------------------------------
/**
    Sets the contents to a float value, and sets the arg type to ARGTYPE_FLOAT.

    @param _f the float value
*/
inline void nArg::SetF(float _f)
{
    n_assert((ARGTYPE_VOID==this->type)||(ARGTYPE_FLOAT==this->type));
    this->type = ARGTYPE_FLOAT;
    this->f = _f;
}

//-----------------------------------------------------------------------------
/**
    Sets the contents to a string, and sets the arg type to ARGTYPE_STRING.
    The string is duplicated internally.

    @param _s the string
*/
inline void nArg::SetS(const char *_s)
{
    n_assert((ARGTYPE_VOID==this->type)||(ARGTYPE_STRING==this->type));
    if (this->s) 
    {
        n_free(this->s);
        this->s = NULL;
    }
    this->type = ARGTYPE_STRING;
    if (_s) {
        this->s = n_strdup(_s);
    } else this->s = NULL;
}

//-----------------------------------------------------------------------------
/**
    Sets the contents to a chunk of code, and sets the arg type to ARGTYPE_CODE.
    The string is duplicated internally.

    @param _c the script chunk string
*/
inline void nArg::SetC(const char* _c)
{
    n_assert((ARGTYPE_VOID==this->type)||(ARGTYPE_CODE==this->type));
    this->type = ARGTYPE_VOID;
    this->SetS(_c);
    this->type = ARGTYPE_CODE;
}

//-----------------------------------------------------------------------------
/**
    Sets the contents to an object pointer, and sets the arg type to
    ARGTYPE_OBJECT. The pointer is NOT safe (if the object is destroyed,
    the pointer points to Nirvana).

    @param _o the object pointer
*/
inline void nArg::SetO(void *_o)
{
    n_assert((ARGTYPE_VOID==this->type)||(ARGTYPE_OBJECT==this->type));
    this->type = ARGTYPE_OBJECT;
    this->o = _o;
}

//-----------------------------------------------------------------------------
/**
    Sets the contents to an array of other nArgs, and sets the arg type 
    to ARGTYPE_LIST.
    The array is NOT duplicated.

    @param _l pointer to array of nArg
    @param len length of array
*/
inline void nArg::SetL(nArg* _l, int len)
{
    n_assert((ARGTYPE_VOID==this->type)||(ARGTYPE_LIST==this->type));
    if (this->l)
    {
        n_delete[] this->l;
        this->l = 0;
        this->listLen = 0;
    }
    this->type = ARGTYPE_LIST;
    if (_l)
    {
        this->l = _l;
        this->listLen = len;
    } 
    else
    {
        this->l = 0;
        this->listLen = 0;
    }
}

//-----------------------------------------------------------------------------
/**
    Returns the argument type of the object, which is one of
    ARGTYPE_VOID        - unitialized
    ARGTYPE_INT
    ARGTYPE_BOOL
    ARGTYPE_FLOAT
    ARGTYPE_OBJECT
    ARGTYPE_CODE
    ARGTYPE_LIST

    @return the arg type
*/
inline 
nArg::ArgType 
nArg::GetType(void) const
{
    return this->type;
}

//-----------------------------------------------------------------------------
/**
    Returns the integer content of the object. Throws assertion if
    not set to an integer.

    @return the integer value
*/
inline 
int 
nArg::GetI(void) const
{
    n_assert(ARGTYPE_INT==this->type);
    return this->i;
}

//-----------------------------------------------------------------------------
/**
    Returns the bool content of the object. Throws assertion if
    not set to a bool.

    @return the bool value
*/
inline 
bool 
nArg::GetB(void) const
{
    n_assert(ARGTYPE_BOOL==this->type);
    return this->b;
}

//-----------------------------------------------------------------------------
/**
    Returns the float content of the object. Throws assertion if
    not set to a float.

    @return the float value
*/
inline 
float 
nArg::GetF(void) const
{
    n_assert(ARGTYPE_FLOAT==this->type);
    return this->f;
}

//-----------------------------------------------------------------------------
/**
    Returns the string content of the object. Throws assertion if
    not set to a string.

    @return the string pointer
*/
inline 
const char*
nArg::GetS(void) const
{
    n_assert(ARGTYPE_STRING==this->type);
    return this->s;    
}

//-----------------------------------------------------------------------------
/**
    Returns the code chunk content of the object as a const char*.

    @return pointer to script code chunk
*/
inline
const char*
nArg::GetC(void) const
{
    n_assert(ARGTYPE_CODE == this->type);
    return this->s;
}

//-----------------------------------------------------------------------------
/**
    Returns the object pointer content of the object. Throws assertion if
    not set to an object pointer.

    @return the object pointer
*/
inline 
void*
nArg::GetO(void) const
{
    n_assert(ARGTYPE_OBJECT==this->type);
    return this->o;
}

//-----------------------------------------------------------------------------
/**
    Returns the nArg array content of the object. Throws assertion if
    not set to a list.

    @param _l Pointer to nArg array. Will be set by this function

    @return the length of the array
*/
inline 
int
nArg::GetL(nArg*& _l) const
{
    n_assert(ARGTYPE_LIST==this->type);
    _l = this->l;
    return this->listLen;
}

//--------------------------------------------------------------------
#endif
