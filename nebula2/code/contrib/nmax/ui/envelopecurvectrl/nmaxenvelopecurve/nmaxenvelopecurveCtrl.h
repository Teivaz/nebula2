#pragma once
//-----------------------------------------------------------------------------
/**
    @class nmaxenvelopecurveCtrl
    @brief 

*/
#include "nmaxenvelopecurveDialog.h"
#include "nmaxenvelopecolorcurveDialog.h"

//-----------------------------------------------------------------------------
class nmaxenvelopecurveCtrl : public COleControl
{
    DECLARE_DYNCREATE(nmaxenvelopecurveCtrl)

public:
    /// Constructor.
    nmaxenvelopecurveCtrl();

private:
    nmaxenvelopecurveDialog    m_CurveDlg;
    nmaxenvelopecolorcurveDialog m_ColorCurveDlg;

// override
public:
    virtual void OnDraw(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid);
    virtual void DoPropExchange(CPropExchange* pPX);
    virtual void OnResetState();

// implementation.
protected:
    ~nmaxenvelopecurveCtrl();

    DECLARE_OLECREATE_EX(nmaxenvelopecurveCtrl)    
    DECLARE_OLETYPELIB(nmaxenvelopecurveCtrl)      
    DECLARE_PROPPAGEIDS(nmaxenvelopecurveCtrl)     
    DECLARE_OLECTLTYPE(nmaxenvelopecurveCtrl)      

// message map.
    DECLARE_MESSAGE_MAP()

// dispatch map.
    DECLARE_DISPATCH_MAP()

// event map.
    DECLARE_EVENT_MAP()

// dispatch and event ID.
public:
    enum {
        dispidGetColor = 24L,
        dispidSetColor = 23L,
        dispidGetValue = 22L,
        dispidSetValue = 21L,
        dispidGetPos = 20L,
        dispidSetPos = 19L,
        dispidGetData = 16L,
        eventidOnChangedValue = 1L,
        dispidinit = 15L,
        dispidName = 13,
        dispidUpdateDialogControls = 12L,
        dispidGetSelectedCurve = 10L,
        dispidFunction = 9,
        dispidAmplitude = 8,
        dispidFrequency = 7,
        dispidMax = 6,
        dispidMin = 5,
        dispidControlType = 2
    };
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

protected:
    enum ControlType
    {
        CT_Curve,
        CT_ColorCurve,
        CT_MAX,
    };

    BSTR GetData(void);

    BYTE m_ControlType;
    BYTE GetControlType(void);
    void SetControlType(BYTE newVal);

    BSTR GetName(void);
    void SetName(LPCTSTR newVal);

    void SetPos(BYTE index, FLOAT value);
    FLOAT GetPos(BYTE index);

    void SetValue(BYTE index, FLOAT value);
    FLOAT GetValue(BYTE index);

    void SetColor(BYTE index, OLE_COLOR value);
    OLE_COLOR GetColor(BYTE index);
    
    FLOAT GetMin(void);
    void SetMin(FLOAT newVal);
    FLOAT GetMax(void);
    void SetMax(FLOAT newVal);
    FLOAT GetFrequency(void);
    void SetFrequency(FLOAT newVal);
    FLOAT GetAmplitude(void);
    void SetAmplitude(FLOAT newVal);
    BYTE GetFunction(void);
    void SetFunction(BYTE newVal);
    BYTE GetSelectedCurve(void);
    
    void UpdateDialogControls(void);
    void init(BYTE type, LPCTSTR name, FLOAT* defValue, FLOAT min, FLOAT max);

    afx_msg LRESULT OnECNChangedValue(WPARAM wParam, LPARAM lParam);
    void OnChangedValue(void)
    {
        FireEvent(eventidOnChangedValue, EVENT_PARAM(VTS_NONE));
    }
};
//-----------------------------------------------------------------------------