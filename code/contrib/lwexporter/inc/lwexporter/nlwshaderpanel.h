#ifndef N_LW_SHADER_PANEL_H
#define N_LW_SHADER_PANEL_H
//----------------------------------------------------------------------------
#include "util/nstring.h"
#include "util/narray.h"
#include "wx/wx.h"
#include "wx/gbsizer.h"
#include "lwexporter/nlwshaderpaneltemplate.h"

class TiXmlElement;
class nLWShaderExportSettings;
class nLWShaderPanelParam;

//----------------------------------------------------------------------------
/**
*/
class nLWShaderPanel : public wxDialog
{
public:
    nLWShaderPanel(wxWindow* parent, 
                   const nLWShaderPanelTemplate* panelTemplate,
                   const nArray<nString>& shaderNames);
    virtual ~nLWShaderPanel();

    void ResetShaderPopup();

    void SetShaderSettings(nLWShaderExportSettings*);

private:
    void OnShaderPopup(wxCommandEvent&);
    void OnOKBtn(wxCommandEvent&);
    void OnCancelBtn(wxCommandEvent&);

    nString shaderName;
    nString shaderFile;
    wxChoice* shaderPopup;
    nLWShaderExportSettings* curShaderSettings;
    nArray<nLWShaderPanelParam*> shaderParamArray;

    DECLARE_EVENT_TABLE()
};

//----------------------------------------------------------------------------
#endif // N_LW_SHADER_PANEL_H
