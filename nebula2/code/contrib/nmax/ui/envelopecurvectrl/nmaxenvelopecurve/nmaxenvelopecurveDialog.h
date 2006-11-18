#pragma once
//-----------------------------------------------------------------------------
/**
    @class nmaxenvelopecurveDialog
    @brief 

*/
#include "EnvelopeCurve.h"
#include "afxwin.h"

//-----------------------------------------------------------------------------
class nmaxenvelopecurveDialog : public CDialog
{
    DECLARE_DYNAMIC(nmaxenvelopecurveDialog)

public:
    /// Constructor.
    nmaxenvelopecurveDialog(CWnd* pParent = NULL);
    virtual ~nmaxenvelopecurveDialog();
    
    /// Dialog data.
    enum { IDD = IDD_ENVELOPE_CURVE };

private:
    CEnvelopeCurve m_CurveControl;
    //CFont m_font;

protected:
    /// DDX/DDV
    virtual void DoDataExchange(CDataExchange* pDX);

    float AdjustMinMaxFloat( UINT nID, float fMin, float fMax, float fRevision );

    DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();

    CEnvelopeCurve* GetEnvelopeCurve() { return &m_CurveControl; }
    void UpdateDialogControls();

public:
    afx_msg LRESULT OnECNChangedValue(WPARAM wParam, LPARAM lParam);
    afx_msg void OnEnChangeEditSelectedPos();
    afx_msg void OnEnChangeEditSelectedValue();
    afx_msg void OnEnChangeEditMax();
    afx_msg void OnEnChangeEditMin();
    afx_msg void OnEnChangeEditFrequency();
    afx_msg void OnEnChangeEditAmplitude();
    afx_msg void OnCbnSelchangeComboFunction();
    afx_msg void OnBnClickedBtnToggle();
    afx_msg void OnPaint();
protected:
    virtual void OnOK();
    virtual void OnCancel();
};
//-----------------------------------------------------------------------------
