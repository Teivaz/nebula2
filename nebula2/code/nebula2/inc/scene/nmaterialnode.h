#ifndef N_MATERIALNODE_H
#define N_MATERIALNODE_H
//------------------------------------------------------------------------------
/**
    @class nMaterialNode
    @ingroup SceneNodes

    @brief A material node defines a shader resource and associated shader
    variables. A shader resource is an external file (usually a text file)
    which defines a surface shader.
    
    Material nodes themselves cannot render anything useful, they can just
    adjust render states as preparation of an actual rendering process.
    Thus, subclasses should be derived which implement some sort of
    shape rendering.
    
    See also @ref N2ScriptInterface_nmaterialnode

    (C) 2002 RadonLabs GmbH
*/
#include "scene/nabstractshadernode.h"
#include "kernel/nref.h"
#include "kernel/nautoref.h"

class nShader2;
class nGfxServer2;

//------------------------------------------------------------------------------
class nMaterialNode : public nAbstractShaderNode
{
public:
    /// constructor
    nMaterialNode();
    /// destructor
    virtual ~nMaterialNode();
    /// object persistency
    virtual bool SaveCmds(nPersistServer *ps);
    /// load resources
    virtual bool LoadResources();
    /// unload resources
    virtual void UnloadResources();

    /// indicate to scene graph that we provide a surface shader
    virtual bool HasShader(nFourCC fourcc) const;
    /// perform pre-instancing rending of shader
    virtual bool ApplyShader(nFourCC fourcc, nSceneServer* sceneServer);
    /// perform per-instance-rendering of shader
    virtual bool RenderShader(nFourCC fourcc, nSceneServer* sceneServer, nRenderContext* renderContext);

    /// set shader resource name
    void SetShader(nFourCC fourcc, const char* name);
    /// get shader resource name
    const char* GetShader(nFourCC fourcc) const;
    /// get number of shaders
    int GetNumShaders() const;
    /// get shader at index
    void GetShaderAt(int index, nFourCC& fourcc, const char*& name) const;
    /// get pointer to shader object
    nShader2* GetShaderObject(nFourCC fourcc);

    /// get fourcc code from string
    static nFourCC StringToFourCC(const char* str);
    /// get string from fourcc code
    static const char* FourCCToString(nFourCC fourcc, char* buf, int bufSize);

protected:
    /// recursively append instance parameters to provided instance stream declaration
    virtual void UpdateInstStreamDecl(nInstanceStream::Declaration& decl);

private:
    class ShaderEntry
    {
    public:
        /// default constructor
        ShaderEntry();
        /// constructor
        ShaderEntry(nFourCC shaderFourCC, const char* shaderName);
        /// set fourcc code
        void SetFourCC(nFourCC shaderFourCC);
        /// get shader fourcc code
        nFourCC GetFourCC() const;
        /// set shader name
        void SetName(const char* shaderName);
        /// get shader name
        const char* GetName() const;
        /// set shader
        void SetShader(nShader2* shd);
        /// get shader pointer
        nShader2* GetShader() const;
        /// is shader ref valid?
        bool IsShaderValid() const;
        /// invalidate entry
        void Invalidate();


        nFourCC fourcc;
        nString name;
        nRef<nShader2> refShader;
    };

    /// load the shader resource 
    bool LoadShaders();
    /// unload the shader resource
    void UnloadShaders();
    /// find a shader entry by its fourcc code
    ShaderEntry* FindShaderEntry(nFourCC fourcc) const;
    /// checks if shader uses texture passed in param
    virtual bool IsTextureUsed(nShaderState::Param param);

    nArray<ShaderEntry> shaderArray;
};

//------------------------------------------------------------------------------
/**
*/
inline
int
nMaterialNode::GetNumShaders() const
{
    return this->shaderArray.Size();
}

//------------------------------------------------------------------------------
/**
    Return shader attributes at given index.

    @param  index   [in] index
    @param  fourcc  [out] the fourcc code associated with the shader
    @param  name    [out] the resource name of the shader
*/
inline
void
nMaterialNode::GetShaderAt(int index, nFourCC& fourcc, const char*& name) const
{
    fourcc = this->shaderArray[index].GetFourCC();
    name   = this->shaderArray[index].GetName();
}

//------------------------------------------------------------------------------
/**
    Convert a string to a fourcc code.
*/
inline
nFourCC
nMaterialNode::StringToFourCC(const char* str)
{
    n_assert(str);
    char chr[4] = { 0 };
    int i = 0;
    while (str[i] && (i < 4))
    {
        chr[i] = str[i];
        i++;
    }
    return MAKE_FOURCC(chr[0], chr[1], chr[2], chr[3]);
}

//------------------------------------------------------------------------------
/**
    Convert a fourcc code to a string.
*/
inline
const char*
nMaterialNode::FourCCToString(nFourCC fourcc, char* buf, int bufSize)
{
    n_assert(bufSize >= 5);
    buf[0] = (fourcc)     & 0xff;
    buf[1] = (fourcc>>8)  & 0xff;
    buf[2] = (fourcc>>16) & 0xff;
    buf[3] = (fourcc>>24) & 0xff;
    buf[4] = 0;
    return buf;
}

//------------------------------------------------------------------------------
/**
*/
inline
nShader2*
nMaterialNode::GetShaderObject(nFourCC fourcc)
{
    if (!this->AreResourcesValid())
    {
        this->LoadResources();
    }
    ShaderEntry* shaderEntry = this->FindShaderEntry(fourcc);
    if (shaderEntry)
    {
        return shaderEntry->GetShader();
    }
    return 0;
}

//------------------------------------------------------------------------------
/**
*/
inline
nMaterialNode::ShaderEntry::ShaderEntry() :
    fourcc(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
nMaterialNode::ShaderEntry::ShaderEntry(nFourCC shaderFourCC, const char* shaderName) :
    fourcc(shaderFourCC),
    name(shaderName)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nMaterialNode::ShaderEntry::SetFourCC(nFourCC shaderFourCC)
{
    this->fourcc = shaderFourCC;
}

//------------------------------------------------------------------------------
/**
*/
inline
nFourCC
nMaterialNode::ShaderEntry::GetFourCC() const
{
    return this->fourcc;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nMaterialNode::ShaderEntry::SetName(const char* shaderName)
{
    n_assert(shaderName);
    this->name = shaderName;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nMaterialNode::ShaderEntry::GetName() const
{
    return this->name.IsEmpty() ? 0 : this->name.Get();
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nMaterialNode::ShaderEntry::IsShaderValid() const
{
    return this->refShader.isvalid();
}

//------------------------------------------------------------------------------
/**
*/
inline
nShader2*
nMaterialNode::ShaderEntry::GetShader() const
{
    return this->refShader.get();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nMaterialNode::ShaderEntry::Invalidate()
{
    this->refShader.invalidate();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nMaterialNode::ShaderEntry::SetShader(nShader2* shd)
{
    this->refShader = shd;
}

//------------------------------------------------------------------------------
#endif

