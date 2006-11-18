//-----------------------------------------------------------------------------
//  EnvelopeColorCurve.cpp
//
//-----------------------------------------------------------------------------

#include "stdafx.h"
//#include "testdialog.h"
#include "EnvelopeColorCurve.h"
#include ".\envelopecolorcurve.h"


// CEnvelopeColorCurve

IMPLEMENT_DYNAMIC(CEnvelopeColorCurve, CWnd)

//-----------------------------------------------------------------------------
/**
*/
CEnvelopeColorCurve::CEnvelopeColorCurve()
{
    m_pMemDC = NULL;
    m_pBitmap = NULL;
}

//-----------------------------------------------------------------------------
/**
*/
CEnvelopeColorCurve::~CEnvelopeColorCurve()
{
}

#define ECC_FLIP_RAMP                (WM_USER + 3001)
#define ECC_ALL_KEYS_LIKE_THE_FIRST    (WM_USER + 3002)
#define ECC_MAKE_GRADIENT            (WM_USER + 3003)
#define ECC_BRIGHTER                (WM_USER + 3004)
#define ECC_DARKER                    (WM_USER + 3005)
#define ECC_COPY_COLOR_RAMP            (WM_USER + 3006)
#define ECC_PASTE_COLOR_RAMP        (WM_USER + 3007)


BEGIN_MESSAGE_MAP(CEnvelopeColorCurve, CWnd)
    ON_WM_CREATE()
    ON_WM_LBUTTONDOWN()
    ON_WM_MOUSEMOVE()
    ON_WM_LBUTTONDBLCLK()
    ON_WM_DESTROY()
    ON_WM_SIZE()
    ON_WM_PAINT()
    ON_WM_RBUTTONDOWN()
    ON_COMMAND(ECC_FLIP_RAMP, OnFlipRamp)
    ON_COMMAND(ECC_ALL_KEYS_LIKE_THE_FIRST, OnAllKeysLikeTheFirst)
    ON_COMMAND(ECC_MAKE_GRADIENT, OnMakeGradient)
    ON_COMMAND(ECC_BRIGHTER, OnBrighter)
    ON_COMMAND(ECC_DARKER, OnDarker)
    ON_COMMAND(ECC_COPY_COLOR_RAMP, OnCopyColorRamp)
    ON_COMMAND(ECC_PASTE_COLOR_RAMP, OnPasteColorRamp)
END_MESSAGE_MAP()

//-----------------------------------------------------------------------------
/**
*/
void CEnvelopeColorCurve::CreateCompatibleBitmap( int width, int height )
{
    DestroyCompatibleBitmap();

    m_pMemDC = new CDC;
    m_pBitmap = new CBitmap();
    m_pBitmap->CreateCompatibleBitmap( GetDC(), width, height);

    m_pMemDC->CreateCompatibleDC( GetDC() );
    m_pMemDC->SelectObject( m_pBitmap );
}

//-----------------------------------------------------------------------------
/**
*/
void CEnvelopeColorCurve::DestroyCompatibleBitmap()
{
    if( m_pBitmap )
    {
        m_pBitmap->DeleteObject();
        delete m_pBitmap;
        m_pBitmap = NULL;
    }

    if( m_pMemDC )
    {
        m_pMemDC->DeleteDC();
        delete m_pMemDC;
        m_pMemDC = NULL;
    }
}

//-----------------------------------------------------------------------------
/**
*/
void CEnvelopeColorCurve::SetCurvePos( int index, float pos )
{
    ASSERT( index != 0 && index != 3 );
    m_fPos[ index ] = pos;
}

//-----------------------------------------------------------------------------
/**
*/
void CEnvelopeColorCurve::SetCurveValue( int index, COLORREF clr )
{
    m_clrCurve[ index ] = clr;
}

//-----------------------------------------------------------------------------
/**
*/
void CEnvelopeColorCurve::SetCurveValue( int index, float r, float g, float b )
{
    m_clrCurve[ index ] = RGB( r*255, g*255, b*255);
}

//-----------------------------------------------------------------------------
/**
*/
void CEnvelopeColorCurve::SetSelectedCurvePos( float pos )
{
    SetCurvePos( m_iSelectedCurve, pos );
}

//-----------------------------------------------------------------------------
/**
*/
void CEnvelopeColorCurve::SetSelectedCurveValue( COLORREF clr )
{
    SetCurveValue(m_iSelectedCurve, clr );
}

//-----------------------------------------------------------------------------
/**
*/
void CEnvelopeColorCurve::SetSelectedCurveValue( float r, float g, float b )
{
    SetCurveValue( m_iSelectedCurve, r, g, b);
}

//-----------------------------------------------------------------------------
/**
*/
float CEnvelopeColorCurve::GetCurvePos( int index )
{
    return m_fPos[index];
}

//-----------------------------------------------------------------------------
/**
*/
COLORREF CEnvelopeColorCurve::GetCurveValue( int index )
{
    return m_clrCurve[index];
}

//-----------------------------------------------------------------------------
/**
*/
float CEnvelopeColorCurve::GetSelectedCurvePos()
{
    return m_fPos[m_iSelectedCurve];
}

//-----------------------------------------------------------------------------
/**
*/
COLORREF CEnvelopeColorCurve::GetSelectedCurveValue()
{
    return m_clrCurve[m_iSelectedCurve];
}

//-----------------------------------------------------------------------------
/**
*/
void CEnvelopeColorCurve::FlipRamp()
{
    COLORREF temp = m_clrCurve[CP_A];
    m_clrCurve[CP_A] = m_clrCurve[CP_D];
    m_clrCurve[CP_D] = temp;

    temp = m_clrCurve[CP_B];
    m_clrCurve[CP_B] = m_clrCurve[CP_C];
    m_clrCurve[CP_C] = temp;
}

//-----------------------------------------------------------------------------
/**
*/
void CEnvelopeColorCurve::AllKeysLikeTheFirst()
{
    m_clrCurve[CP_D] = m_clrCurve[CP_C] = m_clrCurve[CP_B] = m_clrCurve[CP_A];
}

//-----------------------------------------------------------------------------
/**
*/
void CEnvelopeColorCurve::MakeGadient( COLORREF a, COLORREF b)
{
    m_clrCurve[CP_A] = a;
    m_clrCurve[CP_D] = b;

    COLORREF temp;
    ColorLerp(&temp, &a, &b, m_fPos[CP_B] );
    m_clrCurve[CP_B] = temp;

    ColorLerp(&temp, &a, &b, m_fPos[CP_B] );
    m_clrCurve[CP_C] = temp;

    // TODO : control -> data
}

//-----------------------------------------------------------------------------
/**
*/
void CEnvelopeColorCurve::Brighter()
{

}

//-----------------------------------------------------------------------------
/**
*/
void CEnvelopeColorCurve::Darker()
{
}

//-----------------------------------------------------------------------------
/**
*/
void CEnvelopeColorCurve::UpdateData( BOOL b )
{
    // control -> data
    if( b )
    {
        CRect rcClient;
        GetClientRect( &rcClient );

        m_rcPanel = rcClient;
        m_rcPanel.DeflateRect( 5, 10, 5, 0 );

        int iWidth = m_rcPanel.Width();

        for( int i = 0; i < CP_MAX; ++i )
        {
            m_ptCurve[i].x = m_fCurvePos[i];
            m_rcCurve[i] = CRect( m_ptCurve[i].x - 4, m_ptCurve[i].y - 4, m_ptCurve[i].x + 4, m_ptCurve[i].y + 4 );

            m_fPos[i] = (m_fCurvePos[i] - m_rcPanel.left ) / float(iWidth);
        }

        CWnd* pOwner = GetOwner();
        ASSERT( pOwner );

        if( pOwner )
            pOwner->SendMessage(ECCN_CHANGEDVALUE, (WPARAM)this, 0 );
    }
    // data -> control
    else
    {
        CRect rcClient;
        GetClientRect( &rcClient );

        m_rcPanel = rcClient;
        m_rcPanel.DeflateRect( 5, 10, 5, 0 );

        int iWidth = m_rcPanel.Width();

        for( int i = 0; i < CP_MAX; ++i )
        {
            m_fCurvePos[i] = m_rcPanel.left + iWidth * m_fPos[i];

            m_ptCurve[i].x = m_fCurvePos[i];
            m_ptCurve[i].y = 5;

            m_rcCurve[i] = CRect( m_ptCurve[i].x - 4, m_ptCurve[i].y - 4, m_ptCurve[i].x + 4, m_ptCurve[i].y + 4 );
        }
    }
}

//-----------------------------------------------------------------------------
/**
*/
int CEnvelopeColorCurve::SelectedCurve( CPoint& pt )
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
void CEnvelopeColorCurve::ColorLerp( COLORREF* pOut, const COLORREF* pSrc, const COLORREF* pDest, float s )
{
    byte srcR = GetRValue(*pSrc);
    byte srcG = GetGValue(*pSrc);
    byte srcB = GetBValue(*pSrc);

    byte dstR = GetRValue(*pDest);
    byte dstG = GetGValue(*pDest);
    byte dstB = GetBValue(*pDest);

    int r = srcR + s * (dstR - srcR);
    int g = srcG + s * (dstG - srcG);
    int  b = srcB + s * (dstB - srcB);

    *pOut = RGB(r,g,b);
}

//-----------------------------------------------------------------------------
/**
*/
COLORREF CEnvelopeColorCurve::ColorBrightness(COLORREF clr, float s )
{
    int r = GetRValue(clr);
    int g = GetGValue(clr);
    int b = GetBValue(clr);

    r = r + r * s;
    g = g + g * s;
    b = b + b * s;

    if( 255 < r )
        r = 255;
    else if( 0 > r )
        r = 0;

    if( 255 < g )
        g = 255;
    else if( 0 > g )
        g = 0;

    if( 255 < b )
        b = 255;
    else if( 0 > b )
        b = 0;


    return RGB(r,g,b);
}

//-----------------------------------------------------------------------------
/**
*/
int CEnvelopeColorCurve::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CWnd::OnCreate(lpCreateStruct) == -1)
        return -1;

    m_fPos[CP_A] = 0.0f;
    m_fPos[CP_B] = 0.2f;
    m_fPos[CP_C] = 0.8f;
    m_fPos[CP_D] = 1.0f;

    m_clrCurve[CP_A] = RGB(255, 255, 255);
    m_clrCurve[CP_B] = RGB(255, 0, 0);
    m_clrCurve[CP_C] = RGB(0, 255, 0);
    m_clrCurve[CP_D] = RGB(0, 0, 255);

    m_iSelectedCurve = 0;

    UpdateData( FALSE );

    CreateCompatibleBitmap( m_rcPanel.Width(), m_rcPanel.Height() );

    return 0;
}

//-----------------------------------------------------------------------------
/**
*/
void CEnvelopeColorCurve::OnLButtonDblClk(UINT nFlags, CPoint point)
{
    int iSelectedCurve = SelectedCurve( point );
    if( -1 < iSelectedCurve)
    {
        CColorDialog dlg(m_clrCurve[iSelectedCurve], CC_FULLOPEN);
        if( dlg.DoModal() == IDOK )
        {
            m_clrCurve[iSelectedCurve ] = dlg.GetColor();

            InvalidateRect( NULL );

            CWnd* pOwner = GetOwner();
            ASSERT( pOwner );

            if( pOwner )
                pOwner->SendMessage(ECCN_CHANGEDVALUE, (WPARAM)this, 0 );
        }
    }

    CWnd::OnLButtonDblClk(nFlags, point);
}

//-----------------------------------------------------------------------------
/**
*/
void CEnvelopeColorCurve::OnLButtonDown(UINT nFlags, CPoint point)
{
    int iSelectedCurve = SelectedCurve( point );
    if( -1 == iSelectedCurve)
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
            pOwner->SendMessage(ECCN_CHANGEDVALUE, (WPARAM)this, 0 );
    }

    CWnd::OnLButtonDown(nFlags, point);
}

//-----------------------------------------------------------------------------
/**
*/
void CEnvelopeColorCurve::OnMouseMove(UINT nFlags, CPoint point)
{
    if( m_bSelectedCurve &&
        (MK_LBUTTON & nFlags) &&
        (MK_CONTROL & nFlags) )
    {
        if( CP_A == m_iSelectedCurve ||
            CP_D == m_iSelectedCurve )
            return;

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

        m_fCurvePos[ m_iSelectedCurve ] = static_cast<float>(pt.x);

        //m_pMemDC->GetPixel(pt.x - m_rcPanel.left, 0 );
        //m_clrCurve[ m_iSelectedCurve ] = m_pMemDC->GetPixel(pt.x - m_rcPanel.left, 0 );

        UpdateData( TRUE );
        InvalidateRect( NULL );
    }

    CWnd::OnMouseMove(nFlags, point);
}

//-----------------------------------------------------------------------------
/**
*/
void CEnvelopeColorCurve::OnDestroy()
{
    CWnd::OnDestroy();

    DestroyCompatibleBitmap();
}

//-----------------------------------------------------------------------------
/**
*/
void CEnvelopeColorCurve::OnPaint()
{
    CPaintDC dc(this); // device context for painting

    CRect rcClient;
    GetClientRect(&rcClient);

    // background
    dc.SelectStockObject( LTGRAY_BRUSH );
    dc.FillRect(&rcClient, NULL );

    // draw color
    int iPosX[CP_MAX];
    iPosX[CP_A] = 0;
    iPosX[CP_B] = m_fPos[CP_B] * m_rcPanel.Width();
    iPosX[CP_C] = m_fPos[CP_C] * m_rcPanel.Width();
    iPosX[CP_D] = m_rcPanel.Width();

    // 가독성을 위해 for 문 중첩하지 않음.
    COLORREF clr;
    // section 1
    float fWidth = float( iPosX[CP_B] - 1 );
    for( int x = 0; x < iPosX[CP_B]; ++x )
    {
        ColorLerp( &clr, &m_clrCurve[CP_A], &m_clrCurve[CP_B], x / fWidth );
        for( int y = 0; y < m_rcPanel.Height(); ++y )
            m_pMemDC->SetPixel( x, y, clr );
    }

    // section 2
    fWidth = float( iPosX[CP_C] - iPosX[CP_B] - 1 );
    for( int x = iPosX[CP_B]; x < iPosX[CP_C]; ++x )
    {
        ColorLerp( &clr, &m_clrCurve[CP_B], &m_clrCurve[CP_C], (x - iPosX[CP_B]) / fWidth );
        for( int y = 0; y < m_rcPanel.Height(); ++y )
            m_pMemDC->SetPixel( x, y, clr );
    }

    // section 3
    fWidth = float( iPosX[CP_D] - iPosX[CP_C]  - 1 );
    for( int x = iPosX[CP_C]; x < iPosX[CP_D]; ++x )
    {
        ColorLerp( &clr, &m_clrCurve[CP_C], &m_clrCurve[CP_D], (x - iPosX[CP_C]) / fWidth );
        for( int y = 0; y < m_rcPanel.Height(); ++y )
            m_pMemDC->SetPixel( x, y, clr );
    }

    dc.BitBlt( m_rcPanel.left, m_rcPanel.top, m_rcPanel.Width(), m_rcPanel.Height(), m_pMemDC, 0, 0, SRCCOPY);

    // curve point
    for( int i = 0 ; i < CP_MAX; ++i )
    {
        CBrush brush;
        brush.CreateSolidBrush( m_clrCurve[i] );
        dc.SelectObject(&brush);

        if( m_iSelectedCurve == i)
            dc.SelectStockObject( WHITE_PEN );    
        else
            dc.SelectStockObject( BLACK_PEN );

        dc.Ellipse( m_rcCurve[i] );
    }
}

//-----------------------------------------------------------------------------
/**
*/
void CEnvelopeColorCurve::OnRButtonDown(UINT nFlags, CPoint point)
{
    CMenu menu;

    menu.CreatePopupMenu();

    menu.AppendMenu(MF_STRING, ECC_FLIP_RAMP, "Flip Ramp");
    menu.AppendMenu(MF_STRING, ECC_ALL_KEYS_LIKE_THE_FIRST, "All Keys Like The First");
    menu.AppendMenu(MF_STRING, ECC_MAKE_GRADIENT, "Make Gradient");
    menu.AppendMenu(MF_STRING, ECC_BRIGHTER, "Brighter +10%");
    menu.AppendMenu(MF_STRING, ECC_DARKER, "Darker -10%");
    menu.AppendMenu(MF_SEPARATOR);
    menu.AppendMenu(MF_STRING, ECC_COPY_COLOR_RAMP, "Copy Color Ramp");
    menu.AppendMenu(MF_STRING, ECC_PASTE_COLOR_RAMP, "Paste Color Ramp");

    CPoint pt = point;
    ClientToScreen(&pt);
    menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, this);
    menu.DestroyMenu();

    CWnd::OnRButtonDown(nFlags, point);
}

//-----------------------------------------------------------------------------
/**
*/
void CEnvelopeColorCurve::OnFlipRamp()
{
    FlipRamp();
    UpdateData(TRUE);
    Invalidate();
}

//-----------------------------------------------------------------------------
/**
*/
void CEnvelopeColorCurve::OnAllKeysLikeTheFirst()
{
    AllKeysLikeTheFirst();
    UpdateData(TRUE);
    Invalidate();
}

//-----------------------------------------------------------------------------
/**
*/
void CEnvelopeColorCurve::OnMakeGradient()
{
    COLORREF a, b;
    CColorDialog dlg(GetCurveValue(CP_A), CC_FULLOPEN);
    if( dlg.DoModal() == IDOK )
    {
        a = dlg.GetColor();

        CColorDialog dlg2(GetCurveValue(CP_A), CC_FULLOPEN);
        if( dlg2.DoModal() == IDOK )
        {
            b = dlg2.GetColor();

            MakeGadient(a, b);
            UpdateData(TRUE);
            Invalidate();
        }
    }
}

//-----------------------------------------------------------------------------
/**
*/
void CEnvelopeColorCurve::OnBrighter()
{
    for( int i = 0; i < CP_MAX; ++i )
    {
        m_clrCurve[i] = ColorBrightness(m_clrCurve[i], 0.1f );
    }
    
    UpdateData(TRUE);
    Invalidate();
}

//-----------------------------------------------------------------------------
/**
*/
void CEnvelopeColorCurve::OnDarker()
{
    for( int i = 0; i < CP_MAX; ++i )
    {
        m_clrCurve[i] = ColorBrightness(m_clrCurve[i], -0.1f );
    }

    UpdateData(TRUE);
    Invalidate();
}

//-----------------------------------------------------------------------------
/**
*/
void CEnvelopeColorCurve::OnCopyColorRamp()
{
    OpenClipboard();
    EmptyClipboard();

    CString strData;
    strData.Format( "ColorCurveData %f %f %d %d %d %d",
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
void CEnvelopeColorCurve::OnPasteColorRamp()
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

    if( "ColorCurveData" != strBuffer )
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

    COLORREF clr;
    strBuffer = strData.Tokenize(" ",iPos);
    clr = atoi( strBuffer );
    SetCurveValue( CP_A, clr );

    strBuffer = strData.Tokenize(" ",iPos);
    clr = atoi( strBuffer );
    SetCurveValue( CP_B, clr );

    strBuffer = strData.Tokenize(" ",iPos);
    clr = atoi( strBuffer );
    SetCurveValue( CP_C, clr );

    strBuffer = strData.Tokenize(" ",iPos);
    clr = atoi( strBuffer );
    SetCurveValue( CP_D, clr );

    UpdateData(FALSE);
    InvalidateRect( NULL );

    CWnd* pOwner = GetOwner();
    ASSERT( pOwner );

    if( pOwner )
        pOwner->SendMessage(ECCN_CHANGEDVALUE, (WPARAM)this, 0 );
}
