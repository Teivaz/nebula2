#ifndef N_MATERIALNODE_H
#define N_MATERIALNODE_H
//------------------------------------------------------------------------------
/**
    @class nMaterialNode
    @ingroup NebulaSceneSystemNodes

    A material node defines a shader resource and associated shader
    variables. A shader resource is an external file (usually a text file)
    which defines a surface shader.
    
    Material nodes themselves cannot render anything useful, they can just
    adjust render states as preparation of an actual rendering process.
    Thus, subclasses should be derived which implement some sort of
    shape rendering.
    
    See also @ref N2ScriptInterface_nmaterialnode

    (C) 2002 RadonLabs GmbH
*/
#ifndef N_ABSTRACTSHADERNODE_H
#include "scene/nabstractshadernode.h"
#endif

#ifndef N_REF_H
#include "kernel/nref.h"
#endif

#ifndef N_AUTOREF_H
#include "kernel/nautoref.h"
#endif

#undef N_DEFINES
#define N_DEFINES nMaterialNode
#include "kernel/ndefdllclass.h"

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
    /// return true if resources for this object are valid
    virtual bool AreResourcesValid() const;

    /// indicate to scene graph that we provide a surface shader
    virtual bool HasShader(uint fourcc) const;
    /// render shader
    virtual void RenderShader(uint fourcc, nSceneServer* sceneServer, nRenderContext* renderContext);

    /// set shader resource name
    void SetShader(uint fourcc, const char* name);
    /// get shader resource name
    const char* GetShader(uint fourcc) const;
    /// get number of shaders
    int GetNumShaders() const;
    /// get shader at index
    void GetShaderAt(int index, uint& fourcc, const char*& name) const;

    /// get fourcc code from string
    static uint StringToFourCC(const char* str);
    /// get string from fourcc code
    static const char* FourCCToString(uint fourcc, char* buf, int bufSize);

    static nKernelServer* kernelServer;

private:
    class ShaderEntry
    {
    public:
        /// default constructor
        ShaderEntry();
        /// constructor
        ShaderEntry(uint shaderFourCC, const char* shaderName);
        /// set fourcc code
        void SetFourCC(uint shaderFourCC);
        /// get shader fourcc code
        uint GetFourCC() const;
        /// set shader name
        void SetName(const char* shaderName);
        /// get shader name
        const char* GetName() const;
        /// get shader pointer
        nShader2* GetShader() const;
        /// is shader ref valid?
        bool IsShaderValid() const;
        /// invalidate entry
        void Invalidate();
        /// set shader
        void SetShader(nShader2* shd);
        /// set the shader variable mask
        void SetVariableMask(uint mask);
        /// get the shader variable mask
        uint GetVariableMask() const;
        /// is the variable mask valid?
        bool IsVariableMaskValid() const;
        /// set the variable mask valid flag
        void SetVariableMaskValid(bool b);

        uint variableMask;     // a set bit for each shader variable valid for this shader
        uint fourcc;
        nString name;
        nRef<nShader2> refShader;
        bool variableMaskValid;
    };

    /// compute the shader variable mask (a set bit for each variable this this shader uses)
    void UpdateShaderVariableMasks();
    /// load the shader resource 
    bool LoadShaders();
    /// unload the shader resource
    void UnloadShaders();
    /// find a shader entry by its fourcc code
    ShaderEntry* FindShaderEntry(uint fourcc) const;

    nArray<ShaderEntry> shaderArray;
    nVariable::Handle modelViewVarHandle;
    nVariable::Handle modelViewProjectionVarHandle;
    nVariable::Handle modelEyePosVarHandle;
    nVariable::Handle modelVarHandle;
    bool useMasksValid;
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
nMaterialNode::GetShaderAt(int index, uint& fourcc, const char*& name) const
{
    fourcc = this->shaderArray[index].GetFourCC();
    name   = this->shaderArray[index].GetName();
}

//------------------------------------------------------------------------------
/**
    Convert a string to a fourcc code.
*/
inline
uint
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
nMaterialNode::FourCCToString(uint fourcc, char* buf, int bufSize)
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
nMaterialNode::ShaderEntry::ShaderEntry() :
    variableMask(0),
    fourcc(0),
    variableMaskValid(false)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
nMaterialNode::ShaderEntry::ShaderEntry(uint shaderFourCC, const char* shaderName) :
    variableMask(0),
    fourcc(shaderFourCC),
    name(shaderName),
    variableMaskValid(false)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nMaterialNode::ShaderEntry::SetVariableMask(uint mask)
{
    this->variableMask = mask;
}

//------------------------------------------------------------------------------
/**
*/
inline
uint
nMaterialNode::ShaderEntry::GetVariableMask() const
{
    return this->variableMask;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nMaterialNode::ShaderEntry::SetFourCC(uint shaderFourCC)
{
    this->fourcc = shaderFourCC;
}

//------------------------------------------------------------------------------
/**
*/
inline
uint
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
    this->variableMaskValid = false;
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
/**
*/
inline
bool
nMaterialNode::ShaderEntry::IsVariableMaskValid() const
{
    return this->variableMaskValid;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nMaterialNode::ShaderEntry::SetVariableMaskValid(bool b)
{
    this->variableMaskValid = b;
}

//------------------------------------------------------------------------------
#endif

