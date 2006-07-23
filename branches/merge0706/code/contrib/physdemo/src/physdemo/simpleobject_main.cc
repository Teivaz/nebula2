//------------------------------------------------------------------------------
//  (C) 2003    Megan Fox
//------------------------------------------------------------------------------
#include "physdemo/simpleobject.h"

nNebulaScriptClass(SimpleObject, "kernel::nroot");

//------------------------------------------------------------------------------
/**
*/
SimpleObject::SimpleObject(): disableTimeout(0)
{
}

//------------------------------------------------------------------------------
/**
*/
SimpleObject::~SimpleObject()
{
    if (this->refFloatyText.isvalid())
        this->refFloatyText->Release();
}

