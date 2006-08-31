//------------------------------------------------------------------------------
//  util/segmentedgfxutil.cc
//  (C) 2005 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "util/segmentedgfxutil.h"
#include "graphics/server.h"
#include "scene/ntransformnode.h"
#include "foundation/factory.h"

namespace Util
{
//------------------------------------------------------------------------------
/**
*/
SegmentedGfxUtil::SegmentedGfxUtil() :
    resourceCache(128, 128)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nArray<Ptr<Graphics::Entity> >
SegmentedGfxUtil::CreateAndSetupGraphicsEntities(const nString& resName, const matrix44& worldMatrix, bool attachToLevel)
{
    Graphics::Level* graphicsLevel = Graphics::Server::Instance()->GetLevel();
    n_assert(graphicsLevel);
    nArray<Ptr<Graphics::Entity> > graphicsEntities;

    // create a resource object
    Ptr<Graphics::Resource> resource = Graphics::Resource::Create();
    resource->SetName(resName);
    resource->Load();
    if (!attachToLevel)
    {
        this->resourceCache.Append(resource);
    }

    // collect segment hierarchy nodes from resource
    nTransformNode* modelNode = (nTransformNode*) resource->GetNode()->Find("model");
    if (modelNode)
    {
        n_assert(modelNode);
        nArray<nRef<nTransformNode> > segments;
        nTransformNode* curNode;
        nClass* tformNodeClass = nKernelServer::Instance()->FindClass("ntransformnode");
        n_assert(tformNodeClass);
        for (curNode = (nTransformNode*) modelNode->GetHead(); curNode; curNode = (nTransformNode*) curNode->GetSucc())
        {
            if (curNode->IsA(tformNodeClass) && curNode->HasHints(nSceneNode::LevelSegment))
            {
                segments.Append(curNode);
            }
        }

        // check if segments actually exist
        if (segments.Size() > 0)
        {
            nTransformNode* shadowNode = (nTransformNode*) resource->GetNode()->Find("shadow");

            // create one graphics entity for each segment
            int num = segments.Size();
            int i;
            for (i = 0; i < num; i++)
            {
                // build a resource name for the segment
                nString segResName = resName;
                segResName.TrimRight("/");
                segResName.Append("/model/");
                segResName.Append(segments[i]->GetName());

                // create and setup graphics entity
				Ptr<Graphics::Entity> ge = Graphics::Entity::Create();
                ge->SetResourceName(segResName);
                ge->SetTransform(worldMatrix);

                // see if a shadow node of the same name exists
                if (shadowNode && shadowNode->Find(segments[i]->GetName()))
                {
                    nString shdResName = resName;
                    shdResName.Append("/shadow/");
                    shdResName.Append(segments[i]->GetName());
                    ge->SetShadowResourceName(shdResName);
                }
                if (attachToLevel)
                {
                    graphicsLevel->AttachEntity(ge);
                }
                graphicsEntities.Append(ge);
            }
            return graphicsEntities;
        }
    }

    // fallthrough: don't create segments
	Ptr<Graphics::Entity> ge = Graphics::Entity::Create();
    ge->SetResourceName(resName);
    ge->SetTransform(worldMatrix);
    if (attachToLevel)
    {
        graphicsLevel->AttachEntity(ge);
    }
    graphicsEntities.Append(ge);

    return graphicsEntities;
}

} // namespace Util
