#ifndef N_LW_SHADER_PANEL_TEMPLATE_H
#define N_LW_SHADER_PANEL_TEMPLATE_H
//----------------------------------------------------------------------------
#include "util/nstring.h"
#include "util/narray.h"

class TiXmlElement;
class nLWShaderExportSettings;
class nLWShaderPanelParam;

//----------------------------------------------------------------------------
/**
    @class nLWShaderPanelTemplate
    @brief A template for a shader dialog.

    Each Nebula 2 shader has a number of parameters that the user can tweak,
    the dialog for editing a Nebula 2 shader in Lightwave is created from a 
    template that is generated based on the shader description in shaders.xml.
*/
class nLWShaderPanelTemplate
{
public:
    nLWShaderPanelTemplate(const nString& shaderName, const nString& shaderFile);
    virtual ~nLWShaderPanelTemplate();

    const nString& GetShaderName() const;
    const nString& GetShaderFile() const;
    bool LoadLayout(TiXmlElement* shaderElem);
    const nArray<nLWShaderPanelParam*>& GetShaderParamArray() const;

private:
    void AddShaderParam(const TiXmlElement* paramElem);

    nString shaderName;
    nString shaderFile;
    nArray<nLWShaderPanelParam*> shaderParamArray;
};

//----------------------------------------------------------------------------
/**
*/
inline
const nString&
nLWShaderPanelTemplate::GetShaderName() const
{
    return this->shaderName;
}

//----------------------------------------------------------------------------
/**
*/
inline
const nString&
nLWShaderPanelTemplate::GetShaderFile() const
{
    return this->shaderFile;
}

//----------------------------------------------------------------------------
/**
*/
inline
const nArray<nLWShaderPanelParam*>& 
nLWShaderPanelTemplate::GetShaderParamArray() const
{
    return this->shaderParamArray;
}

//----------------------------------------------------------------------------
#endif // N_LW_SHADER_PANEL_TEMPLATE_H
