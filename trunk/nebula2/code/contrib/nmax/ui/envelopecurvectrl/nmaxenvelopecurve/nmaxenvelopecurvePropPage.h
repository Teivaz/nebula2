#pragma once
//-----------------------------------------------------------------------------
/**
    @class nmaxenvelopecurvePropPage
    @brief 

*/
class nmaxenvelopecurvePropPage : public COlePropertyPage
{
	DECLARE_DYNCREATE(nmaxenvelopecurvePropPage)
	DECLARE_OLECREATE_EX(nmaxenvelopecurvePropPage)

public:
    /// Constructor.
	nmaxenvelopecurvePropPage();

    //dialog data.
	enum { IDD = IDD_PROPPAGE_NMAXENVELOPECURVE };

// ±¸Çö
protected:
    /// DDX/DDV.
	virtual void DoDataExchange(CDataExchange* pDX);    

protected:
    /// message map.
	DECLARE_MESSAGE_MAP()

};
//-----------------------------------------------------------------------------
