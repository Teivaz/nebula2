#ifndef N_STRLIST_H
#define N_STRLIST_H
//------------------------------------------------------------------------------
/**
    A doubly linked list for named nodes with slow linear search.

    (C) 2002 RadonLabs GmbH
*/
#include <string.h>

#ifndef N_TYPES_H
#include "kernel/ntypes.h"
#endif

#ifndef N_LIST_H
#include "util/nlist.h"
#endif

#ifndef N_STRNODE_H
#include "util/nstrnode.h"
#endif

//------------------------------------------------------------------------------
class nStrList: public nList 
{
public:	
    /// return first element of list
    nStrNode* GetHead() const;
    /// return last element of list
    nStrNode* GetTail() const;
    /// remove first element of list
    nStrNode* RemHead();
    /// remove last element of list
    nStrNode* RemTail();
    /// search for named element (slow)
    nStrNode* Find(const char* str) const;
};

//------------------------------------------------------------------------------
/**
*/
inline
nStrNode*
nStrList::GetHead() const
{
    return (nStrNode *) this->nList::GetHead();
}

//------------------------------------------------------------------------------
/**
*/
inline
nStrNode*
nStrList::GetTail() const
{
    return (nStrNode *) this->nList::GetTail();
}

//------------------------------------------------------------------------------
/**
*/
inline
nStrNode*
nStrList::RemHead()
{
    return (nStrNode *) this->nList::RemHead();
}

//------------------------------------------------------------------------------
/**
*/
inline
nStrNode*
nStrList::RemTail()
{
    return (nStrNode *) this->nList::RemTail();
}

//------------------------------------------------------------------------------
/**
*/
inline
nStrNode*
nStrList::Find(const char* str) const
{
    nStrNode* n;
    for (n = this->GetHead(); n; n = n->GetSucc()) 
    {
        const char* nodeName = n->GetName();
        n_assert(nodeName);
        if (strcmp(str, nodeName) == 0) 
        {
            return n;
        }
    }
    return 0;
};

//------------------------------------------------------------------------------
#endif
