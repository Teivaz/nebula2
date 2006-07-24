//-----------------------------------------------------------------------------
//  nmaxdummy.cc
//
//  (c)2004 Kim, Hyoun Woo.
//-----------------------------------------------------------------------------
#include "export2/nmax.h"
#include "export2/nmaxdummy.h"
#include "export2/nmaxbones.h"
#include "export2/nmaxutil.h"
#include "export2/nmaxcustattrib.h"
#include "pluginlibs/nmaxdlg.h"
#include "pluginlibs/nmaxlogdlg.h"

#include "scene/ntransformnode.h"
#include "scene/nlodnode.h"
#include "scene/nattachmentnode.h"

//-----------------------------------------------------------------------------
/**
*/
nMaxDummy::nMaxDummy()
{
}

//-----------------------------------------------------------------------------
/**
*/
nMaxDummy::~nMaxDummy()
{
}

//-----------------------------------------------------------------------------
/**
    @note
        Do not create more than one nSceneNode derived node from the given node.
*/
nSceneNode* nMaxDummy::Export(INode* inode)
{
    nSceneNode* createdNode = 0;

    createdNode = this->CreateNodeFromCustAttrib(inode);

    if (createdNode)
    {
        n_maxlog(Medium, "%s node has custom attributes.", inode->GetName());
    }
    else
    {
        // special case of a dummy node.
        if (inode->IsGroupHead())
        {
            // the node is group owner node.
            // If this node is a group owner node(this is a dummy node in max), add
            // a transform node, so we can handle the group complete later)
            nTransformNode* transformNode;
            transformNode = static_cast<nTransformNode*>(CreateNebulaObject("ntransformnode",
                                                                            inode->GetName()));
            createdNode = transformNode;
        }
    }

    return createdNode;
}

//-----------------------------------------------------------------------------
/**
    Creates Nebula node from the given node which has corresponding custom
    attributes for the node.
*/
nSceneNode*  nMaxDummy::CreateNodeFromCustAttrib(INode* inode)
{
    TiXmlDocument xmlDoc;

    Object* obj = nMaxUtil::GetBaseObject(inode, 0);
    if (!obj)
    {
        n_maxlog(Medium, "The node %s has no object.", inode->GetName());
        return NULL;
    }

    // convert node custom attributes to xml data.
    nMaxCustAttrib custAttrib;
    if (!custAttrib.Convert(obj, xmlDoc))
    {
        n_maxlog(Medium, "The node %s has no custom attribute.", inode->GetName());
        return NULL;
    }

    TiXmlHandle xmlHandle(&xmlDoc);

    TiXmlElement* e;
    nSceneNode* createdNode = NULL;

    if (e = xmlHandle.FirstChild("LodParam").Element())
    {
        createdNode = ExportLODNode(inode, xmlHandle, "LodParam");
    }
    else
    if (e = xmlHandle.FirstChild("AttachParam").Element())
    {
        createdNode = ExportAttachmentNode(inode, xmlHandle, "AttachParam");
    }
    else
    {
        // the given node has unknown custom attribute.
        n_maxlog(Warning, "Warning: The dummy node '%s' has unknown custom attributes.", inode->GetName());
    }

    return createdNode;
}

//-----------------------------------------------------------------------------
/**
    The given node has custom attributes for LOD node so, create nLodNode from
    its custom attributes.
*/
nSceneNode* nMaxDummy::ExportLODNode(INode *inode, TiXmlHandle &xmlHandle, const char* paramName)
{
    nString objectname = inode->GetName();
    objectname += nMaxUtil::CorrectName(objectname);

    nLodNode* lodNode = static_cast<nLodNode*>(CreateNebulaObject("nlodnode", objectname.Get()));
    if (lodNode)
    {
        TiXmlElement* child;

        double value;

        // get the mesh type
        child = xmlHandle.FirstChild(paramName).FirstChild("threshold").Child("", 0).Element();
        if (child)
        {
            child->Attribute("value", &value);

            // specifies LOD treshold.
            lodNode->AppendThreshold((float)value);
        }

        child = xmlHandle.FirstChild(paramName).FirstChild("mindistance").Child("", 0).Element();
        if (child)
        {
            child->Attribute("value", &value);

            // specifies LOD min distance.
            lodNode->SetMinDistance((float)value);
        }

        child = xmlHandle.FirstChild(paramName).FirstChild("maxdistance").Child("", 0).Element();
        if (child)
        {
            child->Attribute("value", &value);
            lodNode->SetMaxDistance((float)value);
        }
    }
    else
    {
        n_maxlog(Error, "Error: Failed to create LOD node for the dummy node '%s'.", inode->GetName());
    }

    return lodNode;
}

//-----------------------------------------------------------------------------
/**
    The given node has custom attributes for attachment node so, create
    nAttachmentNode from its custom attributes.
*/
nSceneNode* nMaxDummy::ExportAttachmentNode(INode *inode, TiXmlHandle &xmlHandle, const char* paramName)
{
    nSceneNode* createdNode = NULL;

    TiXmlElement* child;
    if (child = xmlHandle.FirstChild(paramName).FirstChild("attachmentjoint").Child("", 0).Element())
    {
        nString boneName = child->Attribute("value");

        if (!boneName.IsEmpty())
        {
            nString objectname;
            objectname += nMaxUtil::CorrectName(inode->GetName());

            nAttachmentNode* attachNode = static_cast<nAttachmentNode*>(CreateNebulaObject("nattachmentnode",
                objectname.Get()));
            if (attachNode)
            {
                INode* parent = inode->GetParentNode();
                if (parent)
                {

                    int boneID = nMaxBoneManager::Instance()->FindBoneIDByName(boneName);
                    if (boneID >= 0)
                    {
                        //specifies a joint of the attachment node.
                        attachNode->SetJointByIndex(boneID);
                    }
                    else
                    {
                        n_maxlog(Warning, "Warning:  The attachment node %s should be child node of any other bone node.",
                            inode->GetName());

                        nKernelServer::Instance()->PopCwd();
                        attachNode->Release();
                    }
                }
                else
                {
                    n_maxlog(Medium, "The attachment node %s has no parent node. It is deleted.", inode->GetName());

                    nKernelServer::Instance()->PopCwd();
                    attachNode->Release();
                    attachNode = 0;
                }
                createdNode = attachNode;
            }
        }
        else
        {
            n_maxlog(Error, "Error: Any bone node is not selected. You should select one of the bones for joint of the attachment node '%s'.", inode->GetName());
        }
    }

    return createdNode;
}