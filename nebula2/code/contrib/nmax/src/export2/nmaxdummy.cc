//-----------------------------------------------------------------------------
//  nmaxdummy.cc
//
//  (c)2004 Kim, Hyoun Woo.
//-----------------------------------------------------------------------------
#include "export2/nmax.h"
#include "export2/nmaxdummy.h"
#include "export2/nmaxutil.h"
#include "export2/nmaxcustattrib.h"

#include "pluginlibs/nmaxdlg.h"
#include "pluginlibs/nmaxlogdlg.h"

#include "scene/ntransformnode.h"
#include "scene/nlodnode.h"
#include "tinyxml/tinyxml.h"

//-----------------------------------------------------------------------------
/**
*/
nMaxDummy::nMaxDummy() :
    isLOD(false),
    threshold(100.0f),
    minDistance(-100.0f),
    maxDistance(100.0f)
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
*/
nSceneNode* nMaxDummy::Export(INode* inode)
{
    nSceneNode* createdNode = 0;

    if (this->GetCustAttrib(inode))
    {
        n_maxlog(Midium, "%s node has custom attributes.", inode->GetName());
    }

    // the node is group owner node.
    if (inode->IsGroupHead())
    {
        //If this node is a group owner node(this is a dummy node in max), add a transform node,
        //so we can handle the group complete later)
        nTransformNode* transformNode;
        //transformNode = static_cast<nTransformNode*>(nMaxSceneObjFactory::Create(inode->GetName(), "ntransformnode"));
        transformNode = static_cast<nTransformNode*>(CreateNebulaObject("ntransformnode", inode->GetName()));
        if (transformNode)
        {
            // transform.
        }

        createdNode = transformNode;
    }

    // the node is LOD dummy node.
    if (this->isLOD)
    {
        nLodNode* lodNode = static_cast<nLodNode*>(CreateNebulaObject("nlodnode", inode->GetName()));
        
        lodNode->AppendThreshold(this->threshold);
        lodNode->SetMinDistance(this->minDistance);
        lodNode->SetMaxDistance(this->maxDistance);

        createdNode = lodNode;
    }

    return createdNode;
}

//-----------------------------------------------------------------------------
/**
*/
bool nMaxDummy::GetCustAttrib(INode* inode)
{
    TiXmlDocument xmlDoc;

    Object* obj = nMaxUtil::GetBaseObject(inode, 0);
    if (!obj)
    {
        n_maxlog(Midium, "The node %s has no object.", inode->GetName());
        return false;
    }

    // convert node custom attributes to xml data.
    nMaxCustAttrib custAttrib;
    if (!custAttrib.Convert(obj, xmlDoc))
    {
        n_maxlog(Midium, "The node %s has no custom attribute.", inode->GetName());
        return false;
    }

    TiXmlHandle xmlHandle(&xmlDoc);

    TiXmlElement* e;

    // parameter name of LOD
    const char* lodParamName = "LodParam";

    e = xmlHandle.FirstChild(lodParamName).Element();
    if (e)
    {
        this->isLOD = true;

        TiXmlElement* child;

        double value;

        // get the mesh type
        child = xmlHandle.FirstChild(lodParamName).FirstChild("threshold").Child("", 0).Element();
        if (child)
        {
            child->Attribute("value", &value);
            this->threshold = (float)value;
        }

        child = xmlHandle.FirstChild(lodParamName).FirstChild("mindistance").Child("", 0).Element();
        if (child)
        {
            child->Attribute("value", &value);
            this->minDistance = (float)value;
        }

        child = xmlHandle.FirstChild(lodParamName).FirstChild("maxdistance").Child("", 0).Element();
        if (child)
        {
            child->Attribute("value", &value);
            this->maxDistance = (float)value;
        }
    }
    else
    {
        ;
    }

    return true;
}