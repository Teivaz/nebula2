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
    /// save object to persistent stream
    virtual bool SaveCmds(nPersistServer *ps);
    /// update transform and render into scene server
    virtual bool RenderTransform(nSceneServer* sceneServer, nRenderContext* renderContext, const matrix44& parentMatrix);
    /// load resources for this object
    virtual bool LoadResources();
    /// unload resources
    virtual void UnloadResources();

    /// retrieve names of loaded skins
    nArray<nString> GetNamesOfLoadedSkins() const;
    /// retrieve full names of loaded skins
    nArray<nString> GetFullNamesOfLoadedSkins() const;
    /// retrieve names of loaded animations
    const nArray<nString>& GetNamesOfLoadedAnimations();
    /// retrieve names of loaded variations
    const nArray<nString>& GetNamesOfLoadedVariations();

    /// get index of character set variable in render context
    //int GetRenderContextCharacterSetIndex() const;

    /// read skin list from a XML file
    static bool ReadCharacterStuffFromXML(nString fileName,nString &resultSkinList,nString &resultVariation);
    /// save skin list to a XML file
    static bool WriteCharacterStuffFromXML(nString fileName, nString skins,nString variation);

protected:

    /// loads the skins that belong to this character
    void LoadSkinsFromSubfolder(nString path);

    /// looks up the skin animator which belongs to this character3 node
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
