#ifndef N_CMDPROTO_H
#define N_CMDPROTO_H
//------------------------------------------------------------------------------
/**
    An nCmdProto object holds the prototype description for an nCmd
    object and can construct nCmd objects based on the prototype
    description "blue print".

    The prototype description is given to the nCmdProto constructor
    as a string of the format

    "outargs_name_inargs"

    "outargs" is a list of characters describing number and datatypes
    of output arguments, "inargs" describes the input args in the
    same way. "name" is the name of the command.

    The following datatypes are defined:

      'v'     - void
      'i'     - int
      'f'     - float
      's'     - string
      'b'     - bool

    Examples of prototype descriptions:

      v_rotate_fff    - name is 'rotate', no output args, 3 float input args, 
      v_set_si        - name is 'set', no output args, one string and one int input arg
      fff_getrotate_v - 3 float output args, no input arg, name is 'getrotate'

    (C) 1999 A.Weissflog
*/
#ifndef N_TYPES_H
#include "kernel/ntypes.h"
#endif

#ifndef N_HASHNODE_H
#include "util/nhashnode.h"
#endif

#undef N_DEFINES
#define N_DEFINES nCmdProto
#include "kernel/ndefdllclass.h"

//------------------------------------------------------------------------------
class nCmd;
class N_PUBLIC nCmdProto : public nHashNode 
{
public:
    /// constructor
    nCmdProto(const char *proto_def, uint id, void (*cmd_proc)(void *, nCmd *));
    /// copy constructor
    nCmdProto(const nCmdProto& rhs);
    /// destructor
    ~nCmdProto();
    /// get fourcc code
    uint GetId() const;
    /// get number of input args
    int GetNumInArgs() const;
    /// get number of output args
    int GetNumOutArgs() const;
    /// get overall number of args
    int GetNumArgs() const;
    /// get the prototype string
    const char* GetProtoDef() const;
    /// create a new nCmd object
    nCmd* NewCmd();
    /// release nCmd object
    void RelCmd(nCmd* cmd);
	/// pointer to C style command handler
    void (*cmdProc)(void *, nCmd *);

private:
    /// check if the provided data type character is a valid argument
    bool IsValidArg(char c);

    uint fourcc;
    uchar numInArgs;
    uchar numOutArgs;
    bool cmdLocked;
    nCmd* cmdTemplate;

    nString protoDef;
};

//------------------------------------------------------------------------------
/**
*/
inline
uint
nCmdProto::GetId() const
{
    return this->fourcc;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nCmdProto::GetNumInArgs() const
{
    return this->numInArgs;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nCmdProto::GetNumOutArgs() const
{
    return this->numOutArgs;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nCmdProto::GetNumArgs() const
{
    return (this->numInArgs + this->numOutArgs);
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nCmdProto::GetProtoDef() const
{
    return this->protoDef.IsEmpty() ? 0 : this->protoDef.Get();
}

//------------------------------------------------------------------------------
/**
*/
inline
bool 
nCmdProto::IsValidArg(char c)
{
    switch(c) {
        case 'i':
        case 'f':
        case 's':
        case 'v':
        case 'b':
        case 'o':
        case 'c':
            return true;
        default:
            return false;
    }
}

//------------------------------------------------------------------------------
#endif    
    
    
