#ifndef N_STACK_H
#define N_STACK_H
//------------------------------------------------------------------------------
/**
    A stack template class.

    (C) 2002 RadonLabs GmbH
*/
#ifndef N_TYPES_H
#include "kernel/ntypes.h"
#endif

#ifndef N_LIST_H
#include "util/nlist.h"
#endif

#ifndef N_NODE_H
#include "util/nnode.h"
#endif

//------------------------------------------------------------------------------
template<class TYPE> class nStack 
{
public:
    /// destructor
    ~nStack();
    /// push element onto stack
    void Push(TYPE e);
    /// pop element from stack
    TYPE Pop();
    /// get top element of stack
    const TYPE& Top() const;
    /// check if stack is empty
    bool IsEmpty() const;

private:
    /// a private stack node class
    class nStackNode : public nNode
    {
#ifdef __VC__
        friend nStack;
#else
        friend class nStack;
#endif 

        /// constructor
        nStackNode(const TYPE& e)
        {
            this->elm = e;
        }

        /// get embedded element
        const TYPE& Get() const
        {
            return this->elm;
        }   

        TYPE elm;
    };

    nList l;
};

//------------------------------------------------------------------------------
/**
*/
template<class TYPE>
nStack<TYPE>::~nStack()
{
    // delete any remaining elements on stack
    nStackNode* sn;
    while (sn = (nStackNode*) l.RemHead()) 
    {
        n_delete sn;
    }
}

//------------------------------------------------------------------------------
/**
*/
template<class TYPE>
void 
nStack<TYPE>::Push(TYPE e) 
{
    nStackNode* sn = n_new nStackNode(e);
    l.AddHead(sn);
}

//------------------------------------------------------------------------------
/**
*/
template<class TYPE>
TYPE
nStack<TYPE>::Pop()
{
    nStackNode* sn = (nStackNode*) l.RemHead();
    if (sn) 
    {
        TYPE t = sn->Get();
        n_delete sn;
        return t;
    } 
    else 
    {
        return 0;
    }
}

//------------------------------------------------------------------------------
/**
*/
template<class TYPE>
const TYPE&
nStack<TYPE>::Top() const
{
    nStackNode* sn = (nStackNode*) l.GetHead();
    n_assert(sn);
    return sn->Get();
}

//------------------------------------------------------------------------------
/**
*/
template<class TYPE>
bool
nStack<TYPE>::IsEmpty() const
{
    return l.IsEmpty();
}

//------------------------------------------------------------------------------
#endif

