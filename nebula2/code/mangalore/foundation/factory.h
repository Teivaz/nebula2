#ifndef FOUNDATION_FACTORY_H
#define FOUNDATION_FACTORY_H
//------------------------------------------------------------------------------
/**
	Facilities for creating objects by type name to support load/save mechanism.

    (C) 2005 Radon Labs GmbH
*/
#include "foundation/ptr.h"
#include "util/narray.h"
#include "util/nstring.h"

namespace Foundation 
{
class RefCounted;

// Factory function.
typedef RefCounted* (*FactoryFunction)();


//------------------------------------------------------------------------------
class Factory
{
public:
	/// The one and only instance.
	static Factory* Instance();
	/// Optional destructor.
	static void Destroy();

	/// Map factory function `function' to class name `className'.
	void Add(FactoryFunction function, const nString& className);
	/// Is a factory function associated with class name `className'?
	bool Has(const nString& className) const;
	/// Create object associated with `className'.
	RefCounted* Create(const nString& className) const;

protected:
	/// Constructor.
	Factory ();
	/// Destruct.
	~Factory();

private:
	static Factory* instance;

	// To be replaced by a useful radonlabs hash table (there is none so far)...
	struct Cell
	{
		Cell() : name(""), function(0) {};
		Cell(const nString& n, FactoryFunction f) : name(n), function(f) {};
		nString name;
		FactoryFunction function;
	};
	nArray<Cell> table;
};

}; // namespace Foundation

//------------------------------------------------------------------------------
#endif