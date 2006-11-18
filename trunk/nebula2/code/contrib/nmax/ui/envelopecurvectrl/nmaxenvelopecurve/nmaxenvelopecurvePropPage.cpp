//-----------------------------------------------------------------------------
//  nmaxenvelopecurvePropPage.cpp
//
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "nmaxenvelopecurve.h"
#include "nmaxenvelopecurvePropPage.h"
#include ".\nmaxenvelopecurveproppage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


IMPLEMENT_DYNCREATE(nmaxenvelopecurvePropPage, COlePropertyPage)


BEGIN_MESSAGE_MAP(nmaxenvelopecurvePropPage, COlePropertyPage)
END_MESSAGE_MAP()


IMPLEMENT_OLECREATE_EX(nmaxenvelopecurvePropPage, "NMAXENVELOPECU.nmaxenvelopecuPropPage.1",
    0x9f67fef8, 0xe83e, 0x4e88, 0xbd, 0xc4, 0x9f, 0x31, 0x32, 0xf2, 0xff, 0xa)

//-----------------------------------------------------------------------------
/**
*/
BOOL nmaxenvelopecurvePropPage::nmaxenvelopecurvePropPageFactory::UpdateRegistry(BOOL bRegister)
{
    if (bRegister)
        return AfxOleRegisterPropertyPageClass(AfxGetInstanceHandle(),
            m_clsid, IDS_NMAXENVELOPECURVE_PPG);
    else
        return AfxOleUnregisterClass(m_clsid, NULL);
}

//-----------------------------------------------------------------------------
/**
*/
nmaxenvelopecurvePropPage::nmaxenvelopecurvePropPage() :
    COlePropertyPage(IDD, IDS_NMAXENVELOPECURVE_PPG_CAPTION)
{
}

//-----------------------------------------------------------------------------
/**
*/
void nmaxenvelopecurvePropPage::DoDataExchange(CDataExchange* pDX)
{
    DDP_PostProcessing(pDX);
}
