//-----------------------------------------------------------------------------
//  nmaxenvelopecurveCtrl.cpp
//
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "nmaxenvelopecurve.h"
#include "nmaxenvelopecurveCtrl.h"
#include "nmaxenvelopecurvePropPage.h"

#include ".\nmaxenvelopecurvectrl.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


IMPLEMENT_DYNCREATE(nmaxenvelopecurveCtrl, COleControl)


// message map.

BEGIN_MESSAGE_MAP(nmaxenvelopecurveCtrl, COleControl)
    ON_MESSAGE(ECN_CHANGEDVALUE, OnECNChangedValue )
    //ON_MESSAGE(ECCN_CHANGEDVALUE, OnECCNChangedValue )
    ON_OLEVERB(AFX_IDS_VERB_PROPERTIES, OnProperties)
    ON_WM_CREATE()
END_MESSAGE_MAP()



// dispatch map.

BEGIN_DISPATCH_MAP(nmaxenvelopecurveCtrl, COleControl)
    DISP_PROPERTY_EX_ID(nmaxenvelopecurveCtrl, "ControlType", dispidControlType, GetControlType, SetControlType, VT_UI1)
    DISP_PROPERTY_EX_ID(nmaxenvelopecurveCtrl, "Min", dispidMin, GetMin, SetMin, VT_R4)
    DISP_PROPERTY_EX_ID(nmaxenvelopecurveCtrl, "Max", dispidMax, GetMax, SetMax, VT_R4)
    DISP_PROPERTY_EX_ID(nmaxenvelopecurveCtrl, "Frequency", dispidFrequency, GetFrequency, SetFrequency, VT_R4)
    DISP_PROPERTY_EX_ID(nmaxenvelopecurveCtrl, "Amplitude", dispidAmplitude, GetAmplitude, SetAmplitude, VT_R4)
    DISP_PROPERTY_EX_ID(nmaxenvelopecurveCtrl, "ModulationFunc", dispidFunction, GetFunction, SetFunction, VT_UI1)
    DISP_FUNCTION_ID(nmaxenvelopecurveCtrl, "GetSelectedCurve", dispidGetSelectedCurve, GetSelectedCurve, VT_UI1, VTS_NONE)
    DISP_FUNCTION_ID(nmaxenvelopecurveCtrl, "UpdateDialogControls", dispidUpdateDialogControls, UpdateDialogControls, VT_EMPTY, VTS_NONE)
    DISP_PROPERTY_EX_ID(nmaxenvelopecurveCtrl, "Name", dispidName, GetName, SetName, VT_BSTR)
    DISP_FUNCTION_ID(nmaxenvelopecurveCtrl, "init", dispidinit, init, VT_EMPTY, VTS_UI1 VTS_BSTR VTS_PR4 VTS_R4 VTS_R4)
    DISP_FUNCTION_ID(nmaxenvelopecurveCtrl, "GetData", dispidGetData, GetData, VT_BSTR, VTS_NONE)
    DISP_FUNCTION_ID(nmaxenvelopecurveCtrl, "SetPos", dispidSetPos, SetPos, VT_EMPTY, VTS_UI1 VTS_R4)
    DISP_FUNCTION_ID(nmaxenvelopecurveCtrl, "GetPos", dispidGetPos, GetPos, VT_R4, VTS_UI1)
    DISP_FUNCTION_ID(nmaxenvelopecurveCtrl, "SetValue", dispidSetValue, SetValue, VT_EMPTY, VTS_UI1 VTS_R4)
    DISP_FUNCTION_ID(nmaxenvelopecurveCtrl, "GetValue", dispidGetValue, GetValue, VT_R4, VTS_UI1)
    DISP_FUNCTION_ID(nmaxenvelopecurveCtrl, "SetColor", dispidSetColor, SetColor, VT_EMPTY, VTS_UI1 VTS_COLOR)
    DISP_FUNCTION_ID(nmaxenvelopecurveCtrl, "GetColor", dispidGetColor, GetColor, VT_COLOR, VTS_UI1)
END_DISPATCH_MAP()



// event map.

BEGIN_EVENT_MAP(nmaxenvelopecurveCtrl, COleControl)
    EVENT_CUSTOM_ID("OnChangedValue", eventidOnChangedValue, OnChangedValue, VTS_NONE)
END_EVENT_MAP()

// property page.


BEGIN_PROPPAGEIDS(nmaxenvelopecurveCtrl, 1)
    PROPPAGEID(nmaxenvelopecurvePropPage::guid)
END_PROPPAGEIDS(nmaxenvelopecurveCtrl)


// initialize class factory and GUID.

IMPLEMENT_OLECREATE_EX(nmaxenvelopecurveCtrl, "NMAXENVELOPECURV.nmaxenvelopecurvCtrl.1",
    0xeda6dbcd, 0xa8bb, 0x4709, 0xab, 0x92, 0x40, 0x18, 0x1c, 0xc, 0x58, 0xce)


IMPLEMENT_OLETYPELIB(nmaxenvelopecurveCtrl, _tlid, _wVerMajor, _wVerMinor)


const IID BASED_CODE IID_Dnmaxenvelopecurve =
        { 0x949C1889, 0x5D48, 0x44FF, { 0xB4, 0x22, 0xCB, 0xE7, 0x98, 0xF6, 0x40, 0x39 } };
const IID BASED_CODE IID_DnmaxenvelopecurveEvents =
        { 0xBC8E451C, 0x50C9, 0x4F41, { 0x9F, 0x4D, 0x71, 0x13, 0xC3, 0x21, 0x2D, 0x5 } };


static const DWORD BASED_CODE _dwnmaxenvelopecurveOleMisc =
    OLEMISC_ACTIVATEWHENVISIBLE |
    OLEMISC_SETCLIENTSITEFIRST |
    OLEMISC_INSIDEOUT |
    OLEMISC_CANTLINKINSIDE |
    OLEMISC_RECOMPOSEONRESIZE;

IMPLEMENT_OLECTLTYPE(nmaxenvelopecurveCtrl, IDS_NMAXENVELOPECURVE, _dwnmaxenvelopecurveOleMisc)


BOOL nmaxenvelopecurveCtrl::nmaxenvelopecurveCtrlFactory::UpdateRegistry(BOOL bRegister)
{
    if (bRegister)
        return AfxOleRegisterControlClass(
            AfxGetInstanceHandle(),
            m_clsid,
            m_lpszProgID,
            IDS_NMAXENVELOPECURVE,
            IDB_NMAXENVELOPECURVE,
            afxRegApartmentThreading,
            _dwnmaxenvelopecurveOleMisc,
            _tlid,
            _wVerMajor,
            _wVerMinor);
    else
        return AfxOleUnregisterClass(m_clsid, m_lpszProgID);
}

//-----------------------------------------------------------------------------
/**
*/
nmaxenvelopecurveCtrl::nmaxenvelopecurveCtrl()
{
    InitializeIIDs(&IID_Dnmaxenvelopecurve, &IID_DnmaxenvelopecurveEvents);
    m_ControlType = CT_MAX;
}

//-----------------------------------------------------------------------------
/**
*/
nmaxenvelopecurveCtrl::~nmaxenvelopecurveCtrl()
{
}

//-----------------------------------------------------------------------------
/**
*/
void nmaxenvelopecurveCtrl::OnDraw(
            CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid)
{
    if (!pdc)
        return;

    switch( m_ControlType )
    {
    case CT_Curve:
        m_CurveDlg.MoveWindow( rcBounds );
        break;
    case CT_ColorCurve:
        m_ColorCurveDlg.MoveWindow( rcBounds );
        break;
    }
}

//-----------------------------------------------------------------------------
/**
*/
void nmaxenvelopecurveCtrl::DoPropExchange(CPropExchange* pPX)
{
    ExchangeVersion(pPX, MAKELONG(_wVerMinor, _wVerMajor));
    COleControl::DoPropExchange(pPX);
}

//-----------------------------------------------------------------------------
/**
*/
void nmaxenvelopecurveCtrl::OnResetState()
{
    COleControl::OnResetState();
}

//-----------------------------------------------------------------------------
/**
   nmaxenvelopecurveCtrl message handler.
*/
int nmaxenvelopecurveCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (COleControl::OnCreate(lpCreateStruct) == -1)
        return -1;

    m_CurveDlg.Create( nmaxenvelopecurveDialog::IDD, this );
    m_ColorCurveDlg.Create( nmaxenvelopecolorcurveDialog::IDD, this );

    SetControlType( CT_Curve );

/*
    CRect rcClient;
    switch( m_ControlType )
    {
    case CT_Curve:
        m_CurveDlg.Create( nmaxenvelopecurveDialog::IDD, this );
        break;
    case CT_ColorCurve:
        m_ColorCurveDlg.Create( nmaxenvelopecolorcurveDialog::IDD, this );
        break;
    }
*/
    return 0;
}

//-----------------------------------------------------------------------------
/**
*/
BYTE nmaxenvelopecurveCtrl::GetControlType(void)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    return m_ControlType;
}

//-----------------------------------------------------------------------------
/**
*/
void nmaxenvelopecurveCtrl::SetControlType(BYTE newVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    if( newVal > CT_ColorCurve )
        newVal = CT_ColorCurve;

    if( m_ControlType != newVal )
    {
        /*
        switch( m_ControlType )
        {
        case CT_Curve:
            m_ColorCurveDlg.DestroyWindow();
            m_CurveDlg.Create( nmaxenvelopecurveDialog::IDD, this );
            break;
        case CT_ColorCurve:
            m_CurveDlg.DestroyWindow();
            m_ColorCurveDlg.Create( nmaxenvelopecolorcurveDialog::IDD, this );
            break;
        }
        */

        switch( newVal )
        {
        case CT_Curve:
            m_ColorCurveDlg.ShowWindow(SW_HIDE);
            m_CurveDlg.ShowWindow(SW_SHOW);
            break;
        case CT_ColorCurve:
            m_CurveDlg.ShowWindow(SW_HIDE);
            m_ColorCurveDlg.ShowWindow(SW_SHOW);
            break;
        }

        m_ControlType = newVal;
    }

    SetModifiedFlag();
}

//-----------------------------------------------------------------------------
/**
*/
BSTR nmaxenvelopecurveCtrl::GetName(void)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    CString strResult;

    switch( m_ControlType )
    {
    case CT_Curve:
        {
            m_CurveDlg.GetWindowText(strResult);
        }
        break;
    case CT_ColorCurve:
        {
            m_ColorCurveDlg.GetWindowText(strResult);
        }
        break;
    }

    return strResult.AllocSysString();
}

//-----------------------------------------------------------------------------
/**
*/
void nmaxenvelopecurveCtrl::SetName(LPCTSTR newVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    switch( m_ControlType )
    {
    case CT_Curve:
        {
            m_CurveDlg.SetWindowText(newVal);
        }
        break;
    case CT_ColorCurve:
        {
            m_ColorCurveDlg.SetWindowText(newVal);
        }
        break;
    }
  
    SetModifiedFlag();
}

//-----------------------------------------------------------------------------
/**
*/
FLOAT nmaxenvelopecurveCtrl::GetMin(void)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    CEnvelopeCurve* pControl = m_CurveDlg.GetEnvelopeCurve();
    return pControl->GetMin();
}

//-----------------------------------------------------------------------------
/**
*/
void nmaxenvelopecurveCtrl::SetMin(FLOAT newVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    if( m_ControlType != CT_Curve )
        return;

    CEnvelopeCurve* pControl = m_CurveDlg.GetEnvelopeCurve();
    pControl->SetMin( newVal );

    SetModifiedFlag();
}

//-----------------------------------------------------------------------------
/**
*/
FLOAT nmaxenvelopecurveCtrl::GetMax(void)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    CEnvelopeCurve* pControl = m_CurveDlg.GetEnvelopeCurve();
    return pControl->GetMax();
}

//-----------------------------------------------------------------------------
/**
*/
void nmaxenvelopecurveCtrl::SetMax(FLOAT newVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    if( m_ControlType != CT_Curve )
        return;

    CEnvelopeCurve* pControl = m_CurveDlg.GetEnvelopeCurve();
    pControl->SetMax( newVal );

    SetModifiedFlag();
}

//-----------------------------------------------------------------------------
/**
*/
FLOAT nmaxenvelopecurveCtrl::GetFrequency(void)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    CEnvelopeCurve* pControl = m_CurveDlg.GetEnvelopeCurve();
    return pControl->GetFrequency();
}

//-----------------------------------------------------------------------------
/**
*/
void nmaxenvelopecurveCtrl::SetFrequency(FLOAT newVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    if( m_ControlType != CT_Curve )
        return;

    CEnvelopeCurve* pControl = m_CurveDlg.GetEnvelopeCurve();
    pControl->SetFrequency( newVal );

    SetModifiedFlag();
}

//-----------------------------------------------------------------------------
/**
*/
FLOAT nmaxenvelopecurveCtrl::GetAmplitude(void)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    CEnvelopeCurve* pControl = m_CurveDlg.GetEnvelopeCurve();
    return pControl->GetAmplitude();
}

//-----------------------------------------------------------------------------
/**
*/
void nmaxenvelopecurveCtrl::SetAmplitude(FLOAT newVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    if( m_ControlType != CT_Curve )
        return;

    CEnvelopeCurve* pControl = m_CurveDlg.GetEnvelopeCurve();
    pControl->SetAmplitude( newVal );

    SetModifiedFlag();
}

//-----------------------------------------------------------------------------
/**
*/
BYTE nmaxenvelopecurveCtrl::GetFunction(void)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    CEnvelopeCurve* pControl = m_CurveDlg.GetEnvelopeCurve();
    return pControl->GetFunction();
}

//-----------------------------------------------------------------------------
/**
*/
void nmaxenvelopecurveCtrl::SetFunction(BYTE newVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    if( m_ControlType != CT_Curve )
        return;
    
    CEnvelopeCurve* pControl = m_CurveDlg.GetEnvelopeCurve();
    pControl->SetFunction( newVal );

    SetModifiedFlag();
}

//-----------------------------------------------------------------------------
/**
*/
BYTE nmaxenvelopecurveCtrl::GetSelectedCurve(void)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    switch( m_ControlType )
    {
    case CT_Curve:
        {
            CEnvelopeCurve* pControl = m_CurveDlg.GetEnvelopeCurve();
            return pControl->GetSelectedCurve();
        }
        break;
    case CT_ColorCurve:
        {
            CEnvelopeColorCurve* pControl = m_ColorCurveDlg.GetEnvelopeColorCurve();
            return pControl->GetSelectedCurve();
        }
        break;
    }

    return 0;
}

//-----------------------------------------------------------------------------
/**
    Called to update control status after set member function is called.
*/
void nmaxenvelopecurveCtrl::UpdateDialogControls(void)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    switch( m_ControlType )
    {
    case CT_Curve:
        {
            m_CurveDlg.UpdateDialogControls();
        }
        break;
    case CT_ColorCurve:
        {
            m_ColorCurveDlg.UpdateDialogControls();
        }
        break;
    }
}

//-----------------------------------------------------------------------------
/**
*/
void nmaxenvelopecurveCtrl::init(BYTE type, LPCTSTR name, FLOAT* defValue, FLOAT min, FLOAT max)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    SetControlType( type );
    SetName( name );

    for( int i = 0; i < 4; ++i )
        SetValue( i, defValue[i] );
    
    SetPos( 1, defValue[4] );
    SetPos( 2, defValue[5] );
    SetFrequency(defValue[6]);
    SetAmplitude(defValue[7]);
    SetMin( min );
    SetMin( max );

    UpdateDialogControls();
}

//-----------------------------------------------------------------------------
/**
*/
LRESULT nmaxenvelopecurveCtrl::OnECNChangedValue(WPARAM wParam, LPARAM lParam)
{
    OnChangedValue();

    return S_OK;
}

//-----------------------------------------------------------------------------
/**
*/
BSTR nmaxenvelopecurveCtrl::GetData(void)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    CString strResult;

    switch( m_ControlType )
    {
    case CT_Curve:
        {
            CEnvelopeCurve* pControl = m_CurveDlg.GetEnvelopeCurve();
            strResult.Format( "%f %f %f %f %f %f %f %f %f %f %d",
                pControl->GetCurveValue(0),
                pControl->GetCurveValue(1),
                pControl->GetCurveValue(2),
                pControl->GetCurveValue(3),
                pControl->GetCurvePos(1),
                pControl->GetCurvePos(2),
                pControl->GetMin(),
                pControl->GetMax(),
                pControl->GetFrequency(),
                pControl->GetAmplitude(),
                pControl->GetFunction()
                );
        }
        break;
    case CT_ColorCurve:
        {
            CEnvelopeColorCurve* pControl = m_ColorCurveDlg.GetEnvelopeColorCurve();
        }
        break;
    }

    return strResult.AllocSysString();
}

//-----------------------------------------------------------------------------
/**
*/
void nmaxenvelopecurveCtrl::SetPos(BYTE index, FLOAT value)
{
    // prevent to set index 0 and 3.
    if( 0 == index ||
        3 == index )
        return;

    switch( m_ControlType )
    {
    case CT_Curve:
        {
            CEnvelopeCurve* pControl = m_CurveDlg.GetEnvelopeCurve();
            pControl->SetCurvePos( index, value );
        }
        break;
    case CT_ColorCurve:
        {
            CEnvelopeColorCurve* pControl = m_ColorCurveDlg.GetEnvelopeColorCurve();
            pControl->SetCurvePos( index, value );
        }
        break;
    }
}

//-----------------------------------------------------------------------------
/**
*/
FLOAT nmaxenvelopecurveCtrl::GetPos(BYTE index)
{
    if( index > 3 ) index = 3;

    switch( m_ControlType )
    {
    case CT_Curve:
        {
            CEnvelopeCurve* pControl = m_CurveDlg.GetEnvelopeCurve();
            return pControl->GetCurvePos( index );
        }
        break;
    case CT_ColorCurve:
        {
            CEnvelopeColorCurve* pControl = m_ColorCurveDlg.GetEnvelopeColorCurve();
            return pControl->GetCurvePos( index );
        }
        break;
    }

    return 0;    
}

//-----------------------------------------------------------------------------
/**
*/
void nmaxenvelopecurveCtrl::SetValue(BYTE index, FLOAT value)
{
    if( m_ControlType != CT_Curve )
        return;

    CEnvelopeCurve* pControl = m_CurveDlg.GetEnvelopeCurve();
    pControl->SetCurveValue( index, value );
}

//-----------------------------------------------------------------------------
/**
*/
FLOAT nmaxenvelopecurveCtrl::GetValue(BYTE index)
{
    if( m_ControlType != CT_Curve )
        return -1.0f;

    CEnvelopeCurve* pControl = m_CurveDlg.GetEnvelopeCurve();
    return pControl->GetCurveValue( index );
}

//-----------------------------------------------------------------------------
/**
*/
void nmaxenvelopecurveCtrl::SetColor(BYTE index, OLE_COLOR value)
{
    if( m_ControlType != CT_ColorCurve )
        return;

    CEnvelopeColorCurve* pControl = m_ColorCurveDlg.GetEnvelopeColorCurve();
    pControl->SetCurveValue( index, value );
}

//-----------------------------------------------------------------------------
/**
*/
OLE_COLOR nmaxenvelopecurveCtrl::GetColor(BYTE index)
{
    if( m_ControlType != CT_ColorCurve )
        return OLE_COLOR(RGB(0,0,0));

    CEnvelopeColorCurve* pControl = m_ColorCurveDlg.GetEnvelopeColorCurve();
    return OLE_COLOR(pControl->GetCurveValue( index ));
}
