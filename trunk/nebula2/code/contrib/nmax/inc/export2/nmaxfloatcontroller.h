//-----------------------------------------------------------------------------
//
//
//
//-----------------------------------------------------------------------------
#ifndef N_MAXFLOATCONTROLLER_H
#define N_MAXFLOATCONTROLLER_H 
//-----------------------------------------------------------------------------
/**
    @class nMaxFloatController
    @ingroup

    @brief A class for handling single value in controller.
*/
#include "util/narray.h"

class nMaxController;

//-----------------------------------------------------------------------------
class nMaxFloatController : public nMaxController
{
public:
    class SampleKey
    {
    public:
        float time;
        float key;
    };
    
    nMaxFloatController();
    virtual ~nMaxFloatController();

    virtual void Export(Control *control);

    int GetNumKeys() const;
    nArray<SampleKey>& GetKeyArray();

protected:
    void ExportTCBFloat(IKeyControl* keyControl);
    void ExportLinearFloat(IKeyControl* keyControl);
    void ExportHybridFloat(IKeyControl* keyControl);
    void ExportDefaultFloat(Control* control);

protected:
    nArray<SampleKey> keyArray;

};
//-----------------------------------------------------------------------------
inline
int nMaxFloatController::GetNumKeys() const
{
    return this->keyArray.Size();
}
//-----------------------------------------------------------------------------
inline
nArray<nMaxFloatController::SampleKey>& nMaxFloatController::GetKeyArray()
{
    return this->keyArray;
}

#endif
