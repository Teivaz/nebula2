#ifndef GRAPHICS_RESOURCE_H
#define GRAPHICS_RESOURCE_H
//------------------------------------------------------------------------------
/**
    @class Graphics::Resource

    Wraps a single Nebula graphics object identified by a resource name.
    The resource name must consist of a category, a name and an optional
    subname, delimited by slashes, for example:

    @verbatim
    resource name:      "doors/woodendoor"
    file system file:   "gfxlib:doors/woodendoor.n2"
    Nebula location:    "res/gfx/doors/woodendoor"

    resource name:      "doors/woodendoor/knob"
    file system file:   "gfxlib:doors/woodendoor.n2"
    Nebula location:    "res/gfx/doors/woodendoor/knob"
    @endverbatim

    (C) 2003 RadonLabs GmbH
*/
#include "util/nstring.h"
#include "foundation/server.h"
#include "foundation/refcounted.h"

class nTransformNode;
class nRenderContext;

//------------------------------------------------------------------------------
namespace Graphics
{
class Resource : public Foundation::RefCounted
{
    DeclareRtti;
	DeclareFactory(Resource);

public:
    /// constructor
    Resource();
    /// destructor
    ~Resource();
    /// set the resource name
    void SetName(const nString& n);
    /// get the resource name
    const nString& GetName() const;
    /// load the resource
    void Load();
    /// unload the resource
    void Unload();
    /// return true if resource is loaded
    bool IsLoaded() const;
    /// get the Nebula scene node
    nTransformNode* GetNode() const;

private:
    /// find an existing root node
    nRoot* FindRootNode();

    nString name;
    nRef<nRoot> refRootNode;   // need to keep track of root node for proper refcounting
    nRef<nTransformNode> refNode;
};

RegisterFactory(Resource);

//------------------------------------------------------------------------------
/**
*/
inline
nTransformNode*
Resource::GetNode() const
{
    return this->refNode.get();
}

} // namespace Graphics
//------------------------------------------------------------------------------
#endif
