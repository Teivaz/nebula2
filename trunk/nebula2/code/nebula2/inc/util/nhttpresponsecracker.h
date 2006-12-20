#ifndef N_HTTPRESPONSECRACKER_H
#define N_HTTPRESPONSECRACKER_H
#ifndef __WIN32__
#error "nHttpResponseCracker not implemented!"
#endif

#if defined(__WIN32__) || defined(DOXYGEN)
//------------------------------------------------------------------------------
/**
    @class nHttpResponseCracker
    @ingroup Http

    Crack a http server response string of the form

    key=value\n
    key=value\n
    key=...

    (C) 2004 RadonLabs GmbH
*/
#include "kernel/ntypes.h"
#include "util/nstring.h"
#include "util/narray.h"

//------------------------------------------------------------------------------
class nHttpResponseCracker
{
public:
    /// constructor
    nHttpResponseCracker(const nString& response);
    /// return true if attribute found in response
    bool HasKey(const nString& key) const;
    /// return attribute value
    const nString& GetValue(const nString& key) const;

private:
    nArray<nString> keys;
    nArray<nString> values;
};

//------------------------------------------------------------------------------
/**
*/
inline
nHttpResponseCracker::nHttpResponseCracker(const nString& response)
{
    // split string into key/value pairs
    nArray<nString> tokens;
    response.Tokenize("\r\n; ", tokens);
    int i;
    int num = tokens.Size();
    for (i = 0; i < num; i++)
    {
        // split key/value pairs into key and value
        nArray<nString> keyTokens;
        tokens[i].Tokenize("=", keyTokens);
        if (2 == keyTokens.Size())
        {
            this->keys.Append(keyTokens[0]);
            this->values.Append(keyTokens[1]);
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nHttpResponseCracker::HasKey(const nString& key) const
{
    n_assert(!key.IsEmpty());
    return (0 != this->keys.Find(key));
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
nHttpResponseCracker::GetValue(const nString& key) const
{
    n_assert(!key.IsEmpty());
    int valueIndex = this->keys.FindIndex(key);
    n_assert(valueIndex != -1);
    return this->values[valueIndex];
}

//------------------------------------------------------------------------------
#endif /* __WIN32__ or DOXYGEN */
#endif
