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
    /// Constructor.
    nMaxDummy();
    /// Destructor.
    virtual ~nMaxDummy();

    ///
    nSceneNode* Export(INode* inode);

protected:
    ///
    nSceneNode* CreateNodeFromCustAttrib(INode* inode);

    ///
    nSceneNode* ExportLODNode(INode *inode, TiXmlHandle &xmlHandle, const char* paramName);

protected:

};
//-----------------------------------------------------------------------------
#endif
