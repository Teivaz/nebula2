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
    // not needed as objects are children and will be released by nRoot anyway.
//    if (this->refPhysGeom.isvalid())
//        this->refPhysGeom->Release();
//    if (this->refPhysBody.isvalid())
//        this->refPhysBody->Release();

    if (this->refFloatyText.isvalid())
        this->refFloatyText->Release();
}

