#ifndef N_KEYANIMATOR_H
#define N_KEYANIMATOR_H
//------------------------------------------------------------------------------
/**
    A common base class for animator classes which need to sample
    vectors from key arrays.
    
    (C) 2004 RadonLabs GmbH
*/
#include "scene/nanimator.h"

//------------------------------------------------------------------------------
class nKeyAnimator : public nAnimator
{
public:
    /// constructor
    nKeyAnimator();

protected:
    /// a key class with a time and a key value
    class Key
    {
    public:
        /// default constructor
        Key();
        /// constructor
        Key(float t, const vector4& v);

        float time;
        vector4 value;
    };
    /// get an interpolated key
    bool SampleKey(float time, const nArray<Key>& keyArray, vector4& result);
};

//------------------------------------------------------------------------------
/**
*/
inline
nKeyAnimator::Key::Key()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
nKeyAnimator::Key::Key(float t, const vector4& v) :
    time(t),
    value(v)
{
    // empty
}

//------------------------------------------------------------------------------
#endif
