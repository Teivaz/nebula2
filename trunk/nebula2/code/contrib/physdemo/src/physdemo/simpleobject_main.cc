//------------------------------------------------------------------------------
//  (C) 2003	Megan Fox
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
}

bool
SimpleObject::Release()
{
    if (this->refPhysGeom.isvalid())
        this->refPhysGeom->Release();
    if (this->refPhysBody.isvalid())
        this->refPhysBody->Release();
    if (this->refRootShapeNode.isvalid())
        this->refRootShapeNode->Release();

    if (this->refFloatyText.isvalid())
        this->refFloatyText->Release();

    nRoot::Release();

    return true;
}