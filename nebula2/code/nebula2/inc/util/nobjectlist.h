#ifndef N_OBJECTLIST_H
#define N_OBJECTLIST_H
//------------------------------------------------------------------------------
/**
    @class nObjectList
    @ingroup Util
    @brief A doubly linked list for object nodes.

    (C) 2005 RadonLabs GmbH
*/
#include "kernel/ntypes.h"
#include "util/nlist.h"
#include "util/nobjectnode.h"

//------------------------------------------------------------------------------
template <class TYPE>
class nObjectList: public nList
{
public:
    /// return first element of list
    nObjectNode<TYPE>* GetHead() const;
    /// return last element of list
    nObjectNode<TYPE>* GetTail() const;
    /// remove first element of list
    nObjectNode<TYPE>* RemHead();
    /// remove last element of list
    nObjectNode<TYPE>* RemTail();
};

//------------------------------------------------------------------------------
/**
*/
template <class TYPE>
inline
nObjectNode<TYPE>*
nObjectList<TYPE>::GetHead() const
{
    return (nObjectNode<TYPE>*)this->nList::GetHead();
}

//------------------------------------------------------------------------------
/**
*/
template <class TYPE>
inline
nObjectNode<TYPE>*
nObjectList<TYPE>::GetTail() const
{
    return (nObjectNode<TYPE>*)this->nList::GetTail();
}

//------------------------------------------------------------------------------
/**
*/
template <class TYPE>
inline
nObjectNode<TYPE>*
nObjectList<TYPE>::RemHead()
{
    return (nObjectNode<TYPE>*)this->nList::RemHead();
}

//------------------------------------------------------------------------------
/**
*/
template <class TYPE>
inline
nObjectNode<TYPE>*
nObjectList<TYPE>::RemTail()
{
    return (nObjectNode<TYPE>*)this->nList::RemTail();
}

//------------------------------------------------------------------------------
#endif
