#ifndef N_CMDPROTO_H
#define N_CMDPROTO_H
//------------------------------------------------------------------------------
/**
    @class nCmdProto
    @ingroup NebulaObjectSystem
    @ingroup NebulaScriptServices

    An nCmdProto object holds the prototype description for an nCmd
    object and can construct nCmd objects based on the prototype
    description "blue print".

    The prototype description is given to the nCmdProto constructor
    as a string of the format

    "outargs_name_inargs"

    "outargs" is a list of characters describing number and data types
    of output arguments, "inargs" describes the input args in the
    same way. "name" is the name of the command.

    The following data types are defined:

      - 'v'     - void
      - 'i'     - int
      - 'f'     - float
      - 's'     - string
      - 'b'     - bool
      - 'l'     - list

    Examples of prototype descriptions:

    @verbatim
      v_rotate_fff    - name is 'rotate', no output args, 3 float input args,
      v_set_si        - name is 'set', no output args, one string and one int input arg
      fff_getrotate_v - 3 float output args, no input arg, name is 'getrotate'
    @endverbatim

    (C) 1999 A.Weissflog
*/
#include "kernel/ntypes.h"
#include "util/nhashnode.h"

//------------------------------------------------------------------------------
struct ProtoDefInfo
{
    ProtoDefInfo(const char* proto_def);

    char outArgs[64];
    char inArgs[64];
    char name[128];
    uchar numOutArgs;
    uchar numInArgs;
    bool valid;
};

//------------------------------------------------------------------------------
class nCmd;
class nCmdProto : public nHashNode
{
public:

    /// constructor
    nCmdProto(const char* proto_def, nFourCC id);
    /// copy constructor
    nCmdProto(const nCmdProto& rhs);
    /// destructor
    virtual ~nCmdProto();
    /// execute a command on the provided object
    virtual bool Dispatch(void*, nCmd*) = 0;
    /// get fourcc code
    nFourCC GetId() const;
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

private:

    nFourCC fourcc;
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
nFourCC
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
#endif
