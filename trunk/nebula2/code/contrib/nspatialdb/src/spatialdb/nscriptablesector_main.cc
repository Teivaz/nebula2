//-------------------------------------------------
// nscriptablesector_main.cc
// (C) 2004 Gary Haussmann
//--------------------------------------------------

#include "spatialdb/nscriptablesector.h"
#include "scene/nscenenode.h"
#include "scene/nrendercontext.h"
#include "mathlib/bbox.h"

nNebulaScriptClass(nScriptableSector, "nspatialsector");

nScriptableSector::nScriptableSector() : nSpatialSector()
{
}

nScriptableSector::~nScriptableSector()
{
    // trash all elements in the sector
    for (int i=0; i < scriptelm_array.Size(); i++)
    {
        nSpatialElement *element = scriptelm_array[i];
        nScriptableSectorObject *object = (nScriptableSectorObject *)element->GetPtr();
        RemoveElement(element);
        n_delete(object);
        n_delete(element);
    }
}

/// Create a renderable object and put it in the sector space
nSpatialElement *nScriptableSector::AddVisibleObject(const char *objname, const char *scenenodetorender, const vector3 &pos, float radius)
{
    nSpatialElement *element = n_new(nSpatialElement);
    nScriptableSectorObject *newobject = n_new(nScriptableSectorObject(AutoName(objname), element));
    newobject->rendernode = scenenodetorender;
    if (newobject->rendernode.isvalid())
    {
        newobject->rc.SetRootNode(newobject->rendernode.get());
        newobject->rendernode->RenderContextCreated( &(newobject->rc) );
    }

    element->SetPtr(newobject);

    this->AddElement(element);
    UpdateElement(element, pos, radius);

    matrix44 newtransform;
    newtransform.ident();
    newtransform.scale(vector3(1,1,1) * radius);
    newtransform.translate(pos);
    newobject->rc.SetTransform(newtransform);

    scriptelm_array.PushBack(element);

    return element;
}


/// Create a (non-visible) occluder object from the specified bounding box
nSpatialOccluderElement *nScriptableSector::AddOccludingObject(const char *objname, const vector3 &min, const vector3 &max)
{
    nSpatialOccluderElement *element = n_new(nSpatialOccluderElement);
    nScriptableSectorObject *newobject = n_new(nScriptableSectorObject(AutoName(objname), element));
    element->SetPtr(newobject);

    this->AddElement(element);
    vector3 boxcenter( (min+max)*0.5 ), boxextents( (max-min)*0.5 );
    bbox3 thisbbox(boxcenter, boxextents);

    UpdateElement(element, boxcenter, thisbbox);

    scriptelm_array.PushBack(element);

    return element;
}


/// Create a non-visible portal object that connects to another sector
nSpatialPortalElement *nScriptableSector::AddPortalObject(const char *objname, const char *othersector,  const vector3 &pos, float radius)
{
    nSpatialPortalElement *element = n_new(nSpatialPortalElement(this, NULL));
    nScriptableSectorObject *newobject = n_new(nScriptableSectorObject(AutoName(objname), element));
    element->SetPtr(newobject);

    this->AddElement(element);

    UpdateElement(element, pos, radius);

    scriptelm_array.PushBack(element);

    return element;
}


/// Delete a spatial object by providing the name you gave the object when creating it
bool nScriptableSector::RemObject(const char *objname)
{
    // find the element in here somewhere...
    for (int i=0; i < scriptelm_array.Size(); i++)
    {
        nSpatialElement *walknodes = scriptelm_array[i];
        nScriptableSectorObject *walkobject = (nScriptableSectorObject *)walknodes->GetPtr();

        if (walkobject->objectname == objname)
        {
            scriptelm_array.Erase(i);
            RemoveElement(walknodes);
            n_delete(walkobject);
            n_delete(walknodes);
            return true;
        }
    }

    // not found, oops
    return false;
}

/// Auto-generate a possibly-unique name for this object
nString nScriptableSector::AutoName(const char *suggestname)
{
    if (suggestname == NULL)
        return nString("unknown");
    else
        return nString(suggestname);
}

