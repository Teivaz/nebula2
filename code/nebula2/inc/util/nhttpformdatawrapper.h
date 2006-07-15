#ifndef N_HTTPFORMDATAWRAPPER_H
#define N_HTTPFORMDATAWRAPPER_H
#ifndef __WIN32__
#error "nHttpFormDataWrapper not implemented!"
#endif

#if defined(__WIN32__) || defined(DOXYGEN)
//------------------------------------------------------------------------------
/**
    @class nHttpFormDataWrapper
    @ingroup Http

    Constructs and sends a HTTP POST in format multipart/form-data.
    
    (C) 2004 RadonLabs GmbH
*/
#include "util/nhttpsession.h"
#include "util/nstring.h"
#include "util/narray.h"

//------------------------------------------------------------------------------
class nHttpFormDataWrapper
{
public:
    /// constructor
    nHttpFormDataWrapper();
    /// destructor
    ~nHttpFormDataWrapper();
    /// set the http object name (the POST target)
    void SetObject(const nString& obj);
    /// get http object name
    const nString& GetObject() const;
    /// begin composing the POST
    bool Begin(nHttpSession* httpSession);
    /// add a key/value pair
    void AddAttr(const nString& key, const nString& value);
    /// finish composing
    void End();
    /// send the composed HTTP POST request and returns respone from server
    bool Send(nString& result);
    /// set cookie string
    void SetCookie(const nString& s);
    /// get cookie string
    const nString& GetCookie() const;

private:
    /// builds the requestHeaders member
    void BuildRequestHeaders();
    /// builds the form data string, which contains the attributes
    void BuildFormData();

    static const char* Boundary;
    nHttpSession* httpSession;
    nArray<nString> keys;
    nArray<nString> values;
    nString requestHeaders;
    nString formData;
    nString objectName;
    nString cookie;
};

//------------------------------------------------------------------------------
/**
*/
inline
void
nHttpFormDataWrapper::SetCookie(const nString& s)
{
    this->cookie = s;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
nHttpFormDataWrapper::GetCookie() const
{
    return this->cookie;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nHttpFormDataWrapper::SetObject(const nString& obj)
{
    this->objectName = obj;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
nHttpFormDataWrapper::GetObject() const
{
    return this->objectName;
}

//------------------------------------------------------------------------------
#endif /* __WIN32__ or DOXYGEN */
#endif
