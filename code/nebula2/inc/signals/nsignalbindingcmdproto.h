#ifndef N_SIGNALBINDINGCMDPROTO_H
#define N_SIGNALBINDINGCMDPROTO_H
//------------------------------------------------------------------------------
/**
    @class nSignalBindingCmdProto
    @ingroup NebulaSignals

    nSignalBindingCmdProto is a subclass of nSignalBinding used mainly in two
    cases:

     - When the binding callback invocates code on the script side through the
       Nebula scripting system.

     - When the binding itself has been created from the scripting side.

    (C) 2004 Tragnarion Studios
*/
//------------------------------------------------------------------------------
#include "kernel/nref.h"
#include "util/nstring.h"
#include "signals/nsignalbinding.h"


//------------------------------------------------------------------------------
class nSignalBindingCmdProto : public nSignalBinding
{
public:
    /// binding constructor from nCmdProto (no rebind done)
    nSignalBindingCmdProto(nObject * obj, nCmdProto * proto, int priority);
    /// binding constructor from fourcc code (rebind optional)
    nSignalBindingCmdProto(nObject * obj, nFourCC fcc, int priority, bool rebindOnCall = false);
    /// binding constructor from command name (rebind optional)
    nSignalBindingCmdProto(nObject * obj, const char * name, int priority, bool rebindOnCall = false);
    /// destructor
    virtual ~nSignalBindingCmdProto();

    /** @name Invocation
        Methods for invoking a signal binding. */
    //@{
    /// Invocation used from scripting side
    virtual bool Invoke(nCmd * cmd);
    /// Invocation used from native side
    virtual bool Invoke(va_list args);
    //@}

    /// Return prototype string
    virtual const char * GetProtoDef() const;

    /// Return true if binding points to the object provided
    virtual bool IsBoundWithObject(const nObject * objectPtr) const;
    /// Return true if binding bounds to a command with the cmdproto provided
    virtual bool IsBoundWithCmdProto(const nCmdProto * functionPtr) const;
    /// Return true if binding bounds to a command with the name provided
    virtual bool IsBoundWithCmdName(const char * name) const;
    /// Return true if binding bounds to a command with the fourcc provided
    virtual bool IsBoundWithCmdFourCC(nFourCC fourcc) const;
    /// Return true if the binding is valid
    virtual bool IsValid() const;

protected:

    nCmdProto * GetCmdProto() const;

    enum RebindStatus {
        DontRebind,
        RebindByFourCC,
        RebindByName
    };

    RebindStatus rebind;
    nRef<nObject> refObject;
    union
    {
        nCmdProto * cmdProto;
        nFourCC fourcc;
        nString * cmdname;
    };
};

//------------------------------------------------------------------------------
inline
nSignalBindingCmdProto::nSignalBindingCmdProto(nObject * obj, nCmdProto * proto, int priority) :
    nSignalBinding(priority),
    refObject(obj),
    cmdProto(proto),
    rebind(DontRebind)
{
    /// empty
}

//------------------------------------------------------------------------------
inline
nSignalBindingCmdProto::nSignalBindingCmdProto(nObject * obj, nFourCC fcc, int priority, bool rebindOnCall) :
    nSignalBinding(priority),
    refObject(obj)
{
    if (rebindOnCall)
    {
        rebind = RebindByFourCC;
        this->fourcc = fcc;
    }
    else
    {
        rebind = DontRebind;
        this->cmdProto = obj->GetClass()->FindCmdById(this->fourcc);
    }
}

//------------------------------------------------------------------------------
inline
nSignalBindingCmdProto::nSignalBindingCmdProto(nObject * obj, const char * name, int priority, bool rebindOnCall) :
    nSignalBinding(priority),
    refObject(obj)
{
    if (rebindOnCall)
    {
        rebind = RebindByName;
        this->cmdname = n_new(nString)(name);
    }
    else
    {
        rebind = DontRebind;
        this->cmdProto = obj->GetClass()->FindCmdByName(name);
    }
}

//------------------------------------------------------------------------------
inline
nSignalBindingCmdProto::~nSignalBindingCmdProto()
{
    /// empty
    if (rebind == RebindByName)
    {
        n_delete(this->cmdname);
        this->cmdname = 0;
    }
}

//------------------------------------------------------------------------------
inline
bool
nSignalBindingCmdProto::IsValid() const
{
    return this->refObject.isvalid();
}

//------------------------------------------------------------------------------
#endif // N_SIGNALBINDINGCMDPROTO_H
