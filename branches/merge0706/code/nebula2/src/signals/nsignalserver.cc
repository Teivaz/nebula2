//------------------------------------------------------------------------------
/**
    @file nsignalserver.cc

    (c) 2004 Tragnarion Studios
*/
//------------------------------------------------------------------------------
#include "kernel/ncmd.h"
#include "signals/nsignalserver.h"
#include "signals/nsignalemitter.h"
#include "kernel/ntimeserver.h"
#include "kernel/nkernelserver.h"

nNebulaClass( nSignalServer, "kernel::nroot" );

//------------------------------------------------------------------------------
nSignalServer* nSignalServer::Singleton = 0;

//------------------------------------------------------------------------------
void
nSignalServer::Trigger(nTime t)
{
    // look for the insertion point (sorted by time t)
    nSignalServer::nPostedSignal * postedSignal = static_cast<nSignalServer::nPostedSignal *> (this->postedSignals.GetHead());
    while (postedSignal && postedSignal->t <= t)
    {
        if (postedSignal->emitter.isvalid())
        {
            // execute the signal
            postedSignal->cmd->GetProto()->Dispatch(postedSignal->emitter.get(), postedSignal->cmd);
        }

        // release posted signal
        postedSignal->cmd->GetProto()->RelCmd(postedSignal->cmd);
        postedSignal->emitter.invalidate();
        postedSignal->cmd = 0;
        postedSignal->t = 0;
        postedSignal->Remove();
        this->freeSignals.AddTail(postedSignal);

        postedSignal = static_cast<nSignalServer::nPostedSignal *> (this->postedSignals.GetHead());
    }
}

//------------------------------------------------------------------------------
bool
nSignalServer::PostCmd(nTime relT, nObject * object, nCmd * cmd)
{
    nPostedSignal * postedSignal = static_cast<nSignalServer::nPostedSignal *> (this->freeSignals.GetHead());
    if (postedSignal)
    {
        // convert relative time in absolute time
        nTime t = relT + nTimeServer::Instance()->GetFrameTime();

        // look for the insertion point (sorted by time t)
        nSignalServer::nPostedSignal * insertPoint = static_cast<nSignalServer::nPostedSignal *> (this->postedSignals.GetHead());
        while (insertPoint && insertPoint->t < t)
        {
            insertPoint = static_cast<nSignalServer::nPostedSignal *> (insertPoint->GetSucc());
        }

        // initialize & insert
        postedSignal->emitter = object;
        postedSignal->cmd = cmd;
        postedSignal->t = t;
        postedSignal->Remove();
        if (insertPoint)
        {
            postedSignal->InsertBefore(insertPoint);
        }
        else
        {
            this->postedSignals.AddTail(postedSignal);
        }

        return true;
    }

    return false;
}


//------------------------------------------------------------------------------
//  EOF
//------------------------------------------------------------------------------
