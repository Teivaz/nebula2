#ifndef N_OBJECTNODE_H
#define N_OBJECTNODE_H
//------------------------------------------------------------------------------
/**
    @class nObjectNode
    @ingroup Util
    @brief A node in a nObjectList.

    (C) 2005 RadonLabs GmbH
*/
#include "kernel/ntypes.h"
#include "util/nnode.h"

//------------------------------------------------------------------------------
template <class TYPE> class nObjectNode: public nNode
{
public:
    /// default constructor
    nObjectNode();
    /// constructor providing custom data pointer
    nObjectNode(void* ptr);
    /// constructor providing an object
    nObjectNode(const TYPE& object);
    /// constructor providing custom data pointer and object
    nObjectNode(const TYPE& object, void* ptr);
    /// set the object of this node
    void SetObject(const TYPE& object);
    /// get the object of this node
    TYPE& GetObject();
    /// get next node in list
    nObjectNode* GetSucc() const;
    /// get previous node in list
    nObjectNode* GetPred() const;

private:
    TYPE object;
};

//------------------------------------------------------------------------------
/**
*/
template<class TYPE>
inline
nObjectNode<TYPE>::nObjectNode()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
template<class TYPE>
inline
nObjectNode<TYPE>::nObjectNode(void* ptr) :
    nNode(ptr)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
template<class TYPE>
inline
nObjectNode<TYPE>::nObjectNode(const TYPE& o) :
    object(o)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
template<class TYPE>
inline
nObjectNode<TYPE>::nObjectNode(const TYPE& o, void* ptr) :
    nNode(ptr),
    object(o)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
template<class TYPE>
inline
void
nObjectNode<TYPE>::SetObject(const TYPE& object)
{
    this->object = object;
}

//------------------------------------------------------------------------------
/**
*/
template<class TYPE>
inline
TYPE&
nObjectNode<TYPE>::GetObject()
{
    return this->object;
}

//------------------------------------------------------------------------------
/**
*/
template<class TYPE>
inline
nObjectNode<TYPE>*
nObjectNode<TYPE>::GetSucc() const
{
    return (nObjectNode<TYPE>*)nNode::GetSucc();
}

//------------------------------------------------------------------------------
/**
*/
template<class TYPE>
inline
nObjectNode<TYPE>*
nObjectNode<TYPE>::GetPred() const
{
    return (nObjectNode<TYPE>*)nNode::GetPred();
}

//------------------------------------------------------------------------------
#endif
