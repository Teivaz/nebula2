#ifndef N_MAXIKCONTROLLER_H
#define N_MAXIKCONTROLLER_H

//-----------------------------------------------------------------------------
/**
    @class nMaxIKController
    @ingroup

    @brief A class for handling Inverse Kinemetics(IK).
*/
class nMaxController;
class nMaxPRSController;

//-----------------------------------------------------------------------------
class nMaxIKController : public nMaxPRSController
{
public:
    nMaxIKController();
    virtual ~nMaxIKController();

    virtual void Export(INode *inode);
protected:
    

};
//-----------------------------------------------------------------------------
#endif
