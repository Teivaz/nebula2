#ifndef N_LW_EXPORT_NODE_TYPE_H
#define N_LW_EXPORT_NODE_TYPE_H
//----------------------------------------------------------------------------
#include "util/nstring.h"

class TiXmlElement;

//----------------------------------------------------------------------------
/**
    @class nLWExportNodeType
    @brief Per-object export settings template.

    The user must specify an export node type for every Lightwave object that
    they want to export to Nebula 2. An export node type specifies what should 
    be exported from Lightwave to Nebula 2 for a particular object.

    Export node types are stored in N2LightwaveExporter.xml.
*/
class nLWExportNodeType
{
public:
    nLWExportNodeType();

    void SetName(const nString& name);
    const nString& GetName() const;
    void SetSceneNodeType(const nString&);
    const nString& GetSceneNodeType() const;
    void SetExportTextures(bool);
    bool GetExportTextures() const;
    void SetExportAnimation(bool);
    bool GetExportAnimation() const;
    void SetVertexComponentMask(int);
    int GetVertexComponentMask() const;

    bool LoadFromXML(TiXmlElement*);
    bool SaveToXML(TiXmlElement*) const;

private:
    nString typeName;
    nString sceneNodeType;
    bool exportTextures;
    bool exportAnimation;
    int vertexComponentMask;
};

//----------------------------------------------------------------------------
/**
*/
inline
nLWExportNodeType::nLWExportNodeType() :
    sceneNodeType("None"),
    exportTextures(false),
    exportAnimation(false),
    vertexComponentMask(0)
{
    // empty
}

//----------------------------------------------------------------------------
/**
*/
inline
void 
nLWExportNodeType::SetName(const nString& name)
{
    this->typeName = name;
}

//----------------------------------------------------------------------------
/**
*/
inline
const nString& 
nLWExportNodeType::GetName() const
{
    return this->typeName;
}

//----------------------------------------------------------------------------
/**
*/
inline
void 
nLWExportNodeType::SetSceneNodeType(const nString& sceneNodeType)
{
    this->sceneNodeType = sceneNodeType;
}

//----------------------------------------------------------------------------
/**
*/
inline
const nString& 
nLWExportNodeType::GetSceneNodeType() const
{
    return this->sceneNodeType;
}

//----------------------------------------------------------------------------
/**
*/
inline
void 
nLWExportNodeType::SetExportTextures(bool exportTextures)
{
    this->exportTextures = exportTextures;
}

//----------------------------------------------------------------------------
/**
*/
inline
bool 
nLWExportNodeType::GetExportTextures() const
{
    return this->exportTextures;
}

//----------------------------------------------------------------------------
/**
*/
inline
void 
nLWExportNodeType::SetExportAnimation(bool exportAnimation)
{
    this->exportAnimation = exportAnimation;
}

//----------------------------------------------------------------------------
/**
*/
inline
bool 
nLWExportNodeType::GetExportAnimation() const
{
    return this->exportAnimation;
}

//----------------------------------------------------------------------------
/**
*/
inline
void 
nLWExportNodeType::SetVertexComponentMask(int mask)
{
    this->vertexComponentMask = mask;
}

//----------------------------------------------------------------------------
/**
*/
inline
int 
nLWExportNodeType::GetVertexComponentMask() const
{
    return this->vertexComponentMask;
}

//----------------------------------------------------------------------------
#endif // N_LW_EXPORT_NODE_TYPE_H
