#ifndef FOUNDATION_RTTI_H
#define FOUNDATION_RTTI_H
//------------------------------------------------------------------------------
/**
    @class Foundation::Rtti

    Implements the runtime type information system of Mangalore. Every class
    derived from RefCounted should define a static RTTI object which is initialized
    with a pointer to a static string containing the human readable name
    of the class, and a pointer to the static RTTI object of the
    parent class.

    (C) 2003 RadonLabs GmbH
*/
#include "kernel/ntypes.h"
#include "util/nstring.h"

//------------------------------------------------------------------------------
namespace Foundation
{
class Rtti
{
public:
    /// constructor
    Rtti(const nString& className, const Rtti* parentClass);
    /// get class name
    const nString& GetName() const;
    /// get pointer to parent class
    const Rtti* GetParent() const;

private:
    const nString name;
    const Rtti* parent;
};

//------------------------------------------------------------------------------
/**
*/
inline
Rtti::Rtti(const nString& className, const Rtti* parentClass) :
    name(className),
    parent(parentClass)
{
    n_assert(className != "");
    n_assert(parentClass != this);
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
Rtti::GetName() const
{
    return this->name;
}

//------------------------------------------------------------------------------
/**
*/
inline
const Rtti*
Rtti::GetParent() const
{
    return this->parent;
}

};  // namespace Foundation

//------------------------------------------------------------------------------
/**
    Type declaration (header file).
*/
#define DeclareRtti \
public: \
    static Foundation::Rtti RTTI; \
    virtual Foundation::Rtti* GetRtti() const;

//------------------------------------------------------------------------------
/**
    Type implementation (source file).
*/
#define ImplementRtti(type, ancestor) \
Foundation::Rtti type::RTTI(#type, &ancestor::RTTI); \
Foundation::Rtti* type::GetRtti() const { return &this->RTTI; }

//------------------------------------------------------------------------------
/**
    Type implementation of topmost type in inheritance hierarchy (source file).
*/
#define ImplementRootRtti(type) \
Foundation::Rtti type::RTTI(#type, 0); \
Foundation::Rtti* type::GetRtti() const { return &this->RTTI; }

//------------------------------------------------------------------------------
#endif
