//----------------------------------------------------------------------------
// (c) 2006    Vadim Macagon
//----------------------------------------------------------------------------
#include "lwexporter/nlwshaderpanelparam.h"
#include "lwexporter/nlwshaderexportsettings.h"
#include "tinyxml/tinyxml.h"
#include "lwwxui/wxcoloralphactrl.h"
#include "lwwxui/wxfloatsliderctrl.h"
#include "lwwxui/wxintsliderctrl.h"
#include "lwwxui/wxtexturefilectrl.h"
#include "lwexporter/nlwviewerremote.h"
#include "lwwxui/wxcustomevents.h"

//----------------------------------------------------------------------------
/**
*/
nLWShaderPanelParam::nLWShaderPanelParam(const nString& shaderName) :
    visible(false),
    shaderName(shaderName)
{
    // empty
}

//----------------------------------------------------------------------------
/**
*/
nLWShaderPanelParam::nLWShaderPanelParam(const nLWShaderPanelParam& other) :
    label(other.label),
    shaderParam(other.shaderParam),
    visible(other.visible),
    shaderName(other.shaderName)
{
    // empty
}

//----------------------------------------------------------------------------
/**
*/
nLWShaderPanelParam::~nLWShaderPanelParam()
{
    // empty
}

//----------------------------------------------------------------------------
/**
*/
const nString& 
nLWShaderPanelParam::GetLabel() const
{
    return this->label;
}

//----------------------------------------------------------------------------
/**
*/
bool 
nLWShaderPanelParam::Visible() const
{
    return this->visible;
}

//----------------------------------------------------------------------------
/**
*/
bool 
nLWShaderPanelParam::NeedsLabelControl() const
{
    return true;
}

//----------------------------------------------------------------------------
/**
*/
void 
nLWShaderPanelParam::Load(const TiXmlElement* paramElem)
{
    nString paramName = paramElem->Attribute("name");
    this->shaderParam = nShaderState::StringToParam(paramName.Get());
    n_assert(this->shaderParam != nShaderState::InvalidParameter);

    this->label = paramElem->Attribute("label");
    
    int needGUI = 0;
    paramElem->Attribute("gui", &needGUI);
    if (needGUI)
    {
        this->visible = true;
    }
    else
    {
        this->visible = false;
    }
}

//----------------------------------------------------------------------------
/**
    Override in sub-classes if the shader parameter value needs to be
    sent to nViewer.
*/
void 
nLWShaderPanelParam::SendParamToViewer()
{
    // empty
}

//----------------------------------------------------------------------------
/**
*/
void 
nLWShaderPanelParam::OnChange(wxCommandEvent& WXUNUSED(event))
{
    this->SendParamToViewer();
}

//----------------------------------------------------------------------------
/**
*/
nLWShaderPanelPlaceHolderParam::nLWShaderPanelPlaceHolderParam(const nString& shaderName) :
    nLWShaderPanelParam(shaderName)
{
    // empty
}

//----------------------------------------------------------------------------
/**
*/
nLWShaderPanelPlaceHolderParam::~nLWShaderPanelPlaceHolderParam()
{
    // empty
}

//----------------------------------------------------------------------------
/**
*/
void 
nLWShaderPanelPlaceHolderParam::Load(const TiXmlElement* paramElem)
{
    nLWShaderPanelParam::Load(paramElem);

    this->paramType = paramElem->Attribute("type");
}

//----------------------------------------------------------------------------
/**
*/
void 
nLWShaderPanelPlaceHolderParam::CopyParamTo(nLWShaderExportSettings*)
{
    // empty
}

//----------------------------------------------------------------------------
/**
*/
void 
nLWShaderPanelPlaceHolderParam::CopyParamFrom(nLWShaderExportSettings*)
{
    // empty
}

//----------------------------------------------------------------------------
/**
*/
wxWindow* 
nLWShaderPanelPlaceHolderParam::CreateControl(wxWindow* parent)
{
    return new wxStaticText(parent, wxID_ANY, this->paramType.Get());
}

//----------------------------------------------------------------------------
/**
*/
nLWShaderPanelParam* 
nLWShaderPanelPlaceHolderParam::Clone()
{
    return n_new(nLWShaderPanelPlaceHolderParam(*this));
}

//----------------------------------------------------------------------------
/**
*/
nLWShaderPanelEnumParam::nLWShaderPanelEnumParam(const nString& shaderName) :
    nLWShaderPanelParam(shaderName),
    firstMemberId(0),
    defMemberId(0),
    ctrl(0)
{
    // empty
}

//----------------------------------------------------------------------------
/**
*/
nLWShaderPanelEnumParam::nLWShaderPanelEnumParam(const nLWShaderPanelEnumParam& other) :
    nLWShaderPanelParam(other),
    enumMembers(other.enumMembers),
    firstMemberId(other.firstMemberId),
    defMemberId(other.defMemberId),
    ctrl(0)
{
    // empty
}

//----------------------------------------------------------------------------
/**
*/
nLWShaderPanelEnumParam::~nLWShaderPanelEnumParam()
{
    // empty
}

//----------------------------------------------------------------------------
/**
*/
void 
nLWShaderPanelEnumParam::Load(const TiXmlElement* paramElem)
{
    nLWShaderPanelParam::Load(paramElem);

    nArray<nString> tokens;
    nString enumString = paramElem->Attribute("enum");
    enumString.Tokenize(":", tokens);

    nString member;
    for (int i = 0; i < tokens.Size(); i++)
    {
        member = tokens[i].GetFirstToken("=");
        if (0 == i)
        {
            nString memberId = tokens[i].GetNextToken("=");
            this->firstMemberId = memberId.AsInt();
        }
        this->enumMembers.Append(member);
    }

    paramElem->Attribute("def", &this->defMemberId);
}

//----------------------------------------------------------------------------
/**
*/
void 
nLWShaderPanelEnumParam::CopyParamTo(nLWShaderExportSettings* settings)
{
    if (ctrl)
    {
        settings->SetArg(this->shaderParam, 
                         this->firstMemberId + ctrl->GetSelection());
    }
    else
    {
        settings->SetArg(this->shaderParam, this->defMemberId);
    }
}

//----------------------------------------------------------------------------
/**
*/
void 
nLWShaderPanelEnumParam::CopyParamFrom(nLWShaderExportSettings* settings)
{
    if (ctrl)
    {
        int memberId = settings->GetArg(this->shaderParam).GetInt();
        ctrl->SetSelection(memberId - this->firstMemberId);
    }
}

//----------------------------------------------------------------------------
/**
*/
wxWindow* 
nLWShaderPanelEnumParam::CreateControl(wxWindow* parent)
{
    this->ctrl = new wxChoice(parent, wxID_ANY);
    if (this->ctrl)
    {
        for (int memberIdx = 0; memberIdx < this->enumMembers.Size(); memberIdx++)
        {
            this->ctrl->AppendString(this->enumMembers[memberIdx].Get());
        }
        // select the default item
        this->ctrl->SetSelection(this->defMemberId - this->firstMemberId);
        // hook up the change event handler so we can send changes to the 
        // preview window
        this->ctrl->Connect(wxEVT_CUSTOM_CHANGE, 
                            wxCommandEventHandler(nLWShaderPanelEnumParam::OnChange),
                            0, this);
    }
    return this->ctrl;
}

//----------------------------------------------------------------------------
/**
*/
nLWShaderPanelParam* 
nLWShaderPanelEnumParam::Clone()
{
    return n_new(nLWShaderPanelEnumParam(*this));
}

//----------------------------------------------------------------------------
/**
*/
void 
nLWShaderPanelEnumParam::SendParamToViewer()
{
    if (this->ctrl && nLWViewerRemote::Instance()->IsOpen())
    {
        int paramValue = this->firstMemberId + this->ctrl->GetSelection();
        nLWViewerRemote::Instance()->ChangeShaderParam(this->shaderName, 
                                                       "common", 
                                                       this->shaderParam, 
                                                       paramValue);
    }
}

//----------------------------------------------------------------------------
/**
*/
nLWShaderPanelIntParam::nLWShaderPanelIntParam(const nString& shaderName) :
    nLWShaderPanelParam(shaderName),
    valueLowerBound(0),
    valueUpperBound(0),
    defValue(0),
    ctrl(0)
{
    // empty
}

//----------------------------------------------------------------------------
/**
*/
nLWShaderPanelIntParam::nLWShaderPanelIntParam(const nLWShaderPanelIntParam& other) :
    nLWShaderPanelParam(other),
    valueLowerBound(other.valueLowerBound),
    valueUpperBound(other.valueUpperBound),
    defValue(other.defValue),
    ctrl(0)
{
    
}

//----------------------------------------------------------------------------
/**
*/
void
nLWShaderPanelIntParam::Load(const TiXmlElement* paramElem)
{
    nLWShaderPanelParam::Load(paramElem);

    paramElem->Attribute("min", &this->valueLowerBound);
    paramElem->Attribute("max", &this->valueUpperBound);
    paramElem->Attribute("def", &this->defValue);
}

//----------------------------------------------------------------------------
/**
*/
void 
nLWShaderPanelIntParam::CopyParamTo(nLWShaderExportSettings* settings)
{
    if (this->ctrl)
    {
        settings->SetArg(this->shaderParam, this->ctrl->GetValue());
    }
    else
    {
        settings->SetArg(this->shaderParam, this->defValue);
    }
}

//----------------------------------------------------------------------------
/**
*/
void 
nLWShaderPanelIntParam::CopyParamFrom(nLWShaderExportSettings* settings)
{
    if (this->ctrl)
        ctrl->SetValue(settings->GetArg(this->shaderParam).GetInt());
}

//----------------------------------------------------------------------------
/**
*/
wxWindow* 
nLWShaderPanelIntParam::CreateControl(wxWindow* parent)
{
    this->ctrl = new wxIntSliderCtrl(parent, wxID_ANY, 
                                     this->valueLowerBound, 
                                     this->valueUpperBound);
    if (this->ctrl)
    {
        this->ctrl->SetValue(this->defValue);
        // hook up the change event handler so we can send changes to the 
        // preview window
        this->ctrl->Connect(wxEVT_CUSTOM_CHANGE, 
                            wxCommandEventHandler(nLWShaderPanelIntParam::OnChange),
                            0, this);
    }
    return this->ctrl;
}

//----------------------------------------------------------------------------
/**
*/
nLWShaderPanelParam* 
nLWShaderPanelIntParam::Clone()
{
    return n_new(nLWShaderPanelIntParam(*this));
}

//----------------------------------------------------------------------------
/**
*/
void 
nLWShaderPanelIntParam::SendParamToViewer()
{
    if (this->ctrl && nLWViewerRemote::Instance()->IsOpen())
    {
        nLWViewerRemote::Instance()->ChangeShaderParam(this->shaderName, 
                                                       "common", 
                                                       this->shaderParam, 
                                                       this->ctrl->GetValue());
    }
}

//----------------------------------------------------------------------------
/**
*/
nLWShaderPanelFloatParam::nLWShaderPanelFloatParam(const nString& shaderName) :
    nLWShaderPanelParam(shaderName),
    valueLowerBound(0.0f),
    valueUpperBound(0.0f),
    defValue(0.0f),
    ctrl(0)
{
    // empty
}

//----------------------------------------------------------------------------
/**
*/
nLWShaderPanelFloatParam::nLWShaderPanelFloatParam(const nLWShaderPanelFloatParam& other) :
    nLWShaderPanelParam(other),
    valueLowerBound(other.valueLowerBound),
    valueUpperBound(other.valueUpperBound),
    defValue(other.defValue),
    ctrl(0)
{
    // empty
}

//----------------------------------------------------------------------------
/**
*/
void 
nLWShaderPanelFloatParam::Load(const TiXmlElement* paramElem)
{
    nLWShaderPanelParam::Load(paramElem);

    double lowerBound = 0.0, upperBound = 0.0, defValue = 0.0;
    paramElem->Attribute("min", &lowerBound);
    paramElem->Attribute("max", &upperBound);
    paramElem->Attribute("def", &defValue);
    this->valueLowerBound = (float)lowerBound;
    this->valueUpperBound = (float)upperBound;
    this->defValue = (float)defValue;
}

//----------------------------------------------------------------------------
/**
*/
void 
nLWShaderPanelFloatParam::CopyParamTo(nLWShaderExportSettings* settings)
{
    if (this->ctrl)
    {
        settings->SetArg(this->shaderParam, this->ctrl->GetValue());
    }
    else
    {
        settings->SetArg(this->shaderParam, this->defValue);
    }
}

//----------------------------------------------------------------------------
/**
*/
void 
nLWShaderPanelFloatParam::CopyParamFrom(nLWShaderExportSettings* settings)
{
    if (this->ctrl)
        this->ctrl->SetValue(settings->GetArg(this->shaderParam).GetFloat());
}

//----------------------------------------------------------------------------
/**
*/
wxWindow* 
nLWShaderPanelFloatParam::CreateControl(wxWindow* parent)
{
    this->ctrl = new wxFloatSliderCtrl(parent, wxID_ANY, 
                                       this->valueLowerBound, 
                                       this->valueUpperBound);
    if (this->ctrl)
    {
        this->ctrl->SetValue(this->defValue);
        // hook up the change event handler so we can send changes to the 
        // preview window
        this->ctrl->Connect(wxEVT_CUSTOM_CHANGE, 
                            wxCommandEventHandler(nLWShaderPanelFloatParam::OnChange),
                            0, this);
    }
    return this->ctrl;
}

//----------------------------------------------------------------------------
/**
*/
nLWShaderPanelParam* 
nLWShaderPanelFloatParam::Clone()
{
    return n_new(nLWShaderPanelFloatParam(*this));
}

//----------------------------------------------------------------------------
/**
*/
void 
nLWShaderPanelFloatParam::SendParamToViewer()
{
    if (this->ctrl && nLWViewerRemote::Instance()->IsOpen())
    {
        nLWViewerRemote::Instance()->ChangeShaderParam(this->shaderName, 
                                                       "common", 
                                                       this->shaderParam, 
                                                       this->ctrl->GetValue());
    }
}

//----------------------------------------------------------------------------
/**
*/
nLWShaderPanelBoolParam::nLWShaderPanelBoolParam(const nString& shaderName) :
    nLWShaderPanelParam(shaderName),
    defValue(false),
    ctrl(0)
{
    // empty
}

//----------------------------------------------------------------------------
/**
*/
nLWShaderPanelBoolParam::nLWShaderPanelBoolParam(const nLWShaderPanelBoolParam& other) :
    nLWShaderPanelParam(other),
    defValue(other.defValue),
    ctrl(0)
{
    // empty
}

//----------------------------------------------------------------------------
/**
*/
bool 
nLWShaderPanelBoolParam::NeedsLabelControl() const
{
    return false;
}

//----------------------------------------------------------------------------
/**
*/
void 
nLWShaderPanelBoolParam::Load(const TiXmlElement* paramElem)
{
    nLWShaderPanelParam::Load(paramElem);

    int defState = 0;
    paramElem->Attribute("def", &defState);
    this->defValue = ((0 == defState) ? false : true);
}

//----------------------------------------------------------------------------
/**
*/
void 
nLWShaderPanelBoolParam::CopyParamTo(nLWShaderExportSettings* settings)
{
    if (ctrl)
    {
        settings->SetArg(this->shaderParam, ctrl->GetValue());
    }
    else
    {
        settings->SetArg(this->shaderParam, this->defValue);
    }
}

//----------------------------------------------------------------------------
/**
*/
void 
nLWShaderPanelBoolParam::CopyParamFrom(nLWShaderExportSettings* settings)
{
    if (ctrl)
    {
        ctrl->SetValue(settings->GetArg(this->shaderParam).GetBool());
    }
}

//----------------------------------------------------------------------------
/**
*/
wxWindow* 
nLWShaderPanelBoolParam::CreateControl(wxWindow* parent)
{
    this->ctrl = new wxCheckBox(parent, wxID_ANY, this->label.Get());
    if (this->ctrl)
    {
        this->ctrl->SetValue(this->defValue);
        // hook up the change event handler so we can send changes to the 
        // preview window
        this->ctrl->Connect(wxEVT_CUSTOM_CHANGE, 
                            wxCommandEventHandler(nLWShaderPanelBoolParam::OnChange),
                            0, this);
    }
    return this->ctrl;
}

//----------------------------------------------------------------------------
/**
*/
nLWShaderPanelParam* 
nLWShaderPanelBoolParam::Clone()
{
    return n_new(nLWShaderPanelBoolParam(*this));
}

//----------------------------------------------------------------------------
/**
*/
void 
nLWShaderPanelBoolParam::SendParamToViewer()
{
    if (this->ctrl && nLWViewerRemote::Instance()->IsOpen())
    {
        nLWViewerRemote::Instance()->ChangeShaderParam(this->shaderName, 
                                                       "common", 
                                                       this->shaderParam, 
                                                       this->ctrl->GetValue());
    }
}

//----------------------------------------------------------------------------
/**
*/
nLWShaderPanelColorParam::nLWShaderPanelColorParam(const nString& shaderName) :
    nLWShaderPanelParam(shaderName),
    ctrl(0)
{
    // empty
}

//----------------------------------------------------------------------------
/**
*/
nLWShaderPanelColorParam::nLWShaderPanelColorParam(const nLWShaderPanelColorParam& other) :
    nLWShaderPanelParam(other),
    defValue(other.defValue),
    ctrl(0)
{
    // empty
}

//----------------------------------------------------------------------------
/**
*/
void 
nLWShaderPanelColorParam::Load(const TiXmlElement* paramElem)
{
    nLWShaderPanelParam::Load(paramElem);

    nString defValueStr = paramElem->Attribute("def");
    this->defValue = defValueStr.AsVector4();
}

//----------------------------------------------------------------------------
/**
*/
void 
nLWShaderPanelColorParam::CopyParamTo(nLWShaderExportSettings* settings)
{
    if (ctrl)
    {
        int r = 0, g = 0, b = 0;
        float a = 0.0f;
        ctrl->GetValue(&r, &g, &b, &a);
        settings->SetArg(this->shaderParam, vector4(r, g, b, a));
    }
    else
    {
        settings->SetArg(this->shaderParam, this->defValue);
    }
}

//----------------------------------------------------------------------------
/**
*/
void 
nLWShaderPanelColorParam::CopyParamFrom(nLWShaderExportSettings* settings)
{
    if (ctrl)
    {
        const nShaderArg& arg = settings->GetArg(this->shaderParam);
        nFloat4 f4 = arg.GetFloat4();
        ctrl->SetValue(f4.x, f4.y, f4.z, f4.w);
    }
}

//----------------------------------------------------------------------------
/**
*/
wxWindow* 
nLWShaderPanelColorParam::CreateControl(wxWindow* parent)
{
    this->ctrl = new wxColorAlphaCtrl(parent, wxID_ANY);
    if (this->ctrl)
    {
        this->ctrl->SetValue(this->defValue.x, 
                             this->defValue.y, 
                             this->defValue.z, 
                             this->defValue.w);
        // hook up the change event handler so we can send changes to the 
        // preview window
        this->ctrl->Connect(wxEVT_CUSTOM_CHANGE, 
                            wxCommandEventHandler(nLWShaderPanelColorParam::OnChange),
                            0, this);
    }
    return this->ctrl;
}

//----------------------------------------------------------------------------
/**
*/
nLWShaderPanelParam* 
nLWShaderPanelColorParam::Clone()
{
    return n_new(nLWShaderPanelColorParam(*this));
}

//----------------------------------------------------------------------------
/**
*/
void 
nLWShaderPanelColorParam::SendParamToViewer()
{
    if (this->ctrl && nLWViewerRemote::Instance()->IsOpen())
    {
        int r, g, b;
        float a;
        this->ctrl->GetValue(&r, &g, &b, &a);
        nFloat4 f4;
        f4.x = r;
        f4.y = g;
        f4.z = b;
        f4.w = a;
        nLWViewerRemote::Instance()->ChangeShaderParam(this->shaderName, 
                                                       "common", 
                                                       this->shaderParam, 
                                                       f4);
    }
}

//----------------------------------------------------------------------------
/**
*/
nLWShaderPanelTextureParam::nLWShaderPanelTextureParam(const nString& shaderName) :
    nLWShaderPanelParam(shaderName),
    ctrl(0)
{
    // empty
}

//----------------------------------------------------------------------------
/**
*/
nLWShaderPanelTextureParam::nLWShaderPanelTextureParam(const nLWShaderPanelTextureParam& other) :
    nLWShaderPanelParam(other),
    defValue(other.defValue),
    ctrl(0)
{
    // empty
}

//----------------------------------------------------------------------------
/**
*/
void 
nLWShaderPanelTextureParam::Load(const TiXmlElement* paramElem)
{
    nLWShaderPanelParam::Load(paramElem);

    this->defValue = paramElem->Attribute("def");
}

//----------------------------------------------------------------------------
/**
*/
void 
nLWShaderPanelTextureParam::CopyParamTo(nLWShaderExportSettings* settings)
{
    if (ctrl)
    {
        settings->SetTexture(this->shaderParam, ctrl->GetValue());
    }
    else
    {
        settings->SetTexture(this->shaderParam, this->defValue);
    }
}

//----------------------------------------------------------------------------
/**
*/
void 
nLWShaderPanelTextureParam::CopyParamFrom(nLWShaderExportSettings* settings)
{
    if (ctrl)
        ctrl->SetValue(settings->GetTexture(this->shaderParam));
}

//----------------------------------------------------------------------------
/**
*/
wxWindow* 
nLWShaderPanelTextureParam::CreateControl(wxWindow* parent)
{
    this->ctrl = new wxTextureFileCtrl(parent, wxID_ANY);
    if (this->ctrl)
    {
        this->ctrl->SetValue(this->defValue);
    }
    return this->ctrl;
}

//----------------------------------------------------------------------------
/**
*/
nLWShaderPanelParam* 
nLWShaderPanelTextureParam::Clone()
{
    return n_new(nLWShaderPanelTextureParam(*this));
}

//----------------------------------------------------------------------------
// EOF
//----------------------------------------------------------------------------
