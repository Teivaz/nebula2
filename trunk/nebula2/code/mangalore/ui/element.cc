//------------------------------------------------------------------------------
//  ui/element.cc
//  (C) 2005 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "ui/element.h"
#include "ui/factorymanager.h"
#include "ui/server.h"
#include "ui/event.h"

namespace UI
{
ImplementRtti(UI::Element, Foundation::RefCounted);

//------------------------------------------------------------------------------
/**
*/
Element::Element() :
    dismissed(false),
    isVisible(true),
    parentElement(0),
    isValid(false),
    userData(0)
{
    this->transformNodeClass = nKernelServer::Instance()->FindClass("ntransformnode");
}

//------------------------------------------------------------------------------
/**
*/
Element::~Element()
{
    n_assert(this->childElements.Size() == 0);
}

//------------------------------------------------------------------------------
/**
    Called by parent to build the gui hierarchy.
*/
void
Element::AttachElement(Element* elm)
{
    n_assert(elm);
    this->childElements.Append(elm);
    elm->OnCreate(this);
}

//------------------------------------------------------------------------------
/**
    Remove a child element. This will invoke the OnRemove() method on the
    child.
*/
void
Element::RemoveElement(Element* elm)
{
    n_assert(elm);
    elm->OnDestroy();
    nArray<Ptr<Element> >::iterator iter = this->childElements.Find(elm);
    n_assert(iter);
    this->childElements.Erase(iter);
}

//------------------------------------------------------------------------------
/**
    Provides access to the child element array.
*/
const nArray<Ptr<Element> >&
Element::GetElements() const
{
    return this->childElements;
}

//------------------------------------------------------------------------------
/**
    Find a child element by its id. Returns 0 if not found.
*/
Element*
Element::FindElement(const nString& id)
{
    int i;
    for (i = 0; i < this->childElements.Size(); i++)
    {
        if (id == this->childElements[i]->GetId())
        {
            return this->childElements[i];
        }
    }
    // fallthrough: not found
    return 0;
}

//------------------------------------------------------------------------------
/**
    This method is called when the element has been attached to a parent.
*/
void
Element::OnCreate(Element* parent)
{
    n_assert(this->gfxNode.isvalid());
    n_assert(!this->IsValid());
    this->isValid = true;

    this->parentElement = parent;

    // read own attributes and update screen space rect
    this->SetId(this->gfxNode->GetName());
    this->UpdateScreenSpaceRect();

    // create child elements by traversing the Nebula object hierarchy
    // and looking for nodes with the rlGui attribute
    nRoot* nebNode;
    for (nebNode = this->gfxNode->GetHead();
         nebNode;
         nebNode = nebNode->GetSucc())
    {
        if (nebNode->IsA(this->transformNodeClass))
        {
            nTransformNode* tformNode = (nTransformNode*) nebNode;
            if (tformNode->HasAttr("rlGui"))
            {
                // create a new element
                nString guiType = tformNode->GetStringAttr("rlGuiType");
                Element* guiElement = FactoryManager::Instance()->CreateElement(guiType);
                n_assert(guiElement);
                guiElement->SetGfxNode(tformNode);
                this->AttachElement(guiElement);
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
    This method is called when the element has been removed from its parent.
*/
void
Element::OnDestroy()
{
    if (this->IsValid())
    {
        // destroy all children
        int i;
        for (i = 0; i < this->childElements.Size(); i++)
        {
            this->childElements[i]->OnDestroy();
        }
        this->childElements.Clear();

        // clear own parent pointer
        this->parentElement = 0;
        this->isValid = false;
    }
}


//------------------------------------------------------------------------------
/**
    This method is called once per frame during UIServer::Trigger() after
    the input has been delivered to the gui.
*/
void
Element::OnFrame()
{
    // distribute to children
    if (this->IsVisible())
    {
        int i;
        for (i = 0; i < this->childElements.Size(); i++)
        {
            this->childElements[i]->OnFrame();
        }
    }
}

//------------------------------------------------------------------------------
/**
    This method is called when the element should render itself.
*/
void
Element::OnRender()
{
    // distribute to children
    int i;
    for (i = 0; i < this->childElements.Size(); i++)
    {
        this->childElements[i]->OnRender();
    }
}

//------------------------------------------------------------------------------
/**
    Returns the view space transform of the element.
*/
matrix44
Element::GetViewSpaceTransform() const
{
    matrix44 m = this->gfxNode->GetTransform();
    nTransformNode* parentNode = (nTransformNode*) this->gfxNode->GetParent();
    while (parentNode && parentNode->IsA(this->transformNodeClass))
    {
        m = m * parentNode->GetTransform();
        parentNode = (nTransformNode*) parentNode->GetParent();
    }
    return m;
}

//------------------------------------------------------------------------------
/**
    Set the position of the element as view space transformation.
*/
void
Element::SetViewSpaceTransform(const matrix44& m)
{
    // get the inverse exclusive matrix for moving the
    // view space matrix into parent space
    matrix44 invParent;
    nTransformNode* parentNode = (nTransformNode*) this->gfxNode->GetParent();
    while (parentNode && parentNode->IsA(this->transformNodeClass))
    {
        invParent = invParent * parentNode->GetTransform();
        parentNode = (nTransformNode*) parentNode->GetParent();
    }
    invParent.invert_simple();
    matrix44 localMatrix = m * invParent;
    this->gfxNode->SetTransform(localMatrix);
    this->UpdateScreenSpaceRect();
}

//------------------------------------------------------------------------------
/**
    Update the screen space rectangle. This gets the bounding box from
    our Nebula2 node, resolves the hierarchy transforms, and transforms
    the result to screen space.
*/
void
Element::UpdateScreenSpaceRect()
{
    // compute flattened transformation matrix, this is
    matrix44 m = this->GetViewSpaceTransform();

    // multiply by orthogonal projection matrix, this must be the same
    // as used by the GUI shader!
    m = m * Server::Instance()->GetGuiProjectionMatrix();

    // get local bounding box and transform to screen space
    bbox3 box = this->gfxNode->GetLocalBox();
    box.transform_divw(m);

    // build screen space rectangle from result, move the range
    // from (-1.0, 1.0) to (0.0, 1.0)
    vector2 vmin, vmax;
    vmin.x = (box.vmin.x + 1.0f) * 0.5f;
    vmin.y = 1.0f - ((box.vmax.y + 1.0f) * 0.5f);
    vmax.x = (box.vmax.x + 1.0f) * 0.5f;
    vmax.y = 1.0f - ((box.vmin.y + 1.0f) * 0.5f);
    this->screenSpaceRect.set(vmin, vmax);
}

//------------------------------------------------------------------------------
/**
    This method returns true if the given mouse position is inside the
    elements screen space area. By default this checks the mouse position
    against the screen space rectangle computed in UpdateScreenSpaceRect(),
    but you can override the method in a subclass if you need more
    advanced picking.
*/
bool
Element::Inside(const vector2& mousePos)
{
    if (this->IsVisible() && this->IsValid())
    {
        // hmm, we could be animated...
        this->UpdateScreenSpaceRect();
        return this->screenSpaceRect.inside(mousePos);
    }
    else
    {
        return false;
    }
}

//------------------------------------------------------------------------------
/**
    This method sends an event to the current GUI event handler.
*/
void
Element::PutEvent(const nString& eventName)
{
    Ptr<Event> guiEvent = Event::Create();
    guiEvent->SetEventName(eventName);
    guiEvent->SetElement(this);
    Server::Instance()->PutEvent(guiEvent);
}

//------------------------------------------------------------------------------
/**
*/
void
Element::SetVisible(bool b)
{
    this->isVisible = b;

    // distribute to children
    int i;
    for (i = 0; i < this->childElements.Size(); i++)
    {
        this->childElements[i]->SetVisible(b);
    }
}



} // namespace
