#pragma once

#define ECN_CHANGEDVALUE            (WM_USER + 1001)

#define EC_FLIP_CURVE_VERTICAL      (WM_USER + 2001)
#define EC_FLIP_CURVE_HORIZONTAL    (WM_USER + 2002)
#define EC_STRAIGHTEN_CURVE         (WM_USER + 2003)
#define EC_COPY_CURVE               (WM_USER + 2004)
#define EC_PASTE_CURVE              (WM_USER + 2005)
#define EC_ALL_KEYS_TO_MAX          (WM_USER + 2006)
#define EC_ALL_KEYS_TO_MID          (WM_USER + 2007)
#define EC_ALL_KEYS_TO_MIN          (WM_USER + 2008)

//-----------------------------------------------------------------------------
/**
    @struct Point2f
    @brief 

*/
struct Point2f
{
    float x, y;
};

//-----------------------------------------------------------------------------
/**
    @class CEnvelopeCurve
    @brief 

    TODO:
    Where put the priority when both of control points are intersected
    We need to find a better way to get control point what we want to select 
    when both of control points are overlapped.
*/
class CEnvelopeCurve : public CWnd
{
    DECLARE_DYNAMIC(CEnvelopeCurve)

public:
    /// Constructor.
    CEnvelopeCurve();
    /// Destructor.
    virtual ~CEnvelopeCurve();

    enum CurvePoint
    {
        CP_A,
        CP_B,
        CP_C,
        CP_D,
        CP_MAX,
    };
    
    void SetCurvePos( int index, float pos );
    void SetCurveValue( int index, float value );
    void SetSelectedCurvePos( float pos );
    void SetSelectedCurveValue( float value );
    void SetMin( float fMin );
    void SetMax( float fMax );
    void SetFrequency( float fFrequency );
    void SetAmplitude( float fAmplitude );
    void SetFunction( int iFunc );
    
    float GetCurvePos( int index );
    float GetCurveValue( int index );
    float GetSelectedCurvePos();
    float GetSelectedCurveValue();
    float GetMin();
    float GetMax();
    float GetFrequency();
    float GetAmplitude();
    int GetFunction();
    int GetSelectedCurve();
    

    void UpdateData( BOOL b );

    void FlipCruveVertical();
    void FlipCurveHorizontal();
    void StraightenCurve();
    void AllKeysToMaximum();
    void AllKeysToMiddle();
    void AllKeysToMinimum();
    
private:
    // for control
    CRect     m_rcPanel;
    CPoint    m_ptCurve[CP_MAX];
    Point2f   m_fCurve[CP_MAX];        // member to decrease error.
    CRect     m_rcCurve[CP_MAX];
    CPoint    m_ptPolygon[6];
    int       m_iSelectedCurve;
    BOOL      m_bSelectedCurve;
    CMenu     m_menu;

    // for envelope curve
    float     m_fPos[CP_MAX];
    float     m_fValue[CP_MAX];
    float     m_fMin, m_fMax;
    float     m_fFrequency, m_fAmplitude;
    int       m_iModulationFunc;

    int SelectedCurve( CPoint& pt );
    
protected:
    DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnPaint();
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnRButtonDown(UINT nFlags, CPoint point);

    afx_msg void OnFlipCurveVertical();
    afx_msg void OnFlipCurveHorizontal();
    afx_msg void OnStraightenCurve();
    afx_msg void OnCopyCurve();
    afx_msg void OnPasteCurve();
    afx_msg void OnAllKeysToMax();
    afx_msg void OnAllKeysToMid();
    afx_msg void OnAllKeysToMin();
};
//-----------------------------------------------------------------------------
