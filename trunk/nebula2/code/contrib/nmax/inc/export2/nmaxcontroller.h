#ifndef N_MAXCONTROLLER_H
#define N_MAXCONTROLLER_H
//-----------------------------------------------------------------------------
/**
    @class nMaxController
    @ingroup

    @brief A wrapper class for 3dsmax controller.

*/

//-----------------------------------------------------------------------------
class nMaxController
{
public:
	enum Type 
	{
		TCBFloat,
		TCBPosition,
		TCBRotation,
		TCBScale,
        TCBPoint3,
		HybridFloat,
		HybridPosition,
		HybridRotation,
		HybridScale,
        HybridPoint3,
		LinearFloat,
		LinearPosition,
		LinearRotation,
		LinearScale,
        Unknown,
	};

    nMaxController();
    virtual ~nMaxController();

    //virtual void Export(INode *inode);

	Type GetType(Control *control);

};
//-----------------------------------------------------------------------------
#endif
