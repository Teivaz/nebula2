#ifndef N_LW_SHADER_PANEL_FACTORY_H
#define N_LW_SHADER_PANEL_FACTORY_H
//----------------------------------------------------------------------------
extern "C" {
#include <lwsdk/lwsurf.h>
}

#include "util/nstring.h"
#include "util/nhashlist.h"

class nLWShaderExportSettings;
class nLWShaderPanelTemplate;

//----------------------------------------------------------------------------
/**
*/
class nLWShaderPanelFactory
{
public:
    static nLWShaderPanelFactory* Instance();
    static void FreeInstance();
    ~nLWShaderPanelFactory();

    /// set the path to the xml file that contains the shader descriptions
    void SetShadersFile(const nString&);

    /// load shader descriptions from disk
    void Load();

    /// display the shader panel for the given surface
    void DisplayPanel(LWSurfaceID);

    void SetNextShaderPanelToDisplay(const nString& shaderName);

private:
    nLWShaderPanelFactory();
    nLWShaderPanelTemplate* GetNextShaderPanelToDisplay() const;
    void DisplayNextPanel(nLWShaderExportSettings*);

    static nLWShaderPanelFactory* singleton;
    
    nString shadersFileName;
    nHashList panelTemplateList;
    nString nextShaderPanelName;
    nArray<nString> shaderNames;
};

//----------------------------------------------------------------------------
#endif // N_LW_SHADER_PANEL_FACTORY_H
