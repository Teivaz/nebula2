//-----------------------------------------------------------------------------
//  nmaxdummy.h
//
//  (c)2004 Kim, Hyoun Woo.
//-----------------------------------------------------------------------------
#ifndef N_MAXDUMMY_H
#define N_MAXDUMMY_H
//-----------------------------------------------------------------------------
/**
    @class nMaxDummy
    @ingroup NebulaMaxExport2Contrib

    @brief
*/
#include "export2/nmaxnode.h"
#include "tinyxml/tinyxml.h"

class nSceneNode;

//-----------------------------------------------------------------------------
class nMaxDummy : public nMaxNode
{
public:
    nMaxDummy();
    virtual ~nMaxDummy();

    nSceneNode* Export(INode* inode);

protected:
    nSceneNode* CreateNodeFromCustAttrib(INode* inode);

    nSceneNode* ExportAttachmentNode(INode *inode, TiXmlHandle &xmlHandle, const char* paramName);
    nSceneNode* ExportLODNode(INode *inode, TiXmlHandle &xmlHandle, const char* paramName);

protected:

};
//-----------------------------------------------------------------------------
#endif
