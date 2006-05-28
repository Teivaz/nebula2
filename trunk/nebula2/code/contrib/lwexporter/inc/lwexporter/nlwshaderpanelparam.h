#ifndef N_LW_SHADER_PANEL_PARAM_H
#define N_LW_SHADER_PANEL_PARAM_H
//----------------------------------------------------------------------------
extern "C"
{
#include <lwsdk/lwserver.h>
}

#include "util/narray.h"
#include "util/nstring.h"
#include "gfx2/nshaderstate.h"
#include "wx/wx.h"

class TiXmlElement;
class nLWShaderExportSettings;
class nLWPanel;
class wxColorAlphaCtrl;
class wxFloatSliderCtrl;
class wxIntSliderCtrl;
class wxTextureFileCtrl;

//----------------------------------------------------------------------------
/**
*/
class nLWShaderPanelParam
{
public:
    nLWShaderPanelParam();
    virtual ~nLWShaderPanelParam();

    const nString& GetLabel() const;
    bool Visible() const;
    virtual bool NeedsLabelControl() const;
    virtual void Load(const TiXmlElement* paramElem);
    virtual void CopyParamTo(nLWShaderExportSettings*) = 0;
    virtual void CopyParamFrom(nLWShaderExportSettings*) = 0;
    virtual wxWindow* CreateControl(wxWindow* parent) = 0;
    virtual nLWShaderPanelParam* Clone() = 0;

protected:
    nString label;
    nShaderState::Param shaderParam;
    bool visible;
};

//----------------------------------------------------------------------------
/**
*/
class nLWShaderPanelPlaceHolderParam : public nLWShaderPanelParam
{
public:
    nLWShaderPanelPlaceHolderParam();
    virtual ~nLWShaderPanelPlaceHolderParam();

    void Load(const TiXmlElement* paramElem);
    void CopyParamTo(nLWShaderExportSettings*);
    void CopyParamFrom(nLWShaderExportSettings*);
    wxWindow* CreateControl(wxWindow* parent);
    nLWShaderPanelParam* Clone();

protected:
    nString paramType;
};

//----------------------------------------------------------------------------
/**
*/
class nLWShaderPanelEnumParam : public nLWShaderPanelParam
{
public:
    nLWShaderPanelEnumParam();
    ~nLWShaderPanelEnumParam();
    void Load(const TiXmlElement* paramElem);
    void CopyParamTo(nLWShaderExportSettings*);
    void CopyParamFrom(nLWShaderExportSettings*);
    wxWindow* CreateControl(wxWindow* parent);
    nLWShaderPanelParam* Clone();

private:
    nArray<nString> enumMembers;
    int firstMemberId;
    int defMemberId;
    wxChoice* ctrl;
};

//----------------------------------------------------------------------------
/**
*/
class nLWShaderPanelIntParam : public nLWShaderPanelParam
{
public:
    nLWShaderPanelIntParam();
    void Load(const TiXmlElement* paramElem);
    void CopyParamTo(nLWShaderExportSettings*);
    void CopyParamFrom(nLWShaderExportSettings*);
    wxWindow* CreateControl(wxWindow* parent);
    nLWShaderPanelParam* Clone();

private:
    int valueLowerBound;
    int valueUpperBound;
    int defValue;
    wxIntSliderCtrl* ctrl;
};

//----------------------------------------------------------------------------
/**
*/
class nLWShaderPanelFloatParam : public nLWShaderPanelParam
{
public:
    nLWShaderPanelFloatParam();
    void Load(const TiXmlElement* paramElem);
    void CopyParamTo(nLWShaderExportSettings*);
    void CopyParamFrom(nLWShaderExportSettings*);
    wxWindow* CreateControl(wxWindow* parent);
    nLWShaderPanelParam* Clone();

private:
    float valueLowerBound;
    float valueUpperBound;
    float defValue;
    wxFloatSliderCtrl* ctrl;
};

//----------------------------------------------------------------------------
/**
*/
class nLWShaderPanelBoolParam : public nLWShaderPanelParam
{
public:
    nLWShaderPanelBoolParam();
    bool NeedsLabelControl() const;
    void Load(const TiXmlElement* paramElem);
    void CopyParamTo(nLWShaderExportSettings*);
    void CopyParamFrom(nLWShaderExportSettings*);
    wxWindow* CreateControl(wxWindow*);
    nLWShaderPanelParam* Clone();

private:
    bool defValue;
    wxCheckBox* ctrl;
};

//----------------------------------------------------------------------------
/**
*/
class nLWShaderPanelColorParam : public nLWShaderPanelParam
{
public:
    nLWShaderPanelColorParam();
    void Load(const TiXmlElement* paramElem);
    void CopyParamTo(nLWShaderExportSettings*);
    void CopyParamFrom(nLWShaderExportSettings*);
    wxWindow* CreateControl(wxWindow* parent);
    nLWShaderPanelParam* Clone();

private:
    vector4 defValue;
    wxColorAlphaCtrl* ctrl;
};

//----------------------------------------------------------------------------
/**
*/
class nLWShaderPanelTextureParam : public nLWShaderPanelParam
{
public:
    nLWShaderPanelTextureParam();
    void Load(const TiXmlElement* paramElem);
    void CopyParamTo(nLWShaderExportSettings*);
    void CopyParamFrom(nLWShaderExportSettings*);
    wxWindow* CreateControl(wxWindow*);
    nLWShaderPanelParam* Clone();

private:
    //void OnValueChanged(wxCommandEvent&);

    nString defValue;
    //nLWFilePathControl* ctrl;
    //wxTextCtrl* ctrl;
    wxTextureFileCtrl* ctrl;
};

//----------------------------------------------------------------------------
#endif // N_LW_SHADER_PANEL_PARAM_H
