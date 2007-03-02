#ifndef N_FONTDESC_H
#define N_FONTDESC_H
//------------------------------------------------------------------------------
/**
    @class nFontDesc
    @ingroup Gfx2

    A font description object used to create a nFont object.

    (C) 2003 RadonLabs GmbH
*/
#include "kernel/ntypes.h"
#include "util/nstring.h"

//------------------------------------------------------------------------------
class nFontDesc
{
public:
    /// font weights
    enum Weight
    {
        Thin,
        Light,
        Normal,
        Bold,
        ExtraBold,
        InvalidWeight,
    };

    /// constructor
    nFontDesc();
    /// destructor
    ~nFontDesc();
    /// equality operator
    bool operator==(const nFontDesc& rhs) const;
    /// set font height
    void SetHeight(int h);
    /// get font height
    int GetHeight() const;
    /// set font weight
    void SetWeight(Weight w);
    /// get font weight
    Weight GetWeight() const;
    /// set italic flag
    void SetItalic(bool b);
    /// get italic flag
    bool GetItalic() const;
    /// set underline flag
    void SetUnderline(bool b);
    /// get underline flag
    bool GetUnderline() const;
    /// set antialiased flag
    void SetAntiAliased(bool b);
    /// get antialiased flag
    bool GetAntiAliased() const;
    /// set typeface name
    void SetTypeFace(const char* n);
    /// get typeface name
    const char* GetTypeFace() const;
    /// set optional font file name
    void SetFilename(const char* n);
    /// get optional font file name
    const char* GetFilename() const;

    /// convert render flag string to enum
    static Weight StringToWeight(const nString& str);
    /// convert render flag enum to string
    static nString WeightToString(Weight renderFlag);

private:
    int height;
    Weight weight;
    bool italic;
    bool underline;
    bool antiAliased;
    nString typeFace;
    nString filename;
};

//------------------------------------------------------------------------------
/**
*/
inline
nFontDesc::nFontDesc() :
    height(10),
    weight(Normal),
    italic(false),
    underline(false),
    antiAliased(true),
    typeFace("Arial")
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
nFontDesc::~nFontDesc()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nFontDesc::operator==(const nFontDesc& rhs) const
{
    return ((this->height == rhs.height) &&
            (this->weight == rhs.weight) &&
            (this->italic == rhs.italic) &&
            (this->underline == rhs.underline) &&
            (this->antiAliased == rhs.antiAliased) &&
            (this->typeFace == rhs.typeFace));
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nFontDesc::SetHeight(int h)
{
    this->height = h;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nFontDesc::GetHeight() const
{
    return this->height;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nFontDesc::SetWeight(Weight w)
{
    this->weight = w;
}

//------------------------------------------------------------------------------
/**
*/
inline
nFontDesc::Weight
nFontDesc::GetWeight() const
{
    return this->weight;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nFontDesc::SetItalic(bool b)
{
    this->italic = b;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nFontDesc::GetItalic() const
{
    return this->italic;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nFontDesc::SetUnderline(bool b)
{
    this->underline = b;
}

//------------------------------------------------------------------------------
/**
*/
inline bool
nFontDesc::GetUnderline() const
{
    return this->underline;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nFontDesc::SetAntiAliased(bool b)
{
    this->antiAliased = b;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nFontDesc::GetAntiAliased() const
{
    return this->antiAliased;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nFontDesc::SetTypeFace(const char* n)
{
    n_assert(n);
    this->typeFace = n;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nFontDesc::GetTypeFace() const
{
    return this->typeFace.IsEmpty() ? 0 : this->typeFace.Get();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nFontDesc::SetFilename(const char* n)
{
    n_assert(n);
    this->filename = n;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nFontDesc::GetFilename() const
{
    return this->filename.IsEmpty() ? 0 : this->filename.Get();
}

//------------------------------------------------------------------------------
/**
    Convert the weight to its corresponded string.
*/
inline
nString
nFontDesc::WeightToString(nFontDesc::Weight renderFlag)
{
    switch (renderFlag)
    {
        case nFontDesc::Thin:          return nString("Thin");
        case nFontDesc::Light:         return nString("Light");
        case nFontDesc::Normal:        return nString("Normal");
        case nFontDesc::Bold:          return nString("Bold");
        case nFontDesc::ExtraBold:     return nString("ExtraBold");
        default:
            n_error("nFontDesc::WeightToString(): invalid weight!");
    }
}


//------------------------------------------------------------------------------
/**
    Convert the given string to its corresponded weight.
*/
inline
nFontDesc::Weight
nFontDesc::StringToWeight(const nString& str)
{
    n_assert(!str.IsEmpty());
    if ("Thin" == str)
    {
        return Thin;
    }
    if ("Light" == str)
    {
        return Light;
    }
    if ("Normal" == str)
    {
        return Normal;
    }
    if ("Bold" == str)
    {
        return Bold;
    }
    if ("ExtraBold" == str)
    {
        return ExtraBold;
    }
    n_error("nFontDesc::StringToWeight(): invalid weight!");
    return InvalidWeight;
}

//------------------------------------------------------------------------------
#endif

