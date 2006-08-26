#ifndef N_ANIMSTATEARRAY_H
#define N_ANIMSTATEARRAY_H
//------------------------------------------------------------------------------
/**
    @class nAnimStateArray
    @ingroup Anim2

    @brief An anim state array holds several mutually exclusive nAnimState
    objects and allows switching between them.

    (C) 2003 RadonLabs GmbH
*/
#include "anim2/nanimstate.h"
#include "util/narray.h"

//------------------------------------------------------------------------------
class nAnimStateArray
{
public:
    /// constructor
    nAnimStateArray();
    /// destructor
    ~nAnimStateArray();
    /// begin adding anim states
    void Begin(int num);
    /// set a new anim state
    void SetState(int index, const nAnimState& state);
    /// finish adding anim states
    void End();
    /// get number of animation states
    int GetNumStates() const;
    /// get an anim state object at given index
    nAnimState& GetStateAt(int index) const;
    /// find state by name
    nAnimState* FindState(const nString& n) const;
    /// find a state index by name
    int FindStateIndex(const nString& n) const;

private:
    nArray<nAnimState> stateArray;
};

//------------------------------------------------------------------------------
/**
*/
inline
nAnimStateArray::nAnimStateArray() :
    stateArray(0, 0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
nAnimStateArray::~nAnimStateArray()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Begin adding states to the anim state array.

    @param  num     number of anim states
*/
inline
void
nAnimStateArray::Begin(int num)
{
    this->stateArray.SetFixedSize(num);
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nAnimStateArray::SetState(int index, const nAnimState& state)
{
    this->stateArray[index] = state;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nAnimStateArray::End()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nAnimStateArray::GetNumStates() const
{
    return this->stateArray.Size();
}

//------------------------------------------------------------------------------
/**
*/
inline
nAnimState&
nAnimStateArray::GetStateAt(int index) const
{
    return this->stateArray[index];
}

//------------------------------------------------------------------------------
/**
*/
inline
nAnimState*
nAnimStateArray::FindState(const nString& n) const
{
    int i;
    int num = this->stateArray.Size();
    for (i = 0; i < num; i++)
    {
        nAnimState& state = this->stateArray[i];
        if (state.GetName() == n)
        {
            return &state;
        }
    }
    return 0;
}

//------------------------------------------------------------------------------
/**
    Finds a state index by name. Returns -1 if state not found.
*/
inline
int
nAnimStateArray::FindStateIndex(const nString& n) const
{
    int i;
    int num = this->stateArray.Size();
    for (i = 0; i < num; i++)
    {
        nAnimState& state = this->stateArray[i];
        if (state.GetName() == n)
        {
            return i;
        }
    }
    return -1;
}

//------------------------------------------------------------------------------
#endif
