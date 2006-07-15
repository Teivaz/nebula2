#ifndef N_LW_SHADER_EXPORT_SETTINGS_H
#define N_LW_SHADER_EXPORT_SETTINGS_H
//----------------------------------------------------------------------------
#include "gfx2/nshaderparams.h"
#include "util/nstring.h"
#include "scene/nmaterialnode.h"

//----------------------------------------------------------------------------
/**
    @class nLWShaderExportSettings
    @brief Stores all shader parameters for a particular shader.

    This class can copy the stored shader parameters directly into an 
    nMaterialNode.
*/
class nLWShaderExportSettings
{
public:
    nLWShaderExportSettings();
    ~nLWShaderExportSettings();

    /// returns true if the settings have been set, false otherwise
    bool Valid() const;

    void SetShaderName(const nString&);
    const nString& GetShaderName();
    void SetShaderFile(const nString&);
    const nString& GetShaderFile();

    void SetArg(nShaderState::Param, const nShaderArg&);
    const nShaderArg& GetArg(nShaderState::Param) const;
    int GetNumArgs() const;
    const nShaderArg& GetArgAt(int index) const;
    nShaderState::Param GetParamAt(int index) const;

    void SetTexture(nShaderState::Param param, const nString& textureName);
    const nString& GetTexture(nShaderState::Param param) const;
    int GetNumTextures() const;
    const nString& GetTextureAt(int index) const;
    nShaderState::Param GetTextureParamAt(int index) const;

    void CopyTo(nMaterialNode*);
    void CopyNonTextureParamsTo(nMaterialNode*);

    void Clear();

private:
    nString shaderName;
    nString shaderFile;
    nShaderParams shaderParams; // non-texture params

    class TextureParam
    {
    public:
        TextureParam();
        TextureParam(nShaderState::Param, const nString&);

        nShaderState::Param param;
        nString textureName;
    };
    nArray<TextureParam> textureParams;
};

//----------------------------------------------------------------------------
/**
*/
inline
nLWShaderExportSettings::TextureParam::TextureParam():
    param(nShaderState::InvalidParameter)
{
    // empty
}

//----------------------------------------------------------------------------
/**
*/
inline
nLWShaderExportSettings::TextureParam::TextureParam(nShaderState::Param p, 
                                                    const nString& t) :
    param(p),
    textureName(t)
{
    // empty
}

//----------------------------------------------------------------------------
/**
*/
inline
nLWShaderExportSettings::nLWShaderExportSettings() :
    shaderName("None")
{
    // empty
}

//----------------------------------------------------------------------------
/**
*/
inline
nLWShaderExportSettings::~nLWShaderExportSettings()
{
    this->Clear();
}

//----------------------------------------------------------------------------
/**
*/
inline
void 
nLWShaderExportSettings::Clear()
{
    this->shaderName.Clear();
    this->shaderFile.Clear();
    this->shaderParams.Clear();
    this->textureParams.Clear();
}

//----------------------------------------------------------------------------
/**
*/
inline
bool 
nLWShaderExportSettings::Valid() const
{
    return ("None" != this->shaderName);
}

//----------------------------------------------------------------------------
/**
*/
inline
void 
nLWShaderExportSettings::SetShaderName(const nString& shaderName)
{
    this->shaderName = shaderName;
}

//----------------------------------------------------------------------------
/**
*/
inline
const nString& 
nLWShaderExportSettings::GetShaderName()
{
    return this->shaderName;
}

//----------------------------------------------------------------------------
/**
*/
inline
void 
nLWShaderExportSettings::SetShaderFile(const nString& fileName)
{
    this->shaderFile = fileName;
}

//----------------------------------------------------------------------------
/**
*/
inline
const nString& 
nLWShaderExportSettings::GetShaderFile()
{
    return this->shaderFile;
}

//----------------------------------------------------------------------------
/**
*/
inline
void 
nLWShaderExportSettings::SetArg(nShaderState::Param p, const nShaderArg& a)
{
    this->shaderParams.SetArg(p, a);    
}

//----------------------------------------------------------------------------
/**
*/
inline
const nShaderArg& 
nLWShaderExportSettings::GetArg(nShaderState::Param p) const
{
    return this->shaderParams.GetArg(p);
}

//----------------------------------------------------------------------------
/**
*/
inline
int 
nLWShaderExportSettings::GetNumArgs() const
{
    return this->shaderParams.GetNumValidParams();
}

//----------------------------------------------------------------------------
/**
*/
inline
const nShaderArg& 
nLWShaderExportSettings::GetArgAt(int index) const
{
    return this->shaderParams.GetArgByIndex(index);
}

//----------------------------------------------------------------------------
/**
*/
inline
nShaderState::Param 
nLWShaderExportSettings::GetParamAt(int index) const
{
    return this->shaderParams.GetParamByIndex(index);
}

//----------------------------------------------------------------------------
/**
*/
inline
void 
nLWShaderExportSettings::SetTexture(nShaderState::Param param, 
                                    const nString& textureName)
{
    // if the param has been set previously just update it
    for (int i = 0; i < this->textureParams.Size(); i++)
    {
        if (param == this->textureParams[i].param)
        {
            this->textureParams[i].textureName = textureName;
            return;
        }
    }
    // param hasn't been set previously so need to add it in
    this->textureParams.PushBack(TextureParam(param, textureName));
}

//----------------------------------------------------------------------------
/**
*/
inline
const nString&
nLWShaderExportSettings::GetTexture(nShaderState::Param param) const
{
    for (int i = 0; i < this->textureParams.Size(); i++)
    {
        if (param == this->textureParams[i].param)
        {
            return this->textureParams[i].textureName;
        }
    }

    // return an empty string if we didn't find the param
    static nString emptyStr;
    return emptyStr;
}

//----------------------------------------------------------------------------
/**
*/
inline
int 
nLWShaderExportSettings::GetNumTextures() const
{
    return this->textureParams.Size();
}

//----------------------------------------------------------------------------
/**
*/
inline
const nString& 
nLWShaderExportSettings::GetTextureAt(int index) const
{
    return this->textureParams[index].textureName;
}

//----------------------------------------------------------------------------
/**
*/
inline
nShaderState::Param 
nLWShaderExportSettings::GetTextureParamAt(int index) const
{
    return this->textureParams[index].param;
}

//----------------------------------------------------------------------------
/**
*/
inline
void 
nLWShaderExportSettings::CopyNonTextureParamsTo(nMaterialNode* node)
{
    node->SetShader(this->shaderFile.Get());
    node->SetMayaShaderName(this->shaderName);
    for (int i = 0; i < this->shaderParams.GetNumValidParams(); i++)
    {
        nShaderState::Param param = this->shaderParams.GetParamByIndex(i);
        const nShaderArg& value = this->shaderParams.GetArgByIndex(i);
        switch (value.GetType())
        {
            case nShaderState::Bool:
                node->SetBool(param, value.GetBool());
                break;

            case nShaderState::Int:
                node->SetInt(param, value.GetInt());
                break;

            case nShaderState::Float:
                node->SetFloat(param, value.GetFloat());
                break;

            case nShaderState::Float4:
                node->SetVector(param, value.GetVector4());
                break;
        }
    }
}

//----------------------------------------------------------------------------
/**
*/
inline
void 
nLWShaderExportSettings::CopyTo(nMaterialNode* node)
{
    this->CopyNonTextureParamsTo(node);

    for (int i = 0; i < this->textureParams.Size(); i++)
    {
        node->SetTexture(this->textureParams[i].param, 
                         this->textureParams[i].textureName.Get());
    }
}

//----------------------------------------------------------------------------
#endif // N_LW_SHADER_EXPORT_SETTINGS_H
