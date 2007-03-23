//-----------------------------------------------------------------------------
//  EnvelopeCurve.cpp
//
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "EnvelopeCurve.h"
#include ".\envelopecurve.h"

#define POINT_SIZE 4

// CEnvelopeCurve

IMPLEMENT_DYNAMIC(CEnvelopeCurve, CWnd)

//-----------------------------------------------------------------------------
/**
*/
CEnvelopeCurve::CEnvelopeCurve()
{
}

//-----------------------------------------------------------------------------
/**
*/
CEnvelopeCurve::~CEnvelopeCurve()
{
}

BEGIN_MESSAGE_MAP(CEnvelopeCurve, CWnd)
    ON_WM_PAINT()
    ON_WM_LBUTTONDOWN()
    ON_WM_MOUSEMOVE()
    ON_WM_CREATE()
    ON_WM_ERASEBKGND()
    ON_WM_SIZE()
    ON_WM_RBUTTONDOWN()
    ON_COMMAND(EC_FLIP_CURVE_VERTICAL, OnFlipCurveVertical)
    ON_COMMAND(EC_FLIP_CURVE_HORIZONTAL, OnFlipCurveHorizontal)
    ON_COMMAND(EC_STRAIGHTEN_CURVE, OnStraightenCurve)
    ON_COMMAND(EC_COPY_CURVE, OnCopyCurve)
    ON_COMMAND(EC_PASTE_CURVE, OnPasteCurve)
    ON_COMMAND(EC_ALL_KEYS_TO_MAX, OnAllKeysToMax)
    ON_COMMAND(EC_ALL_KEYS_TO_MID, OnAllKeysToMid)
    ON_COMMAND(EC_ALL_KEYS_TO_MIN, OnAllKeysToMin)
END_MESSAGE_MAP()

//-----------------------------------------------------------------------------
/**
*/
void CEnvelopeCurve::OnPaint()
{
    CPaintDC dc(this); // device context for painting

    CBrush* pOldBrush = dc.GetCurrentBrush();
    CPen* pOldPen = dc.GetCurrentPen();

    CRect rcClient;
    GetClientRect(&rcClient);

    // background
    dc.SelectStockObject( LTGRAY_BRUSH );
    dc.FillRect(&rcClient, NULL );

    // panel
    CBrush brushPanel;
    brushPanel.CreateSolidBrush( RGB(150,150,150));
    dc.FillRect( &m_rcPanel, &brushPanel );

    // polygon
    CBrush brushPolygon;
    brushPolygon.CreateSolidBrush( RGB(110,110,110));
    dc.SelectObject( &brushPolygon );
    dc.SelectStockObject( NULL_PEN );
    dc.Polygon( m_ptPolygon, 6 );

    // frame
    dc.SelectStockObject( BLACK_BRUSH );
    dc.FrameRect( &m_rcPanel, NULL );

    // curve line
    dc.SelectStockObject( BLACK_PEN );
    dc.MoveTo( m_ptCurve[CP_A] );
    dc.LineTo( m_ptCurve[CP_B] );
    dc.LineTo( m_ptCurve[CP_C] );
    dc.LineTo( m_ptCurve[CP_D] );

    // curve point
    for( int i = 0 ; i < CP_MAX; ++i )
    {
        if( m_iSelectedCurve == i)
        {
            dc.SelectStockObject( WHITE_PEN );    
            dc.SelectStockObject( BLACK_BRUSH );            
        }
        else
        {
            dc.SelectStockObject( WHITE_PEN );
            dc.SelectStockObject( HOLLOW_BRUSH );
        }
        
        dc.Ellipse( m_rcCurve[i] );
    }

    dc.SelectObject( pOldBrush );
    dc.SelectObject( pOldPen );
    
}

//-----------------------------------------------------------------------------
/**
*/
void CEnvelopeCurve::SetCurvePos( int index, float pos )
{
    m_fPos[ index ] = pos;
}

//-----------------------------------------------------------------------------
/**
*/
void CEnvelopeCurve::SetCurveValue( int index, float value )
{
    m_fValue[ index ] = value;
}

//-----------------------------------------------------------------------------
/**
*/
void CEnvelopeCurve::SetSelectedCurvePos( float pos )
{
    m_fPos[ m_iSelectedCurve ] = pos;
}

//-----------------------------------------------------------------------------
/**
*/
void CEnvelopeCurve::SetSelectedCurveValue( float value )
{
    m_fValue[ m_iSelectedCurve ] = value;
}

//-----------------------------------------------------------------------------
/**
*/
void CEnvelopeCurve::SetMin( float fMin )
{
    m_fMin = fMin;
}

//-----------------------------------------------------------------------------
/**
*/
void CEnvelopeCurve::SetMax( float fMax )
{
    m_fMax = fMax;
}

//-----------------------------------------------------------------------------
/**
*/
void CEnvelopeCurve::SetFrequency( float fFrequency )
{
    m_fFrequency = fFrequency;  
}

//-----------------------------------------------------------------------------
/**
*/
void CEnvelopeCurve::SetAmplitude( float fAmplitude )
{
    m_fAmplitude = fAmplitude;
}

//-----------------------------------------------------------------------------
/**
*/
void CEnvelopeCurve::SetFunction( int iFunc )
{
    m_iModulationFunc = iFunc;
}

//-----------------------------------------------------------------------------
/**
*/
float CEnvelopeCurve::GetCurvePos( int index )
{
    return m_fPos[ index ];
}

//-----------------------------------------------------------------------------
/**
*/
float CEnvelopeCurve::GetCurveValue( int index )
{
    return m_fValue[ index ];
}

//-----------------------------------------------------------------------------
/**
*/
float CEnvelopeCurve::GetSelectedCurvePos()
{
    return m_fPos[ m_iSelectedCurve ];
}

//-----------------------------------------------------------------------------
/**
*/
float CEnvelopeCurve::GetSelectedCurveValue()
{
    return m_fValue[ m_iSelectedCurve ];
}

//-----------------------------------------------------------------------------
/**
*/
float CEnvelopeCurve::GetMin()
{
    return m_fMin;
}

//-----------------------------------------------------------------------------
/**
*/
float CEnvelopeCurve::GetMax()
{
    return m_fMax;
}

//-----------------------------------------------------------------------------
/**
*/
float CEnvelopeCurve::GetFrequency()
{
    return m_fFrequency;
}

//-----------------------------------------------------------------------------
/**
*/
float CEnvelopeCurve::GetAmplitude()
{
    return m_fAmplitude;
}

//-----------------------------------------------------------------------------
/**
*/
int CEnvelopeCurve::GetFunction()
{
    return m_iModulationFunc;
}

//-----------------------------------------------------------------------------
/**
*/
int CEnvelopeCurve::GetSelectedCurve()
{
    return m_iSelectedCurve;
}

//-----------------------------------------------------------------------------
/**
*/
int CEnvelopeCurve::SelectedCurve( CPoint& pt )
{
    for( int i = 0; i < CP_MAX; ++i )
    {
        if( m_rcCurve[i].PtInRect(pt) )
        {
            return i;
        }
    }

    return -1;
}

//-----------------------------------------------------------------------------
/**
*/
void CEnvelopeCurve::FlipCruveVertical()
{
    for( int i = 0; i < CP_MAX; ++i )
    {
        m_fValue[i] = 1.0f - m_fValue[i];
    }

    // data -> control
}

//-----------------------------------------------------------------------------
/**
*/
void CEnvelopeCurve::FlipCurveHorizontal()
{
    float temp = m_fValue[CP_B];
    m_fValue[CP_B] = m_fValue[CP_C];
    m_fValue[CP_C] = temp;

    temp = m_fValue[CP_A];
    m_fValue[CP_A] = m_fValue[CP_D];
    m_fValue[CP_D] = temp;

    // data -> control
}

//-----------------------------------------------------------------------------
/**
*/
void CEnvelopeCurve::StraightenCurve()
{
    for( int i = 0; i < CP_MAX; ++i )
    {
        m_fValue[i] = GetSelectedCurveValue();
    }
}

//-----------------------------------------------------------------------------
/**
*/
void CEnvelopeCurve::AllKeysToMaximum()
{
    for( int i = 0; i < CP_MAX; ++i )
    {
        m_fValue[i] = 1.0f;
    }
}

//-----------------------------------------------------------------------------
/**
*/
void CEnvelopeCurve::AllKeysToMiddle()
{
    for( int i = 0; i < CP_MAX; ++i )
    {
        m_fValue[i] = 0.5f;
    }
}

//-----------------------------------------------------------------------------
/**
*/
void CEnvelopeCurve::AllKeysToMinimum()
{
    for( int i = 0; i < CP_MAX; ++i )
    {
        m_fValue[i] = 0.0f;
    }
}

//-----------------------------------------------------------------------------
/**
    This member function send message when the control is changed and the changes 
    should be applied to its associated data.

    When the control is changed by the external control's values of the dialog, 
    it does not send message.
*/
void CEnvelopeCurve::UpdateData( BOOL bUpdateData )
{
    // control -> data
    if( bUpdateData )
    {
        int iWidth = m_rcPanel.Width();
        int iHeight = m_rcPanel.Height();
        // by point 
        for( int i = 0; i < CP_MAX; ++i )
        {
            m_ptCurve[i].x = static_cast<int>(m_fCurve[i].x);
            m_ptCurve[i].y = static_cast<int>(m_fCurve[i].y);
            m_rcCurve[i] = CRect( m_ptCurve[i].x - POINT_SIZE, m_ptCurve[i].y - POINT_SIZE, m_ptCurve[i].x + POINT_SIZE, m_ptCurve[i].y + POINT_SIZE );

            m_fPos[i] = (m_fCurve[i].x - m_rcPanel.left ) / float(iWidth);
            m_fValue[i] = (m_rcPanel.bottom - m_fCurve[i].y ) / float(iHeight);
        }

        //CWnd* pOwner = GetOwner();
        //ASSERT( pOwner );

        //if( pOwner )
        //    pOwner->SendMessage(ECN_CHANGEDVALUE, (WPARAM)this, 0 );
    }
    // data -> control
    else
    {
        CRect rt;
        GetClientRect( &rt );

        m_rcPanel = rt;
        m_rcPanel.DeflateRect( 5, 5, 5, 5 );

        int iWidth = m_rcPanel.Width();
        int iHeight = m_rcPanel.Height();

        for( int i = 0; i < CP_MAX; ++i )
        {
            m_fCurve[i].x = m_rcPanel.left + iWidth * m_fPos[i];
            m_fCurve[i].y = m_rcPanel.bottom - iHeight * m_fValue[i];

            m_ptCurve[i].x = static_cast<int>(m_fCurve[i].x);
            m_ptCurve[i].y = static_cast<int>(m_fCurve[i].y);

            m_rcCurve[i] = CRect( m_ptCurve[i].x - POINT_SIZE, m_ptCurve[i].y - POINT_SIZE, m_ptCurve[i].x + POINT_SIZE, m_ptCurve[i].y + POINT_SIZE );
        }
    }

    for( int i = 0; i < CP_MAX; ++i )
        m_ptPolygon[i] = m_ptCurve[i];

    m_ptPolygon[4] = m_rcPanel.BottomRight();
    m_ptPolygon[5] = CPoint( m_rcPanel.left, m_rcPanel.bottom );

    CWnd* pOwner = GetOwner();
    ASSERT( pOwner );

    if( pOwner )
        pOwner->SendMessage(ECN_CHANGEDVALUE, (WPARAM)this, 0 );

}

//-----------------------------------------------------------------------------
/**
*/
void CEnvelopeCurve::OnLButtonDown(UINT nFlags, CPoint point)
{
    int iSelectedCurve = SelectedCurve( point );
    if( -1 == iSelectedCurve )
    {
        m_bSelectedCurve = FALSE;

    }
    else
    {
        m_bSelectedCurve = TRUE;
        m_iSelectedCurve = iSelectedCurve;
        InvalidateRect( NULL );

        CWnd* pOwner = GetOwner();
        ASSERT( pOwner );

        if( pOwner )
            pOwner->SendMessage(ECN_CHANGEDVALUE, (WPARAM)this, 0 );
    }
    
    CWnd::OnLButtonDown(nFlags, point);
}

//-----------------------------------------------------------------------------
/**
*/
void CEnvelopeCurve::OnMouseMove(UINT nFlags, CPoint point)
{
    if( (MK_LBUTTON & nFlags) &&
        //(MK_CONTROL & nFlags) &&
        m_bSelectedCurve )
    {
        int iPrevious = m_iSelectedCurve - 1;
        int iNext = m_iSelectedCurve + 1;

        //HACK: we assume it is currently selected control point.
        CPoint pt = point;
        //m_ptCurve[ m_iSelectedCurve ] = point;

        // check x-pos
        // left
        if( -1 < iPrevious )
        {
            if( pt.x < m_ptCurve[ iPrevious ].x )
                pt.x = m_ptCurve[ iPrevious ].x;
        }
        else
        {
            pt.x = m_rcPanel.left;
        }

        // right
        if( iNext < CP_MAX )
        {
            if( pt.x > m_ptCurve[ iNext ].x )
                pt.x = m_ptCurve[ iNext ].x;
        }
        else
        {
            pt.x = m_rcPanel.right;
        }

        // check y-pos
        if( pt.y < m_rcPanel.top )
            pt.y = m_rcPanel.top;

        if( pt.y > m_rcPanel.bottom )
            pt.y = m_rcPanel.bottom;

        m_fCurve[ m_iSelectedCurve ].x = static_cast<float>(pt.x);
        m_fCurve[ m_iSelectedCurve ].y = static_cast<float>(pt.y);

        UpdateData( TRUE );
        InvalidateRect( NULL );
    }

    CWnd::OnMouseMove(nFlags, point);
}

//-----------------------------------------------------------------------------
/**
*/
int CEnvelopeCurve::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CWnd::OnCreate(lpCreateStruct) == -1)
        return -1;

    m_fPos[CP_A] = 0.0f;
    m_fPos[CP_B] = 0.3f;
    m_fPos[CP_C] = 0.7f;
    m_fPos[CP_D] = 1.0f;

    m_fValue[CP_A] = 0.0f;
    m_fValue[CP_B] = 0.2f;
    m_fValue[CP_C] = 0.8f;
    m_fValue[CP_D] = 1.0f;

    m_fMin = 0.0f;
    m_fMax = 1.0f;
    m_fFrequency = 0.0f;
    m_fAmplitude = 0.0f;
    m_iModulationFunc = 0;

    m_iSelectedCurve = 0;

    UpdateData( FALSE );

    return 0;
}

//-----------------------------------------------------------------------------
/**
*/
void CEnvelopeCurve::OnRButtonDown(UINT nFlags, CPoint point)
{
    CMenu menu;

    menu.CreatePopupMenu();

    menu.AppendMenu(MF_STRING, EC_FLIP_CURVE_VERTICAL, "Flip Curve Vertical");
    menu.AppendMenu(MF_STRING, EC_FLIP_CURVE_HORIZONTAL, "Flip Curve Horizeontal");
    menu.AppendMenu(MF_STRING, EC_STRAIGHTEN_CURVE, "Straighten Curve");
    menu.AppendMenu(MF_STRING, EC_COPY_CURVE, "Copy Curve");
    menu.AppendMenu(MF_STRING, EC_PASTE_CURVE, "Paste Curve");
    menu.AppendMenu(MF_SEPARATOR);
    menu.AppendMenu(MF_STRING, EC_ALL_KEYS_TO_MAX, "All Keys To Maximum");
    menu.AppendMenu(MF_STRING, EC_ALL_KEYS_TO_MID, "All Keys To Middle");
    menu.AppendMenu(MF_STRING, EC_ALL_KEYS_TO_MIN, "All Keys To Minimum");

    CPoint pt = point;
    ClientToScreen(&pt);
    menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, this);
    menu.DestroyMenu();

    CWnd::OnRButtonDown(nFlags, point);
}

//-----------------------------------------------------------------------------
/**
*/
void CEnvelopeCurve::OnFlipCurveVertical()
{
    FlipCruveVertical();
    UpdateData(FALSE);
    InvalidateRect( NULL );

    CWnd* pOwner = GetOwner();
    ASSERT( pOwner );

    if( pOwner )
        pOwner->SendMessage(ECN_CHANGEDVALUE, (WPARAM)this, 0 );
}

//-----------------------------------------------------------------------------
/**
*/
void CEnvelopeCurve::OnFlipCurveHorizontal()
{
    FlipCurveHorizontal();
    UpdateData(FALSE);
    InvalidateRect( NULL );

    CWnd* pOwner = GetOwner();
    ASSERT( pOwner );

    if( pOwner )
        pOwner->SendMessage(ECN_CHANGEDVALUE, (WPARAM)this, 0 );
}

//-----------------------------------------------------------------------------
/**
*/
void CEnvelopeCurve::OnStraightenCurve()
{
    StraightenCurve();
    UpdateData(FALSE);
    InvalidateRect( NULL );

    CWnd* pOwner = GetOwner();
    ASSERT( pOwner );

    if( pOwner )
        pOwner->SendMessage(ECN_CHANGEDVALUE, (WPARAM)this, 0 );
}

//-----------------------------------------------------------------------------
/**
*/
void CEnvelopeCurve::OnCopyCurve()
{
    OpenClipboard();
    EmptyClipboard();

    CString strData;
    strData.Format( "CurveData %f %f %f %f %f %f",
        GetCurvePos(CP_B), GetCurvePos(CP_C),
        GetCurveValue(CP_A), GetCurveValue(CP_B), GetCurveValue(CP_C), GetCurveValue(CP_D)
        );

    HANDLE hClipboard = GlobalAlloc (GPTR, strData.GetLength () + 1);
    LPTSTR lpszBuffer = (LPTSTR)GlobalLock (hClipboard);
    _tcscpy (lpszBuffer, strData);
    GlobalUnlock (hClipboard);

    SetClipboardData(CF_TEXT, hClipboard);
    CloseClipboard();
}

//-----------------------------------------------------------------------------
/**
*/
void CEnvelopeCurve::OnPasteCurve()
{
    OpenClipboard();
    
    HANDLE hClipboard = GetClipboardData(CF_TEXT);
    if( NULL == hClipboard )
    {
        CloseClipboard();
        return;
    }

    CString strData;
    LPTSTR lpszBuffer = (LPTSTR)GlobalLock (hClipboard);
    strData.SetString( lpszBuffer );
    GlobalUnlock (hClipboard);

    int iPos = 0;
    CString strBuffer;
    strBuffer = strData.Tokenize(" ", iPos );

    if( "CurveData" != strBuffer )
    {
        CloseClipboard();
        return;
    }

    float fValue;
    strBuffer = strData.Tokenize(" ",iPos);
    fValue = atof( strBuffer );
    SetCurvePos( CP_B, fValue );

    strBuffer = strData.Tokenize(" ",iPos);
    fValue = atof( strBuffer );
    SetCurvePos( CP_C, fValue );

    strBuffer = strData.Tokenize(" ",iPos);
    fValue = atof( strBuffer );
    SetCurveValue( CP_A, fValue );

    strBuffer = strData.Tokenize(" ",iPos);
    fValue = atof( strBuffer );
    SetCurveValue( CP_B, fValue );

    strBuffer = strData.Tokenize(" ",iPos);
    fValue = atof( strBuffer );
    SetCurveValue( CP_C, fValue );

    strBuffer = strData.Tokenize(" ",iPos);
    fValue = atof( strBuffer );
    SetCurveValue( CP_D, fValue );

    UpdateData(FALSE);
    InvalidateRect( NULL );

    CWnd* pOwner = GetOwner();
    ASSERT( pOwner );

    if( pOwner )
        pOwner->SendMessage(ECN_CHANGEDVALUE, (WPARAM)this, 0 );
}

//-----------------------------------------------------------------------------
/**
*/
void CEnvelopeCurve::OnAllKeysToMax()
{
    AllKeysToMaximum();
    UpdateData(FALSE);
    InvalidateRect( NULL );

    CWnd* pOwner = GetOwner();
    ASSERT( pOwner );

    if( pOwner )
        pOwner->SendMessage(ECN_CHANGEDVALUE, (WPARAM)this, 0 );
}

//-----------------------------------------------------------------------------
/**
*/
void CEnvelopeCurve::OnAllKeysToMid()
{
    AllKeysToMiddle();
    UpdateData(FALSE);
    InvalidateRect( NULL );

    CWnd* pOwner = GetOwner();
    ASSERT( pOwner );

    if( pOwner )
        pOwner->SendMessage(ECN_CHANGEDVALUE, (WPARAM)this, 0 );
}

//-----------------------------------------------------------------------------
/**
*/
void CEnvelopeCurve::OnAllKeysToMin()
{
    AllKeysToMinimum();
    UpdateData(FALSE);
    InvalidateRect( NULL );

    CWnd* pOwner = GetOwner();
    ASSERT( pOwner );

    if( pOwner )
        pOwner->SendMessage(ECN_CHANGEDVALUE, (WPARAM)this, 0 );
}
