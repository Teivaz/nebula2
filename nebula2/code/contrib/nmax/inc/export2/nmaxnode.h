//-----------------------------------------------------------------------------
//  nmaxnode.h
//
//  (c)2004 Kim, Hyoun Woo.
//-----------------------------------------------------------------------------
#ifndef N_MAXNODE_H
#define N_MAXNODE_H
//-----------------------------------------------------------------------------
/**
    @class nMaxNode
    @ingroup NebulaMaxExport2Contrib

    @brief
*/
class nSceneNode;

//-----------------------------------------------------------------------------
class nMaxNode
{
public:
    nMaxNode();
    virtual ~nMaxNode();


protected:
    virtual nSceneNode* CreateNebulaObject(const char* classname, const char* objectname);
};
//-----------------------------------------------------------------------------
#endif