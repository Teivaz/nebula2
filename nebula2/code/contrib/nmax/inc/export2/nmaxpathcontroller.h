#ifndef N_MAXPATHCONTROLLER_H
#define N_MAXPATHCONTROLLER_H
//-----------------------------------------------------------------------------
/**
    @class nMaxPathController
    @ingroup

    @brief A class for handling path in 3dsmax.
           (This handles procedure controllers of 3dsmax)
*/
class nMaxController;

//-----------------------------------------------------------------------------
class nMaxPathController : public nMaxController
{
public:
    nMaxPathController();
    virtual ~nMaxPathController();

    virtual void Export(Control *control);
protected:
    

};
//-----------------------------------------------------------------------------
#endif
