//------------------------------------------------------------------------------
//  foundation/factory.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "foundation/factory.h"

namespace Foundation
{
Factory* Factory::instance = 0;

//------------------------------------------------------------------------------
/**
*/
Factory* Factory::Instance()
{
/*	if (instance == 0)
	{
		instance = new Factory;
		n_assert(instance != 0);
	}
	return instance;*/
	static Factory instance;
	return &instance;
}

//------------------------------------------------------------------------------
/**
void
Factory::Destroy()
{
	if (instance != 0)
	{
		delete instance;
		instance = 0;
	}
}*/


//------------------------------------------------------------------------------
/**
*/
Factory::~Factory()
{
}

//------------------------------------------------------------------------------
/**
*/
Factory::Factory ()
{
}

//------------------------------------------------------------------------------
/**
*/
void
Factory::Add(FactoryFunction function, const nString& className)
{
	n_assert2(!Has(className), className.Get());
	Cell newCell(className, function);
	this->table.PushBack(newCell);
}

//------------------------------------------------------------------------------
/**
*/
bool
Factory::Has(const nString& className) const
{
	for (int i = 0; i < this->table.Size(); i++)
	{
		if (this->table[i].name == className)
		{
			return true;
		}
	}
	return false;
}

//------------------------------------------------------------------------------
/**
*/
RefCounted*
Factory::Create(const nString& className) const
{
    n_assert2(Has(className), className.Get());

	for (int i = 0; i < this->table.Size(); i++)
	{
		if (this->table[i].name == className)
		{
			return this->table[i].function();
		}
	}
	return 0;
}

//------------------------------------------------------------------------------
/**
*/
int Factory::GetNumClassNames() const
{
    return this->table.Size();
}

//------------------------------------------------------------------------------
/**
*/
const nString& Factory::GetClassNameAt(int index) const
{
    return this->table[index].name;
}

} // namespace Foundation
