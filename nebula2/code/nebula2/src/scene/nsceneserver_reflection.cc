//------------------------------------------------------------------------------
//  nsceneserver_reflection.cc
//  (C) 2006 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "scene/nsceneserver.h"
#include "scene/nreflectioncameranode.h"
#include "scene/nclippingcameranode.h"
#include "scene/nmaterialnode.h"

//------------------------------------------------------------------------------
/**
    Render the scenes for each camera
*/
void
nSceneServer::RenderCameraScene()
{
    PROFILER_START(this->profRenderCameras);
    nGfxServer2* gfxServer = nGfxServer2::Instance();
    int i;
    for (i = 0; i < this->cameraArray.Size(); i++)
    {
        // get the camera node
        Group& cameraNodeGroup = this->groupArray[cameraArray[i]];
        nAbstractCameraNode* cameraNode = (nAbstractCameraNode*) cameraNodeGroup.sceneNode;

        // check if the render target available
        const nString& rpSectionName = cameraNode->GetRenderPathSection();
        int sectionIndex = this->renderPath.FindSectionIndex(rpSectionName);
        if (-1 != sectionIndex)
        {
            // update camera
            cameraNode->RenderCamera(cameraNodeGroup.modelTransform,
                                    nGfxServer2::Instance()->GetTransform(nGfxServer2::View),
                                    nGfxServer2::Instance()->GetTransform(nGfxServer2::Projection));

            // temp view- and projection matrix
            gfxServer->PushTransform(nGfxServer2::View, cameraNode->GetViewMatrix());
            gfxServer->PushTransform(nGfxServer2::Projection, cameraNode->GetProjectionMatrix());

            // perform rendering through the render path
            this->DoRenderPath(this->renderPath.GetSection(sectionIndex));

            // restore matrices
            gfxServer->PopTransform(nGfxServer2::Projection);
            gfxServer->PopTransform(nGfxServer2::View);
        }
    }
    PROFILER_STOP(this->profRenderCameras);
}

//------------------------------------------------------------------------------
/**
    This method parses the given node for one reflection camera and one
    clippingnode, returns true if this node seems to be a reflecting and
    refraction one
*/
bool
nSceneServer::IsAReflectingShape(const nMaterialNode *shapeNode)  const
{
    // check if shape has child
    nRoot* child = shapeNode->GetHead();

    // return if no child
    if (0 == child)
    {
        return false;
    }

    // check variables
    bool reflect1Found = false;
    bool refract1Found = false;
    bool refract2Found = false;

    while (0 != child && !(reflect1Found && refract1Found && refract2Found))
    {
        // check
        n_assert(child);

        // check if this child is a reflection camera (a reflection camera is a refraction(clipping) camera too!!!)
        if (child->IsA(reqReflectClass))
        {
            reflect1Found = true;

            // check if this child is a refraction(clipping) camera
            if (child->IsA(reqRefractClass))
            {
                refract1Found = true;
            }
        }
        // check if this is a pure refraction(clipping) camer
        else if (child->IsA(reqRefractClass))
        {
            refract2Found = true;
        }
        // if the two cams are not the only childs, this is no reflectin shape
        else
        {
            return false;
        }

        // get next child
        child = child->GetSucc();
    }

    // return if we found both
    return reflect1Found && refract1Found && refract2Found;
}


//------------------------------------------------------------------------------
/**
    checks the given node's bounding box if it is visible or not (inside viewfrustum)
*/
bool
nSceneServer::IsShapesBBVisible(const Group& groupNode)
{
    // get view frustum as matrix
    const matrix44& viewProj = nGfxServer2::Instance()->GetTransform(nGfxServer2::ViewProjection);

    // get bounding box
    bbox3 bBox = groupNode.sceneNode->GetLocalBox();
    bBox.transform(groupNode.modelTransform);

    // compute clipping for box
    int clipStat = bBox.clipstatus(viewProj);
    return (clipStat != 0);
}

//------------------------------------------------------------------------------
/**
    calculates the distance from the bounding box of this element to viewer
*/
float
nSceneServer::CalculateDistanceToBoundingBox(const Group& groupNode)
{
    // get bounding box
    bbox3 bBox = groupNode.sceneNode->GetLocalBox();
    bBox.transform(groupNode.modelTransform);

    // reduce to 4 lines (4 vertices)
    vector3 v1 = bBox.corner_point(0);
    vector3 v2 = bBox.corner_point(3);
    vector3 v3 = bBox.corner_point(6);
    vector3 v4 = bBox.corner_point(7);

    // build 4 lines
    line3 l12(v1, v2);
    line3 l24(v2, v4);
    line3 l43(v4, v3);
    line3 l31(v3, v1);

    // get the distances, if 0 < tVal < 1 then everything is allright, if not
    // calc position to the corner vertex of bounding box
    float retVal;
    float tVal = l12.closestpoint(viewerPos);
    if (tVal < 0.0f)
    {
        retVal = vector3::distance(viewerPos, v1);
    }
    else if (tVal > 1.0f)
    {
        retVal = vector3::distance(viewerPos, v2);
    }
    else
    {
        retVal = l12.distance(viewerPos);
    }

    // get next to compare
    float tempVal;
    tVal = l24.closestpoint(viewerPos);
    if (tVal < 0.0f)
    {
        tempVal = vector3::distance(viewerPos, v2);
    }
    else if (tVal > 1.0f)
    {
        tempVal = vector3::distance(viewerPos, v4);
    }
    else
    {
        tempVal = l24.distance(viewerPos);
    }

    // compare
    retVal = tempVal < retVal ? tempVal : retVal;

    // get next distance to next line
    tVal = l43.closestpoint(viewerPos);
    if (tVal < 0.0f)
    {
        tempVal = vector3::distance(viewerPos, v4);
    }
    else if (tVal > 1.0f)
    {
        tempVal = vector3::distance(viewerPos, v3);
    }
    else
    {
        tempVal = l43.distance(viewerPos);
    }

    // compare
    retVal = tempVal < retVal ? tempVal : retVal;

    // get next to compare
    tVal = l31.closestpoint(viewerPos);
    if (tVal < 0.0f)
    {
        tempVal = vector3::distance(viewerPos, v3);
    }
    else if (tVal > 1.0f)
    {
        tempVal = vector3::distance(viewerPos, v1);
    }
    else
    {
        tempVal = l31.distance(viewerPos);
    }

    // compare
    retVal = tempVal < retVal ? tempVal : retVal;

    // calc dist
    return retVal;
}

//------------------------------------------------------------------------------
/**
    Parses the given node (usually a relecting, refracting sea) for its priority.
    If this sea gets the highest priority, the member 'renderedReflectorPtr' will be set
    to this one, and the distance 'renderedReflectorDistance' will be set.
    Returns true if the given node gains complex render priority
*/
bool
nSceneServer::ParsePriority(const Group& groupNode)
{
    // check if the given shape is visible
    bool visible = true;// this->IsShapesBBVisible(groupNode);

    // do only if it is                                  //////////////////////////////// FIRST CONDITION - visibility
    if (true == visible)
    {
        // get actual distance viewer<->edge of bounding box
        float tempDistance = CalculateDistanceToBoundingBox(groupNode);

        // if there is no chosen one                     //////////////////////////////// SECOND CONDITION - no other assigned
        if (0 == this->renderContextPtr)
        {
            // this is the new one
            this->renderContextPtr = groupNode.renderContext;
            this->renderedReflectorDistance = tempDistance;
            return true;
        }

        // if this is allready the chosen one to render  //////////////////////////////// THIRD CONDITION - not allready assigned
        if (this->renderContextPtr == groupNode.renderContext)
        {
            // assign new distance
            this->renderedReflectorDistance = tempDistance;

            // nothing else to do
            return true;
        }

        // if this is not the chosen one to render, check if it will be new one
        if (tempDistance < this->renderedReflectorDistance) ///////////////////////////// FOURTH CONDITION - nearer then assigned OR assigned is invisible
        {
            // assign new nearest sea object
            this->renderedReflectorDistance = tempDistance;
            this->renderContextPtr = groupNode.renderContext;

            return true;
        }
    }
    else
    {
        // if this was the assigned reflector
        if (groupNode.renderContext == this->renderContextPtr)
        {
            // this is no longer the chosen one
            this->renderContextPtr = 0;
            return false;
        }
    }

    // we should never come here
    return false;
}
