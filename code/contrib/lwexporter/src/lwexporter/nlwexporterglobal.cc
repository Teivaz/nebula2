//----------------------------------------------------------------------------
// (c) 2005    Vadim Macagon
//----------------------------------------------------------------------------
#include "lwexporter/nlwtoolkitglobal.h"


nNebulaUsePackage(nnebula);

//----------------------------------------------------------------------------
/**
*/
nLWToolkitGlobal()
{

}

//----------------------------------------------------------------------------
/**
*/
~nLWToolkitGlobal()
{

}

//----------------------------------------------------------------------------
/**
	This MUST be called when a new plugin is created.
*/
void nLWToolkitGlobal::PluginCreated()
{
	// we have to ensure the master is killed after any other plugin with
	// create, so we use reference counting to accomplish that
	nLWLayoutMaster* master = nLWLayoutMaster::Instance();
	n_assert(master);
	if (master)
	{
		master->AddRef();
	}
}

//----------------------------------------------------------------------------
/**
	This MUST be called when a plugin is destroyed.
*/
void nLWToolkitGlobal::PluginDestroyed()
{
	// decrement the master reference count we incremented in PluginCreated()
	nLWLayoutMaster* master = nLWLayoutMaster::Instance();
	n_assert(master);
	if (master)
	{
		master->Release();
	}
}

//----------------------------------------------------------------------------
// EOF
//----------------------------------------------------------------------------
