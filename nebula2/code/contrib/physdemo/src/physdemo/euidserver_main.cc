//------------------------------------------------------------------------------
//  (C) 2003    Megan Fox
//------------------------------------------------------------------------------
#include "physdemo/euidserver.h"

nNebulaScriptClass(eUIDServer, "nroot");

//------------------------------------------------------------------------------
/**
*/
eUIDServer::eUIDServer(): nextUniqueID(0)
{
}

//------------------------------------------------------------------------------
/**
*/
eUIDServer::~eUIDServer()
{
    // Clear the released ID list
    nNode *node = releasedUIDList.GetHead();

    while(node)
    {
        node->Remove();

        if (node->GetPtr())
            delete(node->GetPtr());
        delete(node);

        node = releasedUIDList.GetHead();
    }
}

//------------------------------------------------------------------------------
/**
    Gets a new unique ID

    @return the requested unique ID
*/
int eUIDServer::GetNewID()
{
    int NewID;

    nNode *listNode = releasedUIDList.GetHead();

    // if releasedUIDList has any released UIDs free, use them before creating a new one
    if (listNode)
    {
        NewID = *((int *)listNode->GetPtr());

        listNode->Remove();
        delete(listNode->GetPtr());
        delete(listNode);

        return NewID;
    }
    // otherwise, create a new UID and issue it.
    else
    {
        NewID = nextUniqueID;
        nextUniqueID++;

        return(NewID);
    }
}

//------------------------------------------------------------------------------
/**
    Release an unused ID.  Allows the unique ID to be requested again by GetNewID().

    @param OldID    the old unique ID to release.
*/
void eUIDServer::ReleaseID(int OldID)
{
    nNode *removedIDNode = new nNode;

    removedIDNode->SetPtr(new int(OldID));

    releasedUIDList.AddTail(removedIDNode);
}
