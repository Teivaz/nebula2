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
    ClearObjects();
}

void nScriptableSector::ClearObjects()
{
    // trash all elements in the sector
    for (int i=0; i < m_scriptobject_array.Size(); i++)
    {
        nScriptableSectorObject *object = m_scriptobject_array[i];
        nSpatialElement *element = object->spatialinfo;
        RemoveElement(element);
        n_delete(object);
        n_delete(element);
    }

    m_scriptobject_array.Clear();
}

/// Create a renderable object and put it in the sector space
nSpatialElement *nScriptableSector::AddVisibleObject(const char *objname, const char *scenenodetorender, const vector3 &pos, float radius)
{
    bbox3 newbox(pos, vector3(radius, radius, radius));

    nSpatialElement *element = n_new(nBBoxSpatialElement(0,newbox));
    nScriptableSectorObject *newobject = n_new(nScriptableSectorObject(AutoName(objname), element));
    element->SetPtr(newobject);
    newobject->rendernode = scenenodetorender;
    if (newobject->rendernode.isvalid())
    {
        newobject->rc.SetRootNode(newobject->rendernode.get());
        newobject->rendernode->RenderContextCreated(&(newobject->rc));
    }

    this->AddElement(element);
//    UpdateElement(element, newbox);

    matrix44 newtransform;
    newtransform.ident();
    newtransform.scale(vector3(1,1,1) * radius);
    newtransform.translate(pos);
    newobject->rc.SetTransform(newtransform);

    m_scriptobject_array.PushBack(newobject);

    return element;
}


/// Create a (non-visible) occluder object from the specified bounding box
nSpatialOccluderElement *nScriptableSector::AddOccludingObject(const char *objname, const vector3 &min, const vector3 &max)
{
    vector3 boxcenter((min+max)*0.5), boxextents((max-min)*0.5);
    bbox3 thisbbox(boxcenter, boxextents);

    nSpatialOccluderElement *element = n_new(nSpatialOccluderElement(thisbbox));
    nScriptableSectorObject *newobject = n_new(nScriptableSectorObject(AutoName(objname), element));
    element->SetPtr(newobject);

    this->AddElement(element);

    m_scriptobject_array.PushBack(newobject);

    return element;
}


/// Create a non-visible portal object that connects to another sector
nSpatialPortalElement *nScriptableSector::AddPortalObject(const char *objname, const char *othersector,  const vector3 &pos, float radius)
{
    bbox3 portalbox(pos, vector3(radius, radius, radius));

    nSpatialPortalElement *element = n_new(nSpatialPortalElement(portalbox, this, NULL));
    nScriptableSectorObject *newobject = n_new(nScriptableSectorObject(AutoName(objname), element));
    element->SetPtr(newobject);

    this->AddElement(element);

    m_scriptobject_array.PushBack(newobject);

    return element;
}


/// Delete a spatial object by providing the name you gave the object when creating it
bool nScriptableSector::RemObject(const char *objname)
{
    // find the element in here somewhere...
    for (int i=0; i < m_scriptobject_array.Size(); i++)
    {
        nScriptableSectorObject *object = m_scriptobject_array[i];
        nSpatialElement *element = object->spatialinfo;

        if (object->objectname == objname)
        {
            m_scriptobject_array.Erase(i);
            RemoveElement(element);
            n_delete(object);
            n_delete(element);
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

