#pragma once
//-----------------------------------------------------------------------------
/**
    @class nmaxenvelopecolorcurveDialog
    @brief 

*/
#include "EnvelopeColorCurve.h"
#include "afxwin.h"

//-----------------------------------------------------------------------------
class nmaxenvelopecolorcurveDialog : public CDialog
{
    DECLARE_DYNAMIC(nmaxenvelopecolorcurveDialog)

public:
    /// Constructor.
    nmaxenvelopecolorcurveDialog(CWnd* pParent = NULL);
    /// Destructor.
    virtual ~nmaxenvelopecolorcurveDialog();

    /// dialog data.
    enum { IDD = IDD_ENVELOPE_COLOR_CURVE };

private:
    CEnvelopeColorCurve m_ColorCurveControl;

protected:
    /// DDX/DDV
    virtual void DoDataExchange(CDataExchange* pDX);    

    float AdjustMinMaxFloat( UINT nID, float fMin, float fMax, float fRevision );

    DECLARE_MESSAGE_MAP()

public:
    CEnvelopeColorCurve* GetEnvelopeColorCurve() { return &m_ColorCurveControl;}
    void UpdateDialogControls();

    virtual BOOL OnInitDialog();

    afx_msg LRESULT OnECCNChangedValue(WPARAM wParam, LPARAM lParam);
    afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
    afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
    afx_msg void OnBnDoubleclickedBtnSelectedColor();
    afx_msg void OnPaint();
    afx_msg void OnEnChangeEditSelectedPos();
};
//-----------------------------------------------------------------------------