//------------------------------------------------------------------------------
//  nlodnode_main.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "scene/nlodnode.h"
#include "scene/nsceneserver.h"
#include "gfx2/ngfxserver2.h"

nNebulaScriptClass(nLodNode, "ntransformnode");

//------------------------------------------------------------------------------
/**
*/
nLodNode::nLodNode() :
    minDistance(-100.0f),
    maxDistance(100.0f)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nLodNode::~nLodNode()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Attach to the scene server.
*/
void
nLodNode::Attach(nSceneServer* sceneServer, nRenderContext* renderContext)
{
    n_assert(sceneServer);
    n_assert(renderContext);

    if (this->CheckFlags(Active))
    {
        // get camera distance
        const matrix44& viewer = nGfxServer2::Instance()->GetTransform(nGfxServer2::InvView);
	    vector3 lodViewer = viewer.pos_component() - this->tform.gettranslation();
        float distance = lodViewer.len();

        if(distance > this->minDistance && distance < this->maxDistance)
        {
            // get number of child nodes
            int num = 0;
            nSceneNode* curChild;
            for (curChild = (nSceneNode*) this->GetHead();
                curChild;
                curChild = (nSceneNode*) curChild->GetSucc())
            {
                num++;
            }

            // if there are not enough thresholds, set some default values
            if(!this->thresholds.Size()) thresholds.PushBack(100.0f);
            while(this->thresholds.Size() < num - 1)
            {
                this->thresholds.PushBack( thresholds[this->thresholds.Size()-1] * 2.0f );
            }

            // find the proper child to attach
            int index = 0;
            if(this->GetHead())
            {
                nSceneNode* childToAttach = (nSceneNode*) this->GetHead(); 
                for (curChild = (nSceneNode*) childToAttach->GetSucc();
                    curChild;
                    curChild = (nSceneNode*) curChild->GetSucc())
                {
                    if(distance >= this->thresholds[index++])
                    {
                        childToAttach = curChild;
                    }
                }     
                childToAttach->Attach(sceneServer, renderContext);
            }
        }
    }
}
