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

//----------------------------------------------------------------------------
/**
*/
nLWShaderPanelParam::nLWShaderPanelParam() :
    visible(false)
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
*/
nLWShaderPanelPlaceHolderParam::nLWShaderPanelPlaceHolderParam()
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
nLWShaderPanelEnumParam::nLWShaderPanelEnumParam() :
    firstMemberId(0),
    defMemberId(0),
    ctrl(0)
{

}

//----------------------------------------------------------------------------
/**
*/
nLWShaderPanelEnumParam::~nLWShaderPanelEnumParam()
{

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
    for (int memberIdx = 0; memberIdx < this->enumMembers.Size(); memberIdx++)
    {
        this->ctrl->AppendString(this->enumMembers[memberIdx].Get());
    }
    // select the default item
    this->ctrl->SetSelection(this->defMemberId - this->firstMemberId);
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
nLWShaderPanelIntParam::nLWShaderPanelIntParam() : 
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
    this->ctrl->SetValue(this->defValue);
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
nLWShaderPanelFloatParam::nLWShaderPanelFloatParam() :
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
    this->ctrl->SetValue(this->defValue);
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
nLWShaderPanelBoolParam::nLWShaderPanelBoolParam() :
    defValue(false),
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
    this->ctrl->SetValue(this->defValue);
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
nLWShaderPanelColorParam::nLWShaderPanelColorParam() :
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
nLWShaderPanelTextureParam::nLWShaderPanelTextureParam() :
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
    //this->ctrl = panel->NewFilePathBox(50, nLWFilePathControl::LOAD);
    //this->ctrl = new wxTextCtrl(parent, wxID_ANY);
    this->ctrl = new wxTextureFileCtrl(parent, wxID_ANY);
    if (this->ctrl)
    {
        this->ctrl->SetValue(this->defValue);
        //this->ctrl->SetEventHandler(nLWShaderPanelTextureParam::OnValueChanged);
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
/**
*/
/*
void
nLWShaderPanelTextureParam::OnValueChanged(wxCommandEvent& WXUNUSED(event))
{
    nString origPath(this->ctrl->GetValue());
    if (origPath.IsEmpty())
    {
        return;
    }
    origPath.ConvertBackslashes();

    nString absPath(origPath);    
    absPath.StripTrailingSlash();
    absPath.ToLower();

    nLWGlobals::DirInfoFunc dirInfo;

    nString lwImagesDir(dirInfo.GetImagesDir());
    lwImagesDir.ToLower();
    lwImagesDir.ConvertBackslashes();
    lwImagesDir.StripTrailingSlash();
    lwImagesDir += "/";

    nString textureExportDir(dirInfo.GetContentDir());
    textureExportDir.ToLower();
    textureExportDir.ConvertBackslashes();
    textureExportDir.StripTrailingSlash();
    // get the base Nebula 2 dir
    textureExportDir = textureExportDir.ExtractToLastSlash();
    textureExportDir.StripTrailingSlash();
    // this will be path_to_nebula2/export/textures/
    textureExportDir += "/export/textures/";

    if ((absPath.Length() > lwImagesDir.Length()) &&
        strncmp(absPath.Get(), lwImagesDir.Get(), lwImagesDir.Length()) == 0)
    {
        // set the path relative to the Lightwave images dir
        ctrl->SetValue(&(origPath.Get()[lwImagesDir.Length()]));
        return;
    }

    nString texturesAssign("textures:");

    if ((absPath.Length() > textureExportDir.Length()) &&
        strncmp(absPath.Get(), textureExportDir.Get(), textureExportDir.Length()) == 0)
    {
        // set the path relative to the dir where textures will be exported
        nString resolvedPath(texturesAssign);
        resolvedPath += &(origPath.Get()[textureExportDir.Length()]);
        ctrl->SetValue(resolvedPath.Get());
        return;
    }
    
    if ((absPath.Length() > 3) && 
        strncmp(&(absPath.Get()[1]), ":/", 2) != 0)
    {
        // assume that if the path is relative it's relative to the images
        // dir and therefore valid, this doesn't work with UNC paths... 
        // but who cares?
        // TODO: check the file actually exists
        return;
    }

    if ((absPath.Length() > texturesAssign.Length()) && 
        strncmp(absPath.Get(), texturesAssign.Get(), texturesAssign.Length()) == 0)
    {
        // assume that if it starts with "textures:" it's a valid path,
        // this will be correct in most cases
        // TODO: check the file actually exists
        return;
    }
    
    wxMessageBox("The texture must be in the proj:work/images " \
                 "directory or the proj:export/textures " \
                 "directory, please pick another texture.", 
                 "Error");
}
*/

//----------------------------------------------------------------------------
// EOF
//----------------------------------------------------------------------------
