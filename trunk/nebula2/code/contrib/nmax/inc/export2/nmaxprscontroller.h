#ifndef N_MAXPRSCONTROLLER_H
#define N_MAXPRSCONTROLLER_H
//-----------------------------------------------------------------------------
/**
    @class nMaxPRSController
    @ingroup 

    @brief A class for handling Position/Rotation/Scale(PRS)

*/

class nMaxController;
class nTransformAnimator;

//-----------------------------------------------------------------------------
class nMaxPRSController : public nMaxController
{
public:
    nMaxPRSController();
    virtual ~nMaxPRSController();

    virtual void Export(INode* inode);

protected:
	virtual void ExportPosition(Control *control, nTransformAnimator* animator);
	virtual void ExportRotation(Control *control, nTransformAnimator* animator);
	virtual void ExportScale(Control *control, nTransformAnimator* animator);

	virtual void ExportTCBPosition();
	virtual void ExportHybridPosition();
	virtual void ExportLinearPosition();
    virtual void ExportDefaultPosition(INode *inode);
	
    bool HasSampledKeys(Control *control);

protected:
    INode* maxNode;

};
//-----------------------------------------------------------------------------
#endif
