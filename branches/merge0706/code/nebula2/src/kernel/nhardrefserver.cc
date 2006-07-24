//------------------------------------------------------------------------------
//  © 2001 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "kernel/nkernelserver.h"
#include "kernel/nhardrefserver.h"
#include "kernel/nhardref.h"
#include "kernel/nroot.h"

//------------------------------------------------------------------------------
/**
*/
nHardRefServer::nHardRefServer()
{
}

//------------------------------------------------------------------------------
/**
    remove any nHardRef objects that are still linked
*/
nHardRefServer::~nHardRefServer()
{
    // removes nHashNode from nHashList, do not delete because they are
    // embedded in nHardRef object
    while (this->strList.RemHead());
}

//------------------------------------------------------------------------------
/**
    Registers a nHardRef object with the nHardRefServer. The nHardRef object
    must be initialized with a the full path name of a target nRoot object.
    If the target object exists, nHardRefServer will validate the nHardRef
    immediately and return. Otherwise the nHardRef will be inserted into
    the internal nHashList until the target object is registered to the
    nHardRefServer through RegisterTargetObject().

    @param  kernelServer    pointer to the nKernelServer
    @param  hardRef         the nHardRef object
*/
void
nHardRefServer::RegisterHardRef(nKernelServer* kernelServer, nHardRef<nRoot>* hardRef)
{
    n_assert(hardRef);
    n_assert(!hardRef->isvalid());
    n_assert(kernelServer);

    nString name = hardRef->getname();
    n_assert(!name.IsEmpty());

    // \todo calls Lookup twice...
    if (kernelServer->Lookup(name.Get()))
    {
        // object exists, nHardRef can be resolved
        hardRef->resolve();
    }
    else
    {
        // object does not exist, store hashNode, the node
        // may already be linked, in this case, do nothing
        if (!hardRef->strNode.IsLinked())
        {
            this->strList.AddTail(&(hardRef->strNode));
        }
    }
}

//------------------------------------------------------------------------------
/**
    Check if any of the stored nHardRefs refer to this object. If yes, validate
    those nHardRefs and remove from internal list.

    @param  targetObject    the potential nRoot target object
*/
void
nHardRefServer::RegisterTargetObject(nRoot& targetObject)
{
    nString targetName = targetObject.GetFullName();

    // parse list of nHardRef for list of matches
    nStrNode* strNode = (nStrNode*) this->strList.GetHead();
    while (strNode)
    {
        nStrNode* nextNode = (nStrNode*) strNode->GetSucc();

        if (0 == strcmp(targetName.Get(), strNode->GetName()))
        {
            // match found, resolve hardref
            //
            // *** NOTE ***
            // Inside nHardRef::resolve(), the nHardRef object
            // will invalidate its name, which removes the
            // embedded nStrNode object from the nHardRefServer's
            // list, because the name of the nStrNode becomes
            // invalid.

            nHardRef<nRoot>* hardRef = (nHardRef<nRoot>*) strNode->GetPtr();
            n_assert(hardRef);
            hardRef->resolve();
        }

        strNode = nextNode;
    }
}

//------------------------------------------------------------------------------
