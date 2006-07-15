#ifndef N_WX_ANIMLIST_CONTROL_H
#define N_WX_ANIMLIST_CONTROL_H
//----------------------------------------------------------------------------
#include "wx/wx.h"
#include "wx/listctrl.h"
#include "util/narray.h"

class nLWAnimationState;

//----------------------------------------------------------------------------
/**
    @class wxAnimListCtrl
    @brief Custom multi-column list control to display animation state info.
*/
class wxAnimListCtrl : public wxListCtrl
{
public:
    wxAnimListCtrl(wxWindow* parent, wxWindowID id, 
                   const wxPoint& pos = wxDefaultPosition, 
                   const wxSize& size = wxDefaultSize);
    virtual ~wxAnimListCtrl();

    void ClearAnims();
    void AddAnim(const nLWAnimationState&);
    void RemoveAnim(int rowIdx);
    void MoveAnim(int rowToMove, int numRowsToMoveBy);
    void SetAnim(int rowIdx, const nLWAnimationState&);
    bool GetAnim(int rowIdx, nLWAnimationState&) const;
    void SelectAnim(int rowIdx);
    int GetSelectedAnimIdx() const;
    virtual wxString OnGetItemText(long rowIdx, long columnIdx) const;

private:
    nArray<nLWAnimationState*> rows;
};

//----------------------------------------------------------------------------
#endif // N_WX_ANIMLIST_CONTROL_H
