//-----------------------------------------------------------------------------
//  nmaxenvelopecurvedialog.cpp
//
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "nmaxenvelopecurve.h"
#include "nmaxenvelopecurvedialog.h"
#include ".\nmaxenvelopecurvedialog.h"


IMPLEMENT_DYNAMIC(nmaxenvelopecurveDialog, CDialog)

//-----------------------------------------------------------------------------
/**
*/
nmaxenvelopecurveDialog::nmaxenvelopecurveDialog(CWnd* pParent /*=NULL*/)
    : CDialog(nmaxenvelopecurveDialog::IDD, pParent)
{
}

//-----------------------------------------------------------------------------
/**
*/
nmaxenvelopecurveDialog::~nmaxenvelopecurveDialog()
{
}

//-----------------------------------------------------------------------------
/**
*/
void nmaxenvelopecurveDialog::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(nmaxenvelopecurveDialog, CDialog)
    ON_MESSAGE(ECN_CHANGEDVALUE, OnECNChangedValue )
    ON_EN_CHANGE(IDC_EDIT_SELECTED_POS, OnEnChangeEditSelectedPos)
    ON_EN_CHANGE(IDC_EDIT_SELECTED_VALUE, OnEnChangeEditSelectedValue)
    ON_EN_CHANGE(IDC_EDIT_MAX, OnEnChangeEditMax)
    ON_EN_CHANGE(IDC_EDIT_MIN, OnEnChangeEditMin)
    ON_EN_CHANGE(IDC_EDIT_FREQUENCY, OnEnChangeEditFrequency)
    ON_EN_CHANGE(IDC_EDIT_AMPLITUDE, OnEnChangeEditAmplitude)
    ON_CBN_SELCHANGE(IDC_COMBO_FUNCTION, OnCbnSelchangeComboFunction)
    ON_BN_CLICKED(IDC_BTN_TOGGLE, OnBnClickedBtnToggle)
    ON_WM_PAINT()
END_MESSAGE_MAP()

//-----------------------------------------------------------------------------
/**
*/
BOOL nmaxenvelopecurveDialog::OnInitDialog()
{
    CDialog::OnInitDialog();

    CRect rcClient;
    GetDlgItem(IDC_STATIC_HOLDER)->GetWindowRect(&rcClient);
    ScreenToClient(&rcClient);

    m_CurveControl.Create(
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
LRESULT nmaxenvelopecurveDialog::OnECNChangedValue(WPARAM wParam, LPARAM lParam)
{
    CEnvelopeCurve* pControl = GetEnvelopeCurve();

    int iSelectedCurve = pControl->GetSelectedCurve();

    if( CEnvelopeCurve::CP_A == iSelectedCurve ||
        CEnvelopeCurve::CP_D == iSelectedCurve )
    {
        GetDlgItem(IDC_EDIT_SELECTED_POS)->EnableWindow(FALSE);
    }
    else
    {
        GetDlgItem(IDC_EDIT_SELECTED_POS)->EnableWindow(TRUE);
    }

    CString strValue;
    strValue.Format( "%.4f", pControl->GetSelectedCurvePos() );
    GetDlgItem(IDC_EDIT_SELECTED_POS)->SetWindowText( strValue );
    strValue.Format( "%.4f", pControl->GetSelectedCurveValue() );
    GetDlgItem(IDC_EDIT_SELECTED_VALUE)->SetWindowText( strValue );
    strValue.Format( "%.4f", pControl->GetMax() );
    GetDlgItem(IDC_EDIT_MAX)->SetWindowText( strValue );
    strValue.Format( "%.4f", pControl->GetMin() );
    GetDlgItem(IDC_EDIT_MIN)->SetWindowText( strValue );
    strValue.Format( "%.2f", pControl->GetFrequency() );
    GetDlgItem(IDC_EDIT_FREQUENCY)->SetWindowText( strValue );
    strValue.Format( "%.2f", pControl->GetAmplitude() );
    GetDlgItem(IDC_EDIT_AMPLITUDE)->SetWindowText( strValue );

    ((CComboBox*)GetDlgItem(IDC_COMBO_FUNCTION))->SetCurSel( pControl->GetFunction() );

    // dialog -> activex event
    CWnd* pOwner = GetOwner();
    ASSERT( pOwner );
    if( pOwner )
        pOwner->SendMessage(ECN_CHANGEDVALUE, (WPARAM)this, 0 );

    return TRUE;
}

//-----------------------------------------------------------------------------
/**
    when we use MFC range checking, it calls message box.
    So we use the following member function instead of DDV_MinMaxFloat macro 
    to prevent the annoying thing above mentioned.
*/
float nmaxenvelopecurveDialog::AdjustMinMaxFloat( UINT nID, float fMin, float fMax, float fRevision )
{
    CString strValue;
    GetDlgItem(nID)->GetWindowText(strValue);


    BOOL bValidate = TRUE;
    for( int i = 0; i < strValue.GetLength(); ++i )
    {
        char c = strValue.GetAt(i);
        if( c == '.' ||
            c == '-' )
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
        // check range.
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

//-----------------------------------------------------------------------------
/**
*/
void nmaxenvelopecurveDialog::OnEnChangeEditSelectedPos()
{
    int iSelectedCurve = GetEnvelopeCurve()->GetSelectedCurve();
    
    if( CEnvelopeCurve::CP_A == iSelectedCurve ||
        CEnvelopeCurve::CP_D == iSelectedCurve )
        return;

    float fPrePos = GetEnvelopeCurve()->GetCurvePos(iSelectedCurve - 1);
    float fNextPos = GetEnvelopeCurve()->GetCurvePos(iSelectedCurve + 1);

    float fResult = AdjustMinMaxFloat(IDC_EDIT_SELECTED_POS, fPrePos, fNextPos, GetEnvelopeCurve()->GetSelectedCurvePos() );

    if( GetEnvelopeCurve()->GetSelectedCurvePos() != fResult )
    {
        GetEnvelopeCurve()->SetSelectedCurvePos( fResult );
        GetEnvelopeCurve()->UpdateData(FALSE);
        GetEnvelopeCurve()->Invalidate();
    }
}

//-----------------------------------------------------------------------------
/**
*/
void nmaxenvelopecurveDialog::OnEnChangeEditSelectedValue()
{
    float fResult = AdjustMinMaxFloat(IDC_EDIT_SELECTED_VALUE, 0.0f, 1.0f, GetEnvelopeCurve()->GetSelectedCurveValue() );

    if( GetEnvelopeCurve()->GetSelectedCurveValue() != fResult )
    {
        GetEnvelopeCurve()->SetSelectedCurveValue( fResult );
        GetEnvelopeCurve()->UpdateData(FALSE);
        GetEnvelopeCurve()->Invalidate();
    }
}

//-----------------------------------------------------------------------------
/**
*/
void nmaxenvelopecurveDialog::OnEnChangeEditMax()
{
    float fResult = AdjustMinMaxFloat(IDC_EDIT_MAX, GetEnvelopeCurve()->GetMin(), 1000.0f, GetEnvelopeCurve()->GetMax() );

    if( GetEnvelopeCurve()->GetMax() != fResult )
    {
        GetEnvelopeCurve()->SetMax( fResult );
        GetEnvelopeCurve()->UpdateData(FALSE);
    }
}

//-----------------------------------------------------------------------------
/**
*/
void nmaxenvelopecurveDialog::OnEnChangeEditMin()
{
    float fResult = AdjustMinMaxFloat(IDC_EDIT_MIN, -1000.0f, GetEnvelopeCurve()->GetMax(), GetEnvelopeCurve()->GetMin() );

    if( GetEnvelopeCurve()->GetMin() != fResult )
    {
        GetEnvelopeCurve()->SetMin( fResult );
        GetEnvelopeCurve()->UpdateData(FALSE);
    }
    
}

//-----------------------------------------------------------------------------
/**
*/
void nmaxenvelopecurveDialog::OnEnChangeEditFrequency()
{
    float fResult = AdjustMinMaxFloat(IDC_EDIT_FREQUENCY, 0.0f, 1000.0f, GetEnvelopeCurve()->GetFrequency() );    
    
    if( GetEnvelopeCurve()->GetFrequency() != fResult )
    {
        GetEnvelopeCurve()->SetFrequency( fResult );
        GetEnvelopeCurve()->UpdateData(FALSE);
    }
}

//-----------------------------------------------------------------------------
/**
*/
void nmaxenvelopecurveDialog::OnEnChangeEditAmplitude()
{
    float fResult = AdjustMinMaxFloat(IDC_EDIT_AMPLITUDE, 0.0f, 1000.0f, GetEnvelopeCurve()->GetAmplitude() );

    if( GetEnvelopeCurve()->GetAmplitude() != fResult )
    {
        GetEnvelopeCurve()->SetAmplitude( fResult );
        GetEnvelopeCurve()->UpdateData(FALSE);
    }
}

//-----------------------------------------------------------------------------
/**
*/
void nmaxenvelopecurveDialog::OnCbnSelchangeComboFunction()
{
    int iCurSel = ((CComboBox*)GetDlgItem(IDC_COMBO_FUNCTION))->GetCurSel();

    if( GetEnvelopeCurve()->GetFunction() != iCurSel )
    {
        GetEnvelopeCurve()->SetFunction(iCurSel);
        GetEnvelopeCurve()->UpdateData(FALSE);
    }
}

//-----------------------------------------------------------------------------
/**
*/
void nmaxenvelopecurveDialog::UpdateDialogControls()
{
    CEnvelopeCurve* pControl = GetEnvelopeCurve();
    pControl->UpdateData( FALSE );

    // activex -> dialog -> control -> ( changedvalue ) dialog -> ( changedvalue event ) activex 
}

//-----------------------------------------------------------------------------
/**
*/
void nmaxenvelopecurveDialog::OnBnClickedBtnToggle()
{
    /*
    CRect rcCleint;
    GetWindowRect(&rcCleint);

    CRect rcCleint2;
    SetRect(&rcCleint2, rcCleint.left, rcCleint.top, rcCleint.right, rcCleint.top + 50);

    //ClientToScreen(&rcCleint2);
    ScreenToClient(&rcCleint2);

    MoveWindow(&rcCleint2);
    GetOwner()->MoveWindow(&rcCleint2);
    */
}

//-----------------------------------------------------------------------------
/**
*/
void nmaxenvelopecurveDialog::OnPaint()
{
    CPaintDC dc(this); // device context for painting

    CRect rc;
    GetDlgItem(IDC_STATIC_NAME)->GetWindowRect(&rc);
    ScreenToClient(&rc);

    CString strName;
    GetWindowText(strName);
    dc.SelectStockObject(SYSTEM_FONT);
    dc.SetBkMode(TRANSPARENT);
    dc.DrawText( strName, &rc, DT_SINGLELINE );
    //dc.DrawText( "Particle Spread Max", &rc, DT_SINGLELINE );
}

//-----------------------------------------------------------------------------
/**
*/
void nmaxenvelopecurveDialog::OnOK()
{
    //CDialog::OnOK();
}

//-----------------------------------------------------------------------------
/**
*/
void nmaxenvelopecurveDialog::OnCancel()
{
    //CDialog::OnCancel();
}
