#ifndef N_SCRIPTABLESECTOR_H
#define N_SCRIPTABLESECTOR_H

#include "spatialdb/nspatialsector.h"
#include "spatialdb/nspatialelements.h"
#include "scene/nrendercontext.h"
#include "kernel/ndynautoref.h"

// the data associated with a specific spatial element
class nScriptableSectorObject;

/**
 * @class nScriptableSector
   @ingroup NSpatialDBContribModule
   @brief A spatial sector which allows element addition/deletion with the script interface.

   This is really just a subclass that allows you to add and position spatial elements
   using script messges.  Such a thing may be useful for real production code, but it is
   mainly used for testing the spatial database where various script files are used to setup different
   spatial configurations.
*/

class nScriptableSector : public nSpatialSector {
public:
    nScriptableSector();
    ~nScriptableSector();

    /// Create a renderable object and put it in the sector space
    nSpatialElement *AddVisibleObject(const char *objname, const char *scenenodetorender, const vector3 &pos, float radius);

    /// Create a (non-visible) occluder object from the specified bounding box
    nSpatialOccluderElement *AddOccludingObject(const char *objname, const vector3 &min, const vector3 &max);

    /// Create a non-visible portal object that connects to another sector
    nSpatialPortalElement *AddPortalObject(const char *objname, const char *othersector, const vector3 &pos, float radius);

    /// Delete a spatial object by providing the name you gave the object when creating it
    /** Returns true if the object was found and deleted */
    bool RemObject(const char *objname);

    /// Clear out all objects
    void ClearObjects();

    /// object persistency
    virtual bool SaveCmds(nPersistServer *ps);

private:
    /// Auto-generate a possibly-unique name for this object
    nString AutoName(const char *suggestname);

    /// list of the scriptable elements in the sector-for easy access later
    nArray<nScriptableSectorObject *> m_scriptobject_array;
};

// your visitors will return nSpatialElements, and a GetPtr() on them will return a pointer to these things
class nScriptableSectorObject
{
public:
    nScriptableSectorObject(nString name, nSpatialElement *e) : objectname(name), spatialinfo(e){
    }

    ~nScriptableSectorObject() {
    }

    nString objectname; // name given to this object by script for later access
    nSpatialElement *spatialinfo;   // we might need to know our spatial info

    nRenderContext rc;              // rendercontext to render with,if this node is visible
    nDynAutoRef<nSceneNode> rendernode; // scene node to render, if this node is visible
};



#endif
