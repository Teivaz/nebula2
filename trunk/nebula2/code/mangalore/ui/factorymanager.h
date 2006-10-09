#ifndef UI_FACTORYMANAGER_H
#define UI_FACTORYMANAGER_H
//------------------------------------------------------------------------------
/**
    @class UI::FactoryManager

    Creates user interface elements.

    (C) 2005 Radon Labs GmbH
*/
#include "foundation/refcounted.h"

//------------------------------------------------------------------------------
namespace UI
{
class Element;

class FactoryManager : public Foundation::RefCounted
{
    DeclareRtti;
	DeclareFactory(FactoryManager);

public:
    /// constructor
    FactoryManager();
    /// destructor
    virtual ~FactoryManager();
    /// get instance pointer
    static FactoryManager* Instance();
    /// create an user interface element from type string
    virtual Element* CreateElement(const nString& type) const;

private:
    static FactoryManager* Singleton;
};

RegisterFactory(FactoryManager);

//------------------------------------------------------------------------------
/**
*/
inline
FactoryManager*
FactoryManager::Instance()
{
    n_assert(0 != Singleton);
    return Singleton;
}

} // namespace UI
//------------------------------------------------------------------------------
#endif
