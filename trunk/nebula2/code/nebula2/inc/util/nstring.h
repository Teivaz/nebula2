#ifndef N_STRING_H
#define N_STRING_H
//------------------------------------------------------------------------------
/**
    @class nString
    @ingroup NebulaDataTypes

    A simple Nebula string class. Very handy when strings must be stored or
    manipulated. Note that Nebula interfaces hand strings around as
    char pointers, not nString objects.

    The current implementation does not allocate extra memory if the
    string has less then 13 characters.

    (C) 2001 RadonLabs GmbH
*/
#include <ctype.h>
#include <string.h>

#include "kernel/ntypes.h"
#include "util/narray.h"

//------------------------------------------------------------------------------
class nString
{
public:
    /// constructor
    nString();
    /// constructor 1
    nString(const char*str);
    /// constructor with int value
    nString(int val);
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
    /// Is `a' equal to `b'?
    friend bool operator == (const nString& a, const nString& b);
    /// Is `a' inequal to `b'?
    friend bool operator != (const nString& a, const nString& b);
    /// Subscript operator (read only).
    const char operator [] (int i) const;
    /// Subscript operator (writeable).
    char& operator [] (int i);
    /// set as char ptr, with explicit length
    void Set(const char* ptr, int length);
    /// set as char ptr
    void Set(const char* str);
    /// set int value
    void Set(int val);
    /// get string as char ptr
    const char* Get() const;
    /// return length of string
    int Length() const;
    /// return true if string object is empty
    bool IsEmpty() const;
    /// append string pointer
    void Append(const char* str);
    /// append string
    void Append(const nString& str);
    /// convert string to lower case
    void ToLower();
    /// get first token (this will destroy the string)
    const char* GetFirstToken(const char* whiteSpace);
    /// get next token (this will destroy the string)
    const char* GetNextToken(const char* whiteSpace);
    /// tokenize string into a provided nString array
    int Tokenize(const char* whiteSpace, nArray<nString>& tokens) const;
    /// extract substring
    nString ExtractRange(int from, int to) const;
    /// terminate string at first occurence of character in set
    void Strip(const char* charSet);

protected:
    /// copy contents
    void Copy(const nString& src);
    /// delete contents
    void Delete();

    enum
    {
        LOCALSTRINGSIZE = 14,
    };
    char* string;
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
nString::Set(const char* str, int length)
{
    this->Delete();
    if (str)
    {
        this->strLen = length;
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
        for (i = 0; i < strLen; i++)
        {
            *ptr++ = *str++;
        }
        *ptr = 0;
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nString::Set(const char* str)
{
    int len = 0;
    if (str)
    {
        len = strlen(str);
    }
    this->Set(str, len);
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nString::Set(int val)
{
    char buf[128];
    sprintf(buf, "%d", val);
    this->Set(buf);
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
nString::nString(int val) :
    string(0)
{
    this->Set(val);
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
    if (&rhs != this)
    {
        this->Delete();
        this->Copy(rhs);
    }
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
void
nString::Append(const nString& str)
{
    this->Append(str.Get());
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
operator == (const nString& a, const nString& b)
{
    return strcmp(a.Get(), b.Get()) == 0;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
operator != (const nString& a, const nString& b)
{
    return strcmp(a.Get(), b.Get()) != 0;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char
nString::operator [] (int i) const
{
    n_assert(0 <= i && i <= strLen - 1);
   if (string != 0)
    {
        return string[i];
    }

    return localString[i];
}

//------------------------------------------------------------------------------
/**
*/
inline
char&
nString::operator [] (int i)
{
    n_assert(0 <= i && i <= strLen - 1);
    if (string != 0)
    {
        return string[i];
    }

    return localString[i];
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
        char* ptr = (char*) str;
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

    ATTENTION: if somebody else calls strtok() while GetFirstToken()/
    GetNextToken() is underway, everything will break apart!
    Check out the Tokenize() method for a better alternative.

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

    ATTENTION: if somebody else calls strtok() while GetFirstToken()/
    GetNextToken() is underway, everything will break apart!
    Check out the Tokenize() method for a better alternative.

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
/**
    Tokenize the string into a provided nString array. Returns the number
    of tokens. This method is recommended over GetFirstToken()/GetNextToken(),
    since it is atomic. This nString object will not be destroyed
    (as is the case with GetFirstToken()/GetNextToken().

    @param  whiteSpace      [in] a string containing the whitespace characters
    @param  tokens          [out] nArray<nString> where tokens will be appended
    @return                 number of tokens found
*/
inline
int
nString::Tokenize(const char* whiteSpace, nArray<nString>& tokens) const
{
    int numTokens = 0;

    // create a temporary string, which will be destroyed during the operation
    nString str(*this);
    char* ptr = (char*) str.Get();
    const char* token;
    while (0 != (token = strtok(ptr, whiteSpace)))
    {
        tokens.Append(nString(token));
        ptr = 0;
        numTokens++;
    }
    return numTokens;
}

//------------------------------------------------------------------------------
/**
    Extract sub string.
*/
inline
nString
nString::ExtractRange(int from, int numChars) const
{
    n_assert(from <= this->strLen);
    n_assert((from + numChars) <= this->strLen);
    const char* str = this->Get();
    nString newString;
    newString.Set(&(str[from]), numChars);
    return newString;
}

//------------------------------------------------------------------------------
/**
    Terminates the string at the first occurance of one of the characters
    in charSet.
*/
inline
void
nString::Strip(const char* charSet)
{
    n_assert(charSet);
    char* str = (char*) this->Get();
    char* ptr = strpbrk(str, charSet);
    if (ptr)
    {
        *ptr = 0;
    }
}

//------------------------------------------------------------------------------
#endif

