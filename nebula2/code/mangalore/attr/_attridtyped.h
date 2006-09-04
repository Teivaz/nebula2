#ifndef ATTR__ATTRID_TYPED_H
#define ATTR__ATTRID_TYPED_H
//------------------------------------------------------------------------------
/**
    @brief Template class to add compile time type information to the _attrid
           class

    Template class to add compile time type information to the _attrid class.
    while all type data is stored in the base class objects, functionality for
    storing the objects in a global registry (one for each id type) is provided
    in here.

    (C) 2005 Radon Labs GmbH
*/
#include "attr/_attrid.h"

//------------------------------------------------------------------------------
template <class _AttrIdType>
class _attridTyped : public _attrid
{
public:
    /// constructor
    _attridTyped(const char* name, Type t, uchar flags);
    /// destructor
    ~_attridTyped();

    // FIXME: [np] make protected with friend function, if possible
    /// find an id object by name
    static _attridTyped<_AttrIdType>* Find( const nString& name );

protected:
    /// default constructor is protected
    _attridTyped();

    /// add the id to the (given) registry
    void AddToRegistry();
    /// remove the id from the (given) registry
    void RemoveFromRegistry();

    static nArray<_attridTyped<_AttrIdType>*>* attrIdRegistry;
};

//------------------------------------------------------------------------------

#define _DeclareAttribute( id, type )   extern const type id ## Data; extern const type* id

#define _DefineAttribute( id, typeConstant, type )\
    const type id ## Data (#id, typeConstant, _attrid::Read|_attrid::Write);\
    const type* id = & id ## Data

#define _DefineStorableAttribute( id, typeConstant, type )\
    const type id ## Data (#id, typeConstant, _attrid::Read|_attrid::Write|_attrid::Store);\
    const type* id = & id ## Data

#define DeclareVoid(id)         _DeclareAttribute( id, _attridTyped<attr::VoidT> );
#define DefineVoid(id)          _DefineAttribute( id, _attrid::Void, _attridTyped<attr::VoidT> );
#define DefineStorableVoid(id)  _DefineStorableAttribute( id, _attrid::Void, _attridTyped<attr::VoidT> );

#define DeclareBool(id)         _DeclareAttribute( id, _attridTyped<attr::BoolT> );
#define DefineBool(id)          _DefineAttribute( id, _attrid::Bool, _attridTyped<attr::BoolT> );
#define DefineStorableBool(id)  _DefineStorableAttribute( id, _attrid::Bool, _attridTyped<attr::BoolT> );

#define DeclareInt(id)          _DeclareAttribute( id, _attridTyped<attr::IntT> );
#define DefineInt(id)           _DefineAttribute( id, _attrid::Int, _attridTyped<attr::IntT> );
#define DefineStorableInt(id)   _DefineStorableAttribute( id, _attrid::Int, _attridTyped<attr::IntT> );

#define DeclareFloat(id)        _DeclareAttribute( id, _attridTyped<attr::FloatT> );
#define DefineFloat(id)         _DefineAttribute( id, _attrid::Float, _attridTyped<attr::FloatT> );
#define DefineStorableFloat(id) _DefineStorableAttribute( id, _attrid::Float, _attridTyped<attr::FloatT> );

#define DeclareString(id)       _DeclareAttribute( id, _attridTyped<attr::StringT> );
#define DefineString(id)        _DefineAttribute( id, _attrid::String, _attridTyped<attr::StringT> );
#define DefineStorableString(id)  _DefineStorableAttribute( id, _attrid::String, _attridTyped<attr::StringT> );

#define DeclareVector3(id)    _DeclareAttribute( id, _attridTyped<attr::Vector3T> );
#define DefineVector3(id)     _DefineAttribute( id, _attrid::Vector3, _attridTyped<attr::Vector3T> );
#define DefineStorableVector3(id)  _DefineStorableAttribute( id, _attrid::Vector3, _attridTyped<attr::Vector3T> );

#define DeclareVector4(id)    _DeclareAttribute( id, _attridTyped<attr::Vector4T> );
#define DefineVector4(id)     _DefineAttribute( id, _attrid::Vector4, _attridTyped<attr::Vector4T> );
#define DefineStorableVector4(id)  _DefineStorableAttribute( id, _attrid::Vector4, _attridTyped<attr::Vector4T> );

#define DeclareMatrix44(id)    _DeclareAttribute( id, _attridTyped<attr::Matrix44T> );
#define DefineMatrix44(id)     _DefineAttribute( id, _attrid::Matrix44, _attridTyped<attr::Matrix44T> );
#define DefineStorableMatrix44(id)  _DefineStorableAttribute( id, _attrid::Matrix44, _attridTyped<attr::Matrix44T> );

//------------------------------------------------------------------------------
/**
*/
template <class _AttrIdType>
_attridTyped<_AttrIdType>::_attridTyped() :
    _attrid()
{
    // empty
}

// NOTE: this is implemented as a pointer, which is more complicated
// than it seems to be necessary. The reason is that the initialization
// order of static objects is not guaranteed, thus the _attrid constructor
// must initialize the registry object on demand and clean it up when
// the last _attrid is killed.
template <class _AttrIdType>
nArray<_attridTyped<_AttrIdType>*>* _attridTyped<_AttrIdType>::attrIdRegistry = 0;

//------------------------------------------------------------------------------
/**
*/
template <class _AttrIdType>
_attridTyped<_AttrIdType>::_attridTyped(const char* n, Type t, uchar f) :
    _attrid( n, t, f )
{
    AddToRegistry();
}

//------------------------------------------------------------------------------
/**
*/
template <class _AttrIdType>
_attridTyped<_AttrIdType>::~_attridTyped()
{
    RemoveFromRegistry();
}

//------------------------------------------------------------------------------
/**
    Add the attribute id to the given registry.
*/
template <class _AttrIdType>
void
_attridTyped<_AttrIdType>::AddToRegistry()
{
    if (0 == attrIdRegistry)
    {
        attrIdRegistry = n_new(nArray<_attridTyped<_AttrIdType>*>(ID_REGISTRY_SIZE, ID_REGISTRY_SIZE));
    }
    attrIdRegistry->Append(this);
}

//------------------------------------------------------------------------------
/**
    Finally removes the AttributeID from the ID registry.
    May only be called from within destructor.
*/
template <class _AttrIdType>
void
_attridTyped<_AttrIdType>::RemoveFromRegistry()
{
    n_assert(0 != attrIdRegistry);

    typename nArray<_attridTyped<_AttrIdType>*>::iterator iter = attrIdRegistry->Find(this);
    n_assert(iter);
    attrIdRegistry->Erase(iter);

    // am I the last one?
    if (0 == attrIdRegistry->Size())
    {
        n_delete(attrIdRegistry);
        attrIdRegistry = 0;
    }
}



//------------------------------------------------------------------------------
/**
    Find an _id object by its name. Returns 0 if not found. This is a static
    method.

    FIXME: currently this is a slow method.
    NOTE:  implemented this way (with all registries in base class, template function) to
           let implementation reside in base class (here) and avoid type casts
*/
template <class _AttrIdType>
_attridTyped<_AttrIdType>*
_attridTyped<_AttrIdType>::Find( const nString& n )
{
    // check if any elements of this id type:
    if (!attrIdRegistry) return 0;

    int num = attrIdRegistry->Size();
    for (int i = 0; i < num; i++)
    {
        if (attrIdRegistry->At(i)->GetName() == n)
        {
            return attrIdRegistry->At(i);
        }
    }
    return 0;
}

//------------------------------------------------------------------------------
#endif
