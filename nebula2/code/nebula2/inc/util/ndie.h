#ifndef N_DIE_H
#define N_DIE_H
//------------------------------------------------------------------------------
/**
    @class nDie
    @ingroup Util

    Implements an N-sided die.

    (C) 2004 RadonLabs GmbH
*/
#include <stdlib.h>
#include "kernel/ntypes.h"

//------------------------------------------------------------------------------
class nDie
{
public:
    /// Construct.
    nDie();

    /// Initialize random number generation with `v'.
    void Initialize(int v);
    /// "Roll" the die. Make result available in `GetValue()'.
    void Update();

    /// Set number of sides to `v'.
    void SetSides(int v);
    /// Number of sides.
    int GetSides() const;

    /// Current value.
    int GetCurrentValue() const;
    /// Smallest available value.
    int GetSmallestValue() const;
    /// Greatest available value.
    int GetGreatestValue() const;

private:
    // Data
    int numSides;
    int value;
};

//------------------------------------------------------------------------------
inline
nDie::nDie() :
    numSides(6),
    value(1)
{
}

//------------------------------------------------------------------------------
inline
void
nDie::Initialize(int v)
{
    // Feed random number generator.
    srand(v);
}

//------------------------------------------------------------------------------
inline
void
nDie::Update()
{
    value = (rand() % numSides) + 1;
}

//------------------------------------------------------------------------------
inline
void
nDie::SetSides(int v)
{
    n_assert(v > 0);
    numSides = v;
}

//------------------------------------------------------------------------------
inline
int
nDie::GetSides() const
{
    n_assert(numSides > 0);
    return numSides;
}

//------------------------------------------------------------------------------
inline
int
nDie::GetCurrentValue() const
{
    return value;
}

//------------------------------------------------------------------------------
inline
int
nDie::GetSmallestValue() const
{
    return 1;
}

//------------------------------------------------------------------------------
inline
int
nDie::GetGreatestValue() const
{
    return numSides;
}

//------------------------------------------------------------------------------
#endif

