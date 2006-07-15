//------------------------------------------------------------------------------
/**
    @file nsignal.cc
    @author Mateu Batle Sastre <mateu.batle@tragnarion.com>

    (c) 2004 Tragnarion Studios
*/
//------------------------------------------------------------------------------
#include "kernel/ntypes.h"
#include "signals/nsignal.h"
#include "signals/nsignalemitter.h"
#include "signals/nsignalbindingset.h"
#include "signals/nsignalbinding.h"

//------------------------------------------------------------------------------
// internal variable used as temporal buffer
char _nSignalPrototypeBuffer[N_MAXPATH];

//------------------------------------------------------------------------------
bool
nSignal::Dispatch(void * emitter, nCmd * cmd)
{
    // get binding list for emitter for this signal
    nSignalBindingSet * bs = this->GetBindingSet(
        reinterpret_cast<nObject *> (emitter),
        this->GetId()
    );

    // if binding set exists then call every binding in the list
    if (bs)
    {
        nSignalBinding * binding = bs->GetHead();
        while (binding)
        {
            if (binding->IsValid())
            {
                binding->Invoke(cmd);
                binding = binding->GetSucc();
            }
            else
            {
                nSignalBinding * tmp = binding->GetSucc();
                n_delete(binding);
                binding = tmp;
            }
        }
    }

    return true;
}

//------------------------------------------------------------------------------
/**
*/
bool
nSignal::Dispatch(nSignalEmitter * emitter, va_list args)
{
    /*
    XXX: The dispatch could be implemented same way as Dispatch(emitter, nCmd)
    but calling Invoke(va_list args).

    The most optimum could be to know if the binding is done with scripting or
    native code, in order to call the proper member function with nCmd or args
    depending on the case.
    */

    // get new command and forward to Dispatch
    nCmd * cmd = this->NewCmd();
    cmd->CopyInArgsFrom(args);
    this->Dispatch(static_cast<nObject *> (emitter), cmd);
    this->RelCmd(cmd);

    return true;
}

//------------------------------------------------------------------------------
/**
*/
nSignalBindingSet *
nSignal::GetBindingSet(nSignalEmitter * emitter, nFourCC signalId) const
{
    return emitter->FindSignalBindingSet(signalId);
}

//------------------------------------------------------------------------------
//  EOF
//------------------------------------------------------------------------------
