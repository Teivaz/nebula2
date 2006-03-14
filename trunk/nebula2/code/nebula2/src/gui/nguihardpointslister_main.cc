//------------------------------------------------------------------------------
//  nguihardpointslister_main.cc
//  (C) 2005 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gui/nguihardpointslister.h"
#include "kernel/nfileserver2.h"
#include "kernel/ndirectory.h"
#include "util/nstring.h"

nNebulaClass(nGuiHardpointsLister, "nguitextview");

//------------------------------------------------------------------------------
/**
*/
nGuiHardpointsLister::nGuiHardpointsLister() :
    dirty(true)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nGuiHardpointsLister::~nGuiHardpointsLister()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiHardpointsLister::OnShow()
{
    this->UpdateContent();
    nGuiTextView::OnShow();
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiHardpointsLister::OnFrame()
{
    if (this->dirty)
    {
        this->UpdateContent();
    }
    nGuiTextView::OnFrame();
}

//------------------------------------------------------------------------------
/**
    Find Hardpoints and add them to the list
*/
void nGuiHardpointsLister::FindHardpointsAndAddToList(nRoot* node)
{
    nRoot* subNode = node->GetHead();
    while( 0 != subNode )
    {
        if( strcmp(subNode->GetName(),"skinanimator") == 0)
        {
            nSkinAnimator* skinAnimator = (nSkinAnimator*) subNode;
            int numJoints = skinAnimator->GetNumJoints();
            
            int j;
            for( j = 0 ; j < numJoints ; j++ )
            {
                int         parentJointIndex;
                vector3     poseTranslate;
                quaternion  poseRotate;
                vector3     poseScale;
                nString     name;
    
                skinAnimator->GetJoint(j,parentJointIndex,poseTranslate,poseRotate,poseScale,name);
                animatorList.Append(skinAnimator);
                jointIndex.Append(j);
                this->AppendLine(name);
            };
        };
        this->FindHardpointsAndAddToList(subNode);
        subNode = subNode->GetSucc();
    };
};

//------------------------------------------------------------------------------
/**
  Gets the selected Hardpoint-Joint (nSkinAnimator + Index)
*/
bool nGuiHardpointsLister::GetSelectedJoint(nSkinAnimator* &skinAnimator,int &index)
{
    if(this->GetNumLines() == 0) 
        return false;

    int nr = this->GetSelectionIndex();
    skinAnimator = animatorList.At(nr);
    index = jointIndex.At(nr);

    return true;
};


//------------------------------------------------------------------------------
/**
    Update the text view with content of directory.
*/
void
nGuiHardpointsLister::UpdateContent()
{
    nNodeList* nodeList = nNodeList::Instance();
    n_assert( nodeList != 0 );
    
    int nodesCount = nodeList->GetCount();

    this->BeginAppend();
    
    int i;
    for( i=0 ; i<nodesCount ; i++)
    {
        nTransformNode* node = nodeList->GetNodeAt(i);
        this->FindHardpointsAndAddToList(node);
    };

    this->EndAppend();
    this->SetLineOffset(0);
    this->dirty = false;
}
