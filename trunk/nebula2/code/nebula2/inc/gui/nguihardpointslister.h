#ifndef N_GUIHARDPOINTSLISTER_H
#define N_GUIHARDPOINTSLISTER_H
//------------------------------------------------------------------------------
/**
    @class nGuiHardpointsLister
    @ingroup Gui

    A widget to list the Hardpoints of all Elements in nNodesList

    (C) 2005 RadonLabs GmbH
*/
#include "gui/nguitextview.h"
#include "scene/nskinanimator.h"
#include "tools/nnodelist.h"

//------------------------------------------------------------------------------
class nGuiHardpointsLister : public nGuiTextView
{
public:
    /// constructor
    nGuiHardpointsLister();
    /// destructor
    virtual ~nGuiHardpointsLister();
    /// called when widget is becoming visible
    virtual void OnShow();
    /// called per frame when parent widget is visible
    virtual void OnFrame();
    /// gets the selected Hardpoint-Joint (nSkinAnimator + Index)
    bool GetSelectedJoint(nSkinAnimator* &skinAnimator,int &index);

protected:
    /// update text list from directory content
    void UpdateContent();

    void FindHardpointsAndAddToList(nRoot* node);

    bool dirty;
    nArray<nSkinAnimator*>   animatorList;
    nArray<int>              jointIndex;
};
//------------------------------------------------------------------------------
#endif