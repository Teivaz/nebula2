//-----------------------------------------------------------------------------
//
//
//
//-----------------------------------------------------------------------------
#ifndef N_MAXPOINT3CONTROLLER_H
#define N_MAXPOINT3CONTROLLER_H
//-----------------------------------------------------------------------------
/**
    @class nMaxPoint3Controller
    @ingroup

    @brief A class for handling Point3 value of the specific controller.
           Usually this is used for extracting color of shader animation.
*/
#include "mathlib/vector.h"
#include "util/narray.h"

class nMaxController;

//-----------------------------------------------------------------------------
class nMaxPoint3Controller : public nMaxController
{
public:
    class SampleKey
    {
    public:
        float time;
        vector3 key;
    };

    nMaxPoint3Controller();
    virtual ~nMaxPoint3Controller();

    int GetNumKeys() const;
    nArray<SampleKey>& GetKeyArray();

    virtual void Export(Control *control);

protected:
    void ExportTCBPoint3(Control* control);
    void ExportHybridPoint3(Control* control);
    void ExportDefaultPoint3(Control* control);

protected:
    nArray<SampleKey> keyArray;

};
//-----------------------------------------------------------------------------
inline
int 
nMaxPoint3Controller::GetNumKeys() const
{
    return this->keyArray.Size();
}
//-----------------------------------------------------------------------------
inline
nArray<nMaxPoint3Controller::SampleKey>& 
nMaxPoint3Controller::GetKeyArray()
{
    return this->keyArray;
}

#endif
