#pragma once


// CEnvelopeColorCurve

#define ECCN_CHANGEDVALUE            (WM_USER + 1001)

#define ECC_FLIP_RAMP                (WM_USER + 3001)
#define ECC_ALL_KEYS_LIKE_THE_FIRST  (WM_USER + 3002)
#define ECC_MAKE_GRADIENT            (WM_USER + 3003)
#define ECC_BRIGHTER                 (WM_USER + 3004)
#define ECC_DARKER                   (WM_USER + 3005)
#define ECC_COPY_COLOR_RAMP          (WM_USER + 3006)
#define ECC_PASTE_COLOR_RAMP         (WM_USER + 3007)

//-----------------------------------------------------------------------------
/**
    @class CEnvelopeColorCurve
    @brief 
*/
class CEnvelopeColorCurve : public CWnd
{
    DECLARE_DYNAMIC(CEnvelopeColorCurve)

public:
    CEnvelopeColorCurve();
    virtual ~CEnvelopeColorCurve();

    enum CurvePoint
    {
        CP_A,
        CP_B,
        CP_C,
        CP_D,
        CP_MAX,
    };

    void SetCurvePos( int index, float pos );
    void SetSelectedCurvePos( float pos );
    void SetCurveValue( int index, COLORREF clr );
    void SetCurveValue( int index, float r, float g, float b );
    void SetSelectedCurveValue( COLORREF clr );
    void SetSelectedCurveValue( float r, float g, float b );
    
    float GetCurvePos( int index );
    float GetSelectedCurvePos();
    COLORREF GetCurveValue( int index );
    COLORREF GetSelectedCurveValue();
    int GetSelectedCurve() { return m_iSelectedCurve; }

    void FlipRamp();
    void AllKeysLikeTheFirst();
    void MakeGadient( COLORREF a, COLORREF b);
    void Brighter();
    void Darker();

    void UpdateData( BOOL b );

private:
    // for control
    CRect    m_rcPanel;
    CPoint   m_ptCurve[CP_MAX];
    CRect    m_rcCurve[CP_MAX];
    int      m_iSelectedCurve;
    BOOL     m_bSelectedCurve;

    CDC*     m_pMemDC;
    CBitmap* m_pBitmap;

    // helper
    float    m_fCurvePos[CP_MAX];

    // for envelope color curve
    float    m_fPos[CP_MAX];
    COLORREF m_clrCurve[CP_MAX];

    int SelectedCurve( CPoint& pt );

    void ColorLerp( COLORREF* pOut, const COLORREF* pSrc, const COLORREF* pDest, float s );
    COLORREF ColorBrightness(COLORREF clr, float s );

    void CreateCompatibleBitmap( int width, int height );
    void DestroyCompatibleBitmap();

protected:
    DECLARE_MESSAGE_MAP()
public:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
    afx_msg void OnDestroy();
    afx_msg void OnPaint();
    afx_msg void OnRButtonDown(UINT nFlags, CPoint point);

    afx_msg void OnFlipRamp();
    afx_msg void OnAllKeysLikeTheFirst();
    afx_msg void OnMakeGradient();
    afx_msg void OnBrighter();
    afx_msg void OnDarker();
    afx_msg void OnCopyColorRamp();
    afx_msg void OnPasteColorRamp();
};
//-----------------------------------------------------------------------------
