//------------------------------------------------------------------------------
//  ui/canvas.cc
//  (C) 2005 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "ui/canvas.h"
#include "ui/server.h"
#include "foundation/factory.h"
#include "graphics/lightentity.h"
#include "kernel/nfileserver2.h"
#include "game/time/guitimesource.h"

namespace UI
{
ImplementRtti(UI::Canvas, UI::Element);
ImplementFactory(UI::Canvas);

//------------------------------------------------------------------------------
/**
*/
Canvas::Canvas()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
Canvas::~Canvas()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Recursively finds the first Nebula canvas node in the hierarchy.
*/
nTransformNode*
Canvas::FindCanvasNodeInHierarchy(nTransformNode* root)
{
    n_assert(root);

    // check if root is a canvas, if yes, break recursion
    if (root->HasAttr("rlGui"))
    {
        // create a new element
        nString guiType = root->GetStringAttr("rlGuiType");
        if ("Canvas" == guiType)
        {
            return root;
        }
    }

    // recurse into children
    nRoot* child;
    for (child = root->GetHead(); child; child = child->GetSucc())
    {
        if (child->IsA(this->transformNodeClass))
        {
            nTransformNode* result = this->FindCanvasNodeInHierarchy((nTransformNode*)child);
            if (result)
            {
                return result;
            }
        }
    }
    return 0;
}

//------------------------------------------------------------------------------
/**
    This creates a graphics entity representing the visual of the canvas
    and converts the resource name into a Nebula2 node pointer.
*/
void
Canvas::OnCreate(UI::Element* parent)
{
    // canvases don't have parents
    n_assert(0 == parent);

    // create graphics entity, first check if it actually exists...
    n_assert(this->resourceName.IsValid());
    nString resPath;
    resPath.Format("gfxlib:%s.n2", this->resourceName.Get());
    if (nFileServer2::Instance()->FileExists(resPath))
    {
	    this->graphicsEntity = Graphics::Entity::Create();
        this->graphicsEntity->SetTimeSource(Game::GuiTimeSource::Instance());
        this->graphicsEntity->SetResourceName(this->resourceName);
        this->graphicsEntity->SetRenderFlag(nRenderContext::DoOcclusionQuery, false);
        this->graphicsEntity->OnActivate();

        // find the Nebula node which represents the canvas
        nTransformNode* rootNode   = this->graphicsEntity->GetResource().GetNode();
        nTransformNode* canvasNode = this->FindCanvasNodeInHierarchy(rootNode);
        n_assert(canvasNode);
        this->SetGfxNode(canvasNode);

        // establish link between UIServer's light source and new graphics entity
        Graphics::LightEntity* lightEntity = Server::Instance()->GetLightEntity();
        this->graphicsEntity->AddLink(Graphics::Entity::LightLink, lightEntity);
        lightEntity->AddLink(Graphics::Entity::LightLink, this->graphicsEntity);

        // create child elements
        Element::OnCreate(parent);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
Canvas::OnDestroy()
{
    if (this->graphicsEntity.isvalid())
    {
        this->graphicsEntity->OnDeactivate();
        this->graphicsEntity = 0;
    }
    Element::OnDestroy();
}

//------------------------------------------------------------------------------
/**
*/
void
Canvas::OnRender()
{
    if (this->IsVisible())
    {
        // first distribute to children
        Element::OnRender();

        // then render our graphics entity
        this->graphicsEntity->Render();
    }
}

} // namespace UI
