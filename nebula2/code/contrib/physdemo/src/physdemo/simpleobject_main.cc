//------------------------------------------------------------------------------
//  (C) 2003    Megan Fox
//------------------------------------------------------------------------------
#include "physdemo/simpleobject.h"

nNebulaScriptClass(SimpleObject, "nroot");

//------------------------------------------------------------------------------
/**
*/
SimpleObject::SimpleObject()
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

