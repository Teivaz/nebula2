//----------------------------------------------------------------------------
// (c) 2006    Vadim Macagon
//----------------------------------------------------------------------------
#include "lwwxui/wxanimlistctrl.h"
#include "lwexporter/nlwanimationstate.h"

//----------------------------------------------------------------------------
enum AnimListBoxColumns
{
    ANIM_LIST_NAME_COL = 0,
    ANIM_LIST_START_COL,
    ANIM_LIST_END_COL,
    ANIM_LIST_FADEIN_COL,
    ANIM_LIST_LOOP_COL
};

//----------------------------------------------------------------------------
/**
*/
wxAnimListCtrl::wxAnimListCtrl(wxWindow* parent, wxWindowID id,
                               const wxPoint& pos, const wxSize& size) :
    wxListCtrl(parent, id, pos, size,
               wxLC_VIRTUAL|wxLC_REPORT|wxLC_SINGLE_SEL|wxLC_HRULES|wxLC_VRULES)
{
    this->InsertColumn(ANIM_LIST_NAME_COL, "State Name", wxLIST_FORMAT_LEFT, 180);
    this->InsertColumn(ANIM_LIST_START_COL, "Start", wxLIST_FORMAT_LEFT, 50);
    this->InsertColumn(ANIM_LIST_END_COL, "End", wxLIST_FORMAT_LEFT, 50);
    this->InsertColumn(ANIM_LIST_FADEIN_COL, "Fade-in", wxLIST_FORMAT_LEFT, 60);
    this->InsertColumn(ANIM_LIST_LOOP_COL, "Loop", wxLIST_FORMAT_LEFT, 40);
}

//----------------------------------------------------------------------------
/**
*/
wxAnimListCtrl::~wxAnimListCtrl()
{
    this->ClearAnims();
}

//----------------------------------------------------------------------------
/**
*/
void
wxAnimListCtrl::ClearAnims()
{
    for (int i = 0; i < this->rows.Size(); i++)
    {
        n_delete(this->rows[i]);
    }
    this->SetItemCount(0);
}

//----------------------------------------------------------------------------
/**
*/
void
wxAnimListCtrl::AddAnim(const nLWAnimationState& anim)
{
    nLWAnimationState* newAnim = n_new(nLWAnimationState);
    *newAnim = anim;
    this->rows.PushBack(newAnim);
    this->SetItemCount(this->rows.Size());
}

//----------------------------------------------------------------------------
/**
*/
void
wxAnimListCtrl::RemoveAnim(int row)
{
    n_assert(row < this->rows.Size());
    if (row < this->rows.Size())
    {
        n_delete(this->rows[row]);
        this->rows.Erase(row);
    }
    this->SetItemCount(this->rows.Size());
    // force a refresh if there are no items left in the list because the
    // list doesn't seem to refresh properly by itself
    if (this->rows.Empty())
    {
        this->Refresh();
    }
}

//----------------------------------------------------------------------------
/**
    @brief Move the given row up or down by the given number of rows.
    @param rowToMove Index of the row that should be moved.
    @param numRowsToMoveBy The number of rows to move the row by, if this is
                           negative the row will be moved up, if it's positive
                           the row will be moved down.
*/
void
wxAnimListCtrl::MoveAnim(int rowToMove, int numRowsToMoveBy)
{
    if (0 == numRowsToMoveBy)
        return;

    if (this->rows.Size() < 2)
        return;

    int destRowIdx = rowToMove + numRowsToMoveBy;
    if (destRowIdx == rowToMove)
        return;

    nLWAnimationState* srcRow = this->rows[rowToMove];
    if (numRowsToMoveBy > 0) // move down
    {
        if (destRowIdx > (this->rows.Size() - 1))
        {
            destRowIdx = this->rows.Size() - 1;
        }
        this->rows.Insert(destRowIdx + 1, srcRow);
        // the original row is still in the same place
        this->rows.Erase(rowToMove);
    }
    else // move up
    {
        if (destRowIdx < 0)
        {
            destRowIdx = 0;
        }
        this->rows.Insert(destRowIdx, srcRow);
        // the original row has been shifted back by one
        this->rows.Erase(rowToMove + 1);
    }

    this->RefreshItems(0, this->rows.Size() - 1);
}

//----------------------------------------------------------------------------
/**
*/
void
wxAnimListCtrl::SetAnim(int row, const nLWAnimationState& anim)
{
    n_assert(row < this->rows.Size());
    if (row < this->rows.Size())
    {
        *(this->rows[row]) = anim;
    }
    this->RefreshItem(row);
}

//----------------------------------------------------------------------------
/**
*/
bool
wxAnimListCtrl::GetAnim(int row, nLWAnimationState& anim) const
{
    n_assert(row < this->rows.Size());
    if (row < this->rows.Size())
    {
        anim = *(this->rows[row]);
        return true;
    }
    return false;
}

//----------------------------------------------------------------------------
/**
*/
void
wxAnimListCtrl::SelectAnim(int rowIdx)
{
    this->SetItemState(rowIdx, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
}

//----------------------------------------------------------------------------
/**
    @brief Get the row index of the currently selected animation in the
           list.
    @return Index of the selected animation, or -1 if no animation is selected.
*/
int
wxAnimListCtrl::GetSelectedAnimIdx() const
{
    return this->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
}

//----------------------------------------------------------------------------
/**
*/
wxString
wxAnimListCtrl::OnGetItemText(long rowIdx, long columnIdx) const
{
    nLWAnimationState* animState = this->rows[rowIdx];
    nString retVal;
    switch (columnIdx)
    {
        case ANIM_LIST_NAME_COL:
            retVal = animState->name;
        break;

        case ANIM_LIST_START_COL:
            retVal.SetInt(animState->startFrame);
        break;

        case ANIM_LIST_END_COL:
            retVal.SetInt(animState->endFrame);
        break;

        case ANIM_LIST_FADEIN_COL:
            retVal.SetFloat(animState->fadeInTime);
        break;

        case ANIM_LIST_LOOP_COL:
            retVal = animState->repeat ? "Y" : "N";
        break;
    }
    return retVal.Get();
}

//----------------------------------------------------------------------------
// EOF
//----------------------------------------------------------------------------
