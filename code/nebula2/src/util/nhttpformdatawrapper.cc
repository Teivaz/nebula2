//------------------------------------------------------------------------------
//  nhttpformdatawrapper.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#if defined(__WIN32__) || defined(DOXYGEN)
#include "util/nhttpformdatawrapper.h"

const char* nHttpFormDataWrapper::Boundary = "---------------------------299213203213844";

//------------------------------------------------------------------------------
/**
*/
nHttpFormDataWrapper::nHttpFormDataWrapper() :
    httpSession(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nHttpFormDataWrapper::~nHttpFormDataWrapper()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Begin constructing a multipart/form-data message.
*/
bool
nHttpFormDataWrapper::Begin(nHttpSession* session)
{
    n_assert(session);
    n_assert(session->IsConnected());
    this->httpSession = session;
    this->keys.Clear();
    this->values.Clear();
    return true;
}

//------------------------------------------------------------------------------
/**
    Adds a new key/value pair to the form data
*/
void
nHttpFormDataWrapper::AddAttr(const nString& key, const nString& value)
{    
    this->keys.Append(key);
    this->values.Append(value);
}

//------------------------------------------------------------------------------
/**
    Finishes composing. Use Send() afterwards to fire the POST request.
*/
void
nHttpFormDataWrapper::End()
{
    this->BuildRequestHeaders();
    this->BuildFormData();
}

//------------------------------------------------------------------------------
/**
    This sends the actual POST request.
*/
bool
nHttpFormDataWrapper::Send(nString& response)
{
    n_assert(this->httpSession);
    n_assert(!this->requestHeaders.IsEmpty());
    n_assert(!this->objectName.IsEmpty());
    n_assert(!this->formData.IsEmpty());

    bool res = this->httpSession->SendPostRequest(this->objectName, this->requestHeaders, this->formData, response);
    return res;
}

//------------------------------------------------------------------------------
/**
    This constructs the additional request headers for the POST request.
*/
void
nHttpFormDataWrapper::BuildRequestHeaders()
{
    this->requestHeaders.Clear();
    this->requestHeaders.Append("Content-type: multipart/form-data; boundary=");
    this->requestHeaders.Append(Boundary);
    this->requestHeaders.Append("\r\n");
    this->requestHeaders.Append("Connection: keep-alive");
    this->requestHeaders.Append("\r\n");
    /*
    if (!this->cookie.IsEmpty())
    {
        this->requestHeaders.Append("Cookie: ");
        this->requestHeaders.Append(this->cookie);
        this->requestHeaders.Append("\r\n");
    }
    */
}

//------------------------------------------------------------------------------
/**
    This builds the actual form data string from the attached attributes.
*/
void
nHttpFormDataWrapper::BuildFormData()
{
    n_assert(this->keys.Size() > 0);
    n_assert(this->keys.Size() == this->values.Size());

    this->formData.Clear();
    int i;
    int num = this->keys.Size();
    for (i = 0; i < num; i++)
    {
        this->formData.Append("--");
        this->formData.Append(Boundary);
        this->formData.Append("\r\n");
        this->formData.Append("Content-Disposition: form-data; name=\"");
        this->formData.Append(this->keys[i]);
        this->formData.Append("\"\r\n\r\n");
        this->formData.Append(this->values[i]);
        this->formData.Append("\r\n");
    }
    this->formData.Append("--");
    this->formData.Append(Boundary);
    this->formData.Append("--\r\n");

    // encode to UTF-8
    this->formData.ANSItoUTF8();
}
#endif /* __WIN32__ or DOXYGEN */

