//----------------------------------------------------------------------------
// (c) 2006    Vadim Macagon
//----------------------------------------------------------------------------
#include "lwwrapper/nlwcustomobjecthandler.h"

//----------------------------------------------------------------------------
/**
*/
nLWCustomObjectHandlerBase::~nLWCustomObjectHandlerBase()
{
    // empty
}

//----------------------------------------------------------------------------
/**
*/
LWError
nLWCustomObjectHandlerBase::OnLoad(const LWLoadState* state)
{
    // do nothing
    return 0;
}

//----------------------------------------------------------------------------
/**
*/
LWError
nLWCustomObjectHandlerBase::OnSave(const LWSaveState* state)
{
    // do nothing
    return 0;
}

//----------------------------------------------------------------------------
/**
*/
const LWItemID*
nLWCustomObjectHandlerBase::OnUseItems()
{
    return 0;
}

//----------------------------------------------------------------------------
/**
*/
void
nLWCustomObjectHandlerBase::OnChangeID(const LWItemID* idList)
{
    // do nothing
}

//----------------------------------------------------------------------------
/**
*/
void
nLWCustomObjectHandlerBase::OnEvaluate(const LWCustomObjAccess* access)
{
    // do nothing
}

//----------------------------------------------------------------------------
/**
*/
unsigned int
nLWCustomObjectHandlerBase::OnFlags()
{
    return 0;
}

//----------------------------------------------------------------------------
/**
*/
LWError
nLWCustomObjectHandlerBase::OnDisplayUI()
{
    // do nothing
    return 0;
}

//----------------------------------------------------------------------------
// EOF
//----------------------------------------------------------------------------
