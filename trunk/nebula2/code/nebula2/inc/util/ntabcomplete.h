#ifndef N_TABCOMPLETE_H
#define N_TABCOMPLETE_H
//------------------------------------------------------------------------------
/**
    @class nTabComplete
    @ingroup Util

    @brief A utility class for Tab completion.

    (C) 2004 RadonLabs GmbH
*/
#include "kernel/ntypes.h"
#include "util/narray.h"
#include "util/nstring.h"

//------------------------------------------------------------------------------
class nTabComplete
{
public:
    /// constructor
    nTabComplete();
    /// destructor
    ~nTabComplete();
    /// set current search string, resets tab index
    void SetSearchString(const nString& str);
    /// get current search string
    const nString& GetSearchString() const;
    /// reset candidate array
    void ClearCandidates();
    /// append a candidate string
    void AppendCandidate(const nString& str);
    /// get next match, wraps around
    nString GetNextMatch();

private:
    /// check if a search string and a candidate string match
    bool Match(const nString& search, const nString& candidate) const;

    nString searchString;
    nArray<nString> candidates;
    int curIndex;
};

//------------------------------------------------------------------------------
/**
*/
inline
nTabComplete::nTabComplete() :
    curIndex(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
nTabComplete::~nTabComplete()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nTabComplete::SetSearchString(const nString& str)
{
    this->searchString = str;
    this->curIndex = 0;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
nTabComplete::GetSearchString() const
{
    return this->searchString;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nTabComplete::ClearCandidates()
{
    this->candidates.Clear();
    this->curIndex = 0;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nTabComplete::AppendCandidate(const nString& str)
{
    n_assert(!str.IsEmpty());
    this->candidates.Append(str);
}

//------------------------------------------------------------------------------
/**
    Check if the 2 strings match.
*/
inline
bool
nTabComplete::Match(const nString& search, const nString& candidate) const
{
    if (search.Length() == 0)
    {
        return true;
    }
    else if (search.Length() <= candidate.Length())
    {
        return (0 == strncmp(search.Get(), candidate.Get(), search.Length()));
    }
    return false;
}

//------------------------------------------------------------------------------
/**
    Find the next entry in the candidates array which matches the search
    string. Will wrap around.
*/
inline
nString
nTabComplete::GetNextMatch()
{
    // from index to end of array
    int index;
    for (index = this->curIndex; index < this->candidates.Size(); index++)
    {
        if (this->Match(this->searchString, this->candidates[index]))
        {
            this->curIndex = index + 1;
            return this->candidates[index];
        }
    }

    // wrap around, and from index to curIndex - 1
    for (index = 0; index < this->curIndex; index++)
    {
        if (this->Match(this->searchString, this->candidates[index]))
        {
            this->curIndex = index + 1;
            return this->candidates[index];
        }
    }

    // no match?
    return this->searchString;
}

//------------------------------------------------------------------------------
#endif
