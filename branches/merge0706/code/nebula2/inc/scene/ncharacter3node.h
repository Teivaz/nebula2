#ifndef N_CHARACTER3NODE_H
#define N_CHARACTER3NODE_H
//------------------------------------------------------------------------------
/**
    @class nCharacter3Node
    @ingroup Scene

    (C) 2005 RadonLabs GmbH
*/
#include "scene/ntransformnode.h"
#include "scene/ncharacter3skinanimator.h"

class nRenderContext;
//------------------------------------------------------------------------------
class nCharacter3Node : public nTransformNode
{
public:
    /// constructor
    nCharacter3Node();
    /// destructor
    virtual ~nCharacter3Node();
    /// update transform and render into scene server
    virtual bool RenderTransform(nSceneServer* sceneServer, nRenderContext* renderContext, const matrix44& parentMatrix);
    /// on render context created
    //virtual void RenderContextCreated(nRenderContext* renderContext);
    /// load resources for this object
    virtual bool LoadResources();
    /// unload resources
    virtual void UnloadResources();

    /// retreive names of loaded skins
    nArray<nString> GetNamesOfLoadedSkins() const;
    /// retreive full names of loaded skins
    nArray<nString> GetFullNamesOfLoadedSkins() const;
    /// retreive names of loaded animations
    const nArray<nString>& GetNamesOfLoadedAnimations();
    /// retreive names of loaded variations
    const nArray<nString>& GetNamesOfLoadedVariations();

    /// get index of character set variable in rendercontext
    //int GetRenderContextCharacterSetIndex() const;

    /// read skinlist from a XML file
    static bool ReadCharacterStuffFromXML(nString fileName,nString &resultSkinList,nString &resultVariation);
    /// save skinlist to a XML file
    static bool WriteCharacterStuffFromXML(nString fileName, nString skins,nString variation);

protected:

    /// loads the skins that belong to this character
    void LoadSkinsFromSubfolder(nString path);

    /// looks up the skinanimator which belongs to this character3 node
    nCharacter3SkinAnimator* FindMySkinAnimator();

    /// lookup first appearance of a specific class under @c node
    nRoot* FindFirstInstance(nRoot* node, nClass* classType);
    /// update bounding box from loaded skins
    void UpdateBoundingBox();

    nClass* transformNodeClass;
    nArray<nRoot*> loadedSkins;
    nArray<nString> loadedSkinName;
    //int characterSetIndex;
};

//------------------------------------------------------------------------------
#endif
