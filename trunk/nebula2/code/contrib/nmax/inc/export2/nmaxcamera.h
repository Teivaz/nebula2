//---------------------------------------------------------------------------
//  nmaxcamera.cc
//
//  (c)2004 Kim, Hyoun Woo
//---------------------------------------------------------------------------
#ifndef N_MAXCAMERA_H
#define N_MAXCAMERA_H
//---------------------------------------------------------------------------
/**
    @class nMaxCamera
    @ingroup NebulaMaxExport2Contrib

    @brief A class for exporting camera data of 3DS Max.

*/
#include "export2/nmaxnode.h"

//---------------------------------------------------------------------------
class nMaxCamera : public nMaxNode
{
public:
    nMaxCamera();
    virtual ~nMaxCamera();

    void Export(INode* inode, Object *obj);

protected:

};
//---------------------------------------------------------------------------
#endif
