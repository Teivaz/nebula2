#ifndef N_AXISFILTER_H
#define N_AXISFILTER_H
//------------------------------------------------------------------------------
/**
    @class nAxisFilter
    @ingroup Input

    @brief A simple axis filter object which filters input values over the last
    few frames.

    (C) 2002 RadonLabs GmbH
*/
#ifndef N_TYPES_H
#include "kernel/ntypes.h"
#endif

//------------------------------------------------------------------------------
class nAxisFilter
{
public:
    /// constructor
    nAxisFilter();
    /// set filter array size
    void SetSize(int historySize);
    /// push new value
    void PushValue(float val);
    /// get filtered value
    float GetFilteredValue() const;
    /// get a raw unfiltered value
    float GetRawValue(int index) const;

private:
    enum
    {
        MAXHISTORYSIZE = 8,
    };

    int size;
    float oneOverSize;
    float array[MAXHISTORYSIZE];
};

//------------------------------------------------------------------------------
/**
*/
inline
void
nAxisFilter::SetSize(int historySize)
{
    n_assert(historySize > 1);
    if (historySize > MAXHISTORYSIZE)
    {
        historySize = MAXHISTORYSIZE;
    }
    this->size = historySize;
    this->oneOverSize = (1.0f / float(this->size));

    int i;
    for (i = 0; i < this->size; i++)
    {
        this->array[i] = 0.0f;
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
nAxisFilter::nAxisFilter()
{
    this->SetSize(4);
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nAxisFilter::PushValue(float val)
{
    // move existing entries
    int i;
    for (i = (this->size - 1); i > 0; --i)
    {
        this->array[i] = this->array[i - 1];
    }

    // write new value to beginning of array
    this->array[0] = val;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nAxisFilter::GetFilteredValue() const
{
    float val = 0.0f;
    int i;
    for (i = 0; i < size; i++)
    {
        val += this->array[i];
    }
    val *= this->oneOverSize;
    return val;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nAxisFilter::GetRawValue(int index) const
{
    n_assert((index >= 0) && (index < this->size));
    return this->array[index];
}

//------------------------------------------------------------------------------
#endif

