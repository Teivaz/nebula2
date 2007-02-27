//-----------------------------------------------------------------------------
//  nmaxenvelopecolorcurveDialog.cpp
//
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "nmaxenvelopecurve.h"
#include "nmaxenvelopecolorcurveDialog.h"
#include ".\nmaxenvelopecolorcurvedialog.h"


// nmaxenvelopecolorcurveDialog 

IMPLEMENT_DYNAMIC(nmaxenvelopecolorcurveDialog, CDialog)

//-----------------------------------------------------------------------------
/**
*/
nmaxenvelopecolorcurveDialog::nmaxenvelopecolorcurveDialog(CWnd* pParent /*=NULL*/)
    : CDialog(nmaxenvelopecolorcurveDialog::IDD, pParent)
{
}

//-----------------------------------------------------------------------------
/**
*/
nmaxenvelopecolorcurveDialog::~nmaxenvelopecolorcurveDialog()
{
}

//-----------------------------------------------------------------------------
/**
*/
void nmaxenvelopecolorcurveDialog::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
}

//-----------------------------------------------------------------------------
/**
*/
float nmaxenvelopecolorcurveDialog::AdjustMinMaxFloat( UINT nID, float fMin, float fMax, float fRevision )
{
    CString strValue;
    GetDlgItem(nID)->GetWindowText(strValue);

    BOOL bValidate = TRUE;
    for( int i = 0; i < strValue.GetLength(); ++i )
    {
        char c = strValue.GetAt(i);
        if( c == '.' )
            continue;

        if( isdigit(c) == 0 )
        {
            bValidate = FALSE;
            break;
        }
    }

    if( !bValidate )
    {
        // previous value
        strValue.Format("%.4f", fRevision );
        GetDlgItem(nID)->SetWindowText(strValue);
        return fRevision;
    }
    else
    {
        // check the range
        float fPos = atof( strValue );

        if( fMin > fPos )
        {
            fPos = fMin;
            strValue.Format("%.4f", fPos );
            GetDlgItem(nID)->SetWindowText(strValue);
        }
        else if( fMax < fPos )
        {
            fPos = fMax;
            strValue.Format("%.4f", fPos );
            GetDlgItem(nID)->SetWindowText(strValue);
        }

        return fPos;
    }
}

BEGIN_MESSAGE_MAP(nmaxenvelopecolorcurveDialog, CDialog)
    ON_MESSAGE(ECCN_CHANGEDVALUE, OnECCNChangedValue )
    ON_WM_LBUTTONDBLCLK()
    ON_WM_DRAWITEM()
    ON_BN_DOUBLECLICKED(IDC_BTN_SELECTED_COLOR, OnBnDoubleclickedBtnSelectedColor)
    ON_WM_PAINT()
    ON_EN_CHANGE(IDC_EDIT_SELECTED_POS, OnEnChangeEditSelectedPos)
END_MESSAGE_MAP()

//-----------------------------------------------------------------------------
/**
*/
BOOL nmaxenvelopecolorcurveDialog::OnInitDialog()
{
    CDialog::OnInitDialog();

    CRect rcClient;
    GetDlgItem(IDC_CONTROL_HOLDER)->GetWindowRect(&rcClient);
    ScreenToClient(&rcClient);

    m_ColorCurveControl.Create(
        NULL, 
        "Control", 
        WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS , 
        rcClient,
        this,
        NULL,
        NULL);

    UpdateDialogControls();

    return TRUE;  // return TRUE unless you set the focus to a control
    // Exception: OCX property page should return FALSE.
}

//-----------------------------------------------------------------------------
/**
*/
void nmaxenvelopecolorcurveDialog::UpdateDialogControls()
{

}

//-----------------------------------------------------------------------------
/**
*/
LRESULT nmaxenvelopecolorcurveDialog::OnECCNChangedValue(WPARAM wParam, LPARAM lParam)
{
    CEnvelopeColorCurve* pControl = GetEnvelopeColorCurve();

    int iSelectedCurve = pControl->GetSelectedCurve();

    if( CEnvelopeColorCurve::CP_A == iSelectedCurve ||
        CEnvelopeColorCurve::CP_D == iSelectedCurve )
    {
        GetDlgItem(IDC_EDIT_SELECTED_POS)->EnableWindow(FALSE);
    }
    else
    {
        GetDlgItem(IDC_EDIT_SELECTED_POS)->EnableWindow(TRUE);
    }

    CString strValue;
    strValue.Format("%.4f", pControl->GetSelectedCurvePos() );
    GetDlgItem(IDC_EDIT_SELECTED_POS)->SetWindowText(strValue);

    GetDlgItem(IDC_BTN_SELECTED_COLOR)->RedrawWindow();

    // dialog -> activex event
    CWnd* pOwner = GetOwner();
    ASSERT( pOwner );
    if( pOwner )
        pOwner->SendMessage(ECCN_CHANGEDVALUE, (WPARAM)this, 0 );

    return S_OK;
}

//-----------------------------------------------------------------------------
/**
*/
void nmaxenvelopecolorcurveDialog::OnLButtonDblClk(UINT nFlags, CPoint point)
{
    
    CDialog::OnLButtonDblClk(nFlags, point);
}

//-----------------------------------------------------------------------------
/**
*/
void nmaxenvelopecolorcurveDialog::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
    if( IDC_BTN_SELECTED_COLOR == nIDCtl )
    {
        CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);

        pDC->FillSolidRect(&lpDrawItemStruct->rcItem, GetEnvelopeColorCurve()->GetSelectedCurveValue() );
        return;
    }
    
    CDialog::OnDrawItem(nIDCtl, lpDrawItemStruct);
}

//-----------------------------------------------------------------------------
/**
*/
void nmaxenvelopecolorcurveDialog::OnBnDoubleclickedBtnSelectedColor()
{
    CEnvelopeColorCurve* pControl = GetEnvelopeColorCurve();

    CColorDialog dlg(pControl->GetSelectedCurveValue(), CC_FULLOPEN);
    if( dlg.DoModal() == IDOK )
    {
        pControl->SetSelectedCurveValue( dlg.GetColor() );
        pControl->Invalidate();

        GetDlgItem(IDC_BTN_SELECTED_COLOR)->Invalidate();

        // dialog -> activex event
        CWnd* pOwner = GetOwner();
        ASSERT( pOwner );
        if( pOwner )
            pOwner->SendMessage(ECCN_CHANGEDVALUE, (WPARAM)this, 0 );
    }
}

//-----------------------------------------------------------------------------
/**
*/
void nmaxenvelopecolorcurveDialog::OnPaint()
{
    CPaintDC dc(this); // device context for painting
}

//-----------------------------------------------------------------------------
/**
*/
void nmaxenvelopecolorcurveDialog::OnEnChangeEditSelectedPos()
{
    int iSelectedCurve = GetEnvelopeColorCurve()->GetSelectedCurve();
    
    if( CEnvelopeColorCurve::CP_A == iSelectedCurve ||
        CEnvelopeColorCurve::CP_D == iSelectedCurve )
        return;

    float fPrePos = GetEnvelopeColorCurve()->GetCurvePos(iSelectedCurve - 1);
    float fNextPos = GetEnvelopeColorCurve()->GetCurvePos(iSelectedCurve + 1);

    float fResult = AdjustMinMaxFloat(IDC_EDIT_SELECTED_POS, fPrePos, fNextPos, GetEnvelopeColorCurve()->GetSelectedCurvePos() );

    if( GetEnvelopeColorCurve()->GetSelectedCurvePos() != fResult )
    {
        GetEnvelopeColorCurve()->SetSelectedCurvePos( fResult );
        GetEnvelopeColorCurve()->UpdateData(FALSE);
        GetEnvelopeColorCurve()->Invalidate();
    }
}
