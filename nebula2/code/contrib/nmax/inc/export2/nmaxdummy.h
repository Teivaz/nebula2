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

class nSceneNode;

//-----------------------------------------------------------------------------
class nMaxDummy : public nMaxNode
{
public:
    nMaxDummy();
    virtual ~nMaxDummy();

    nSceneNode* Export(INode* inode);

protected:
    bool GetCustAttrib(INode* inode);

protected:
    bool isLOD;

    float threshold;
    float minDistance;
    float maxDistance;

};
//-----------------------------------------------------------------------------
#endif
