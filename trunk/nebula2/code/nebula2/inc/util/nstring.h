#ifndef N_STRING_H
#define N_STRING_H
//------------------------------------------------------------------------------
/**
    A simple Nebula string class. Very handy when strings must be stored or
    manipulated. Note that Nebula interfaces hand strings around as
    char pointers, not nString objects.

    The current implementation does not allocate extra memory if the
    string has less then 13 characters.

    (C) 2001 RadonLabs GmbH
*/
#include <ctype.h>
#include <string.h>

#ifndef N_TYPES_H
#include "kernel/ntypes.h"
#endif

//------------------------------------------------------------------------------
class nString
{
public:
    /// constructor
    nString();
    /// constructor 1
    nString(const char*str);
    /// copy constructor
    nString(const nString& rhs);
    /// destructor
    ~nString();
    /// = operator
    nString& operator=(const nString& rhs);
    /// = operator with string
    nString& operator=(const char* rhs);
    /// += operator with char*
    nString& operator+=(const char* rhs);
    /// += operator with string
    nString& operator+=(const nString& rhs);
    /// equality operator
    bool operator==(const nString& rhs) const;
    /// equality operator
    bool operator==(const char* str) const;
    /// set as char ptr
    void Set(const char* str);
    /// get string as char ptr
    const char* Get() const;
    /// return length of string
    int Length() const;
    /// return true if string object is empty
    bool IsEmpty() const;
    /// append string
    void Append(const char* str);
    /// convert string to lower case
    void ToLower();
    /// get first token (this will destroy the string)
    const char* GetFirstToken(const char* whiteSpace);
    /// get next token (this will destroy the string)
    const char* GetNextToken(const char* whiteSpace);

protected:
    /// copy contents
    void Copy(const nString& src);
    /// delete contents
    void Delete();

    enum
    {
        LOCALSTRINGSIZE = 14,
    };
    const char* string;
    char localString[LOCALSTRINGSIZE];
    ushort strLen;
};

//------------------------------------------------------------------------------
/**
*/
inline
nString::nString() :
    string(0)
{
    this->localString[0] = 0;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nString::Delete()
{
    if (this->string)
    {
        n_free((void*) this->string);
        this->string = 0;
    }
    this->localString[0] = 0;
}

//------------------------------------------------------------------------------
/**
*/
inline
nString::~nString()
{
    this->Delete();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nString::Set(const char* str)
{
    this->Delete();
    if (str)
    {
        this->strLen = strlen(str);
        char* ptr = this->localString;
        if (strLen >= LOCALSTRINGSIZE)
        {
            ptr = (char*) n_malloc(strLen + 1);
            this->string = ptr;
        }
        else
        {
            ptr = this->localString;
        }
        int i;
        for (i = 0; i <= strLen; i++)
        {
            *ptr++ = *str++;
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nString::Copy(const nString& src)
{
    n_assert(0 == this->string);
    const char* str = src.Get();
    if (str)
    {
        this->Set(str);
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
nString::nString(const char* str) :
    string(0)
{
    n_assert(str);
    this->Set(str);
}

//------------------------------------------------------------------------------
/**
*/
inline
nString::nString(const nString& rhs) :
    string(0)
{
    this->Copy(rhs);
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nString::Get() const
{
    if (this->string)
    {
        return this->string;
    }
    else if (this->localString[0])
    {
        return this->localString;
    }
    else
    {
        return "";
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
nString&
nString::operator=(const nString& rhs)
{
    this->Delete();
    this->Copy(rhs);
    return *this;
}

//------------------------------------------------------------------------------
/**
*/
inline
nString&
nString::operator=(const char* rhs)
{
    this->Set(rhs);
    return *this;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nString::Append(const char* str)
{
    ushort rlen = strlen(str);
    ushort tlen = this->strLen + rlen;
    if (this->string)
    {
        char* ptr = (char*) n_malloc(tlen + 1);
        strcpy(ptr, this->string);
        strcat(ptr, str);
        n_free((void*) this->string);
        this->string = ptr;
        this->strLen = tlen;
    }
    else if (this->localString[0])
    {
        if (tlen >= LOCALSTRINGSIZE)
        {
            char* ptr = (char*) n_malloc(tlen + 1);
            strcpy(ptr, this->localString);
            strcat(ptr, str);
            this->localString[0] = 0;
            this->string = ptr;
        }
        else
        {
            strcat(this->localString, str);
        }
        this->strLen = tlen;
    }
    else
        this->Set(str);    
}

//------------------------------------------------------------------------------
/**
*/
inline
nString&
nString::operator+=(const char* rhs)
{
    this->Append(rhs);
    return *this;
}

//------------------------------------------------------------------------------
/**
*/
inline
nString&
nString::operator+=(const nString& rhs)
{
    this->Append(rhs.Get());       
    return *this;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool 
nString::operator==(const nString& rhs) const
{
    if (strcmp(this->Get(), rhs.Get()) == 0) return true;
    else                                     return false;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nString::operator==(const char* str) const
{
    if (strcmp(this->Get(), str) == 0) return true;
    else                               return false;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nString::Length() const
{
    return this->strLen;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nString::IsEmpty() const
{
    if (this->string && (this->string[0] != 0))
    {
        return false;
    }
    else if (this->localString[0] != 0)
    {
        return false;
    }
    return true;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nString::ToLower()
{
    char* str = (char*) (this->string ? this->string : this->localString);
    if (str)
    {
        char c;
        char* ptr = (char*) this->string;
        while ((c = *ptr))
        {
            *ptr++ = tolower(c);
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
static
inline
nString
operator+(const nString& s0, const nString& s1)
{
    nString newString = s0;
    newString.Append(s1.Get());
    return newString;
}

//------------------------------------------------------------------------------
/**
    Get the first token in the string, delimited by one or more of the
    character in 'whiteSpace'. This simply invokes the strtok() function
    internally, and will destroy the contained string. After calling
    GetFirstToken(), invoke GetNextToken() until 0 returns.

    @param  whiteSpace  string containing white space characters
*/
inline
const char*
nString::GetFirstToken(const char* whiteSpace)
{
    n_assert(whiteSpace);
    return strtok((char*) this->Get(), whiteSpace);
}

//------------------------------------------------------------------------------
/**
    Get the next token in the string. Call this after a GetFirstToken()
    or GetNextToken(). Returns 0 if no more tokens in string. This method
    will destroy the original string.

    @param  whiteSpace  string containing whitespace characters
*/
inline
const char*
nString::GetNextToken(const char* whiteSpace)
{
    n_assert(whiteSpace);
    return strtok(0, whiteSpace);
}

//------------------------------------------------------------------------------
#endif

