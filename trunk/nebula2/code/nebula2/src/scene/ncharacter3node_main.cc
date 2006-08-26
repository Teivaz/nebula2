//------------------------------------------------------------------------------
//  ncharacter3node_main.cc
//  (C) 2005 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "scene/ncharacter3node.h"
#include "variable/nvariableserver.h"
#include "scene/nrendercontext.h"
#include "scene/nsceneserver.h"
#include "scene/nanimator.h"
#include "kernel/ndirectory.h"
#include "character/ncharacter3set.h"
#include "util/nstream.h"

nNebulaScriptClass(nCharacter3Node, "ntransformnode");

//------------------------------------------------------------------------------
/**
*/
nCharacter3Node::nCharacter3Node():
    characterSetIndex(-1)
{
}

//------------------------------------------------------------------------------
/**
*/
nCharacter3Node::~nCharacter3Node()
{
    // empty
}


//------------------------------------------------------------------------------
/**
*/
bool
nCharacter3Node::RenderTransform(nSceneServer* sceneServer, nRenderContext* renderContext, const matrix44& parentMatrix)
{
    bool success = true;

    int i;
    nClass* nTransformNodeClass = this->kernelServer->FindClass("ntransformnode");

    // read characterset variable from rendercontext
    if(this->characterSetIndex != -1)
    {
        nVariable& varCharacter3Set = renderContext->GetLocalVar(this->characterSetIndex);
        nCharacter3Set* charSet = (nCharacter3Set*) varCharacter3Set.GetObj();
        if((charSet) && (charSet->IsValid()))
        {
            // ok, valid characterset found

            // set skins
            for( i = 0; i < this->loadedSkins.Size(); i++)
            {
                if(this->loadedSkins[i]->IsA(nTransformNodeClass))
                {
                    nTransformNode* node = (nTransformNode*) this->loadedSkins[i];
                    bool isVisible = charSet->IsSkinVisibleAtIndex(i);
                    node->SetActive( isVisible );
                };
            };

            // set animation
            nCharacter3SkinAnimator* animator = this->FindMySkinAnimator();
            const char* stateChannel = animator->GetStateChannel();
            int setAnim = charSet->GetCurrentAnimation();
            nVariable::Handle varHandle;
            varHandle = nVariableServer::Instance()->GetVariableHandleByName(stateChannel);
            nVariableServer::Instance()->SetIntVariable(varHandle, setAnim);

            // set variation
//            animator->SetCurrentVariation(charSet->GetCurrentVariationIndex());
        };
    };

    success &= nTransformNode::RenderTransform(sceneServer, renderContext, parentMatrix);


    // FIXME : something's not calculated correct here, the boundbox is definately too large
    //         nevertheless it is acceptable for the moment
    bbox3 myNewBBox;
    // update boundbox
    for( i = 0; i < this->loadedSkins.Size(); i++)
    {
        if(this->loadedSkins[i]->IsA(nTransformNodeClass))
        {
            nTransformNode* node = (nTransformNode*) this->loadedSkins[i];
            if(node->GetActive())
            {
                // extend boundbox
                bbox3 skinBox = node->GetLocalBox();
                matrix44 skinMatrix = node->GetTransform();
                skinBox.transform(skinMatrix);
                myNewBBox.extend(skinBox);
            };

        };
    };
    this->SetLocalBox(myNewBBox);


    return success;
};

//------------------------------------------------------------------------------
/**
*/
void
nCharacter3Node::RenderContextCreated(nRenderContext* renderContext)
{
    nTransformNode::RenderContextCreated(renderContext);
    nVariable var;
    var.SetType(nVariable::Object);
    var.SetObj(0);
    this->characterSetIndex = renderContext->AddLocalVar(var);
    this->FindMySkinAnimator()->SetCharacterSetIndexHandle(this->characterSetIndex);
};

//------------------------------------------------------------------------------
/**
*/
/*
bool
nCharacter3Node::RenderGeometry(nSceneServer* sceneServer, nRenderContext* renderContext)
{
    return nSceneNode::RenderGeometry(sceneServer, renderContext);
};
*/

//------------------------------------------------------------------------------
/**
*/
bool
nCharacter3Node::LoadResources()
{
    bool result = true;

    if(!this->AreResourcesValid())
    {
        result &= nSceneNode::LoadResources();
        nString name = this->GetName();
        this->LoadSkinsFromSubfolder(nString("gfxlib:characters/")+name+"/skins");

        // now try to initialize the characternodes by using the default skin list
        nString skinList,variation;
        if(this->ReadCharacterStuffFromXML(nString("proj:export/gfxlib/characters/")+name+"/skinlists/_auto_default_.xml",skinList,variation))
        {
            nArray<nString> skin;
            skinList.Tokenize(" ",skin);

            // set skins
            nClass* nTransformNodeClass = this->kernelServer->FindClass("ntransformnode");
            int i,k;
            for( i = 0; i < this->loadedSkins.Size(); i++)
            {
                nArray<nString> nameTokens;
                int numTokens = this->loadedSkinName[i].Tokenize("/",nameTokens);

                if(this->loadedSkins[i]->IsA(nTransformNodeClass))
                {
                    nTransformNode* node = (nTransformNode*) this->loadedSkins[i];

                    bool visible = false;

                    if(numTokens == 3)
                    {
                        for(k = 0; k < skin.Size(); k += 3)
                        {
                            if( (skin[k] == nameTokens[0]) &&
                                (skin[k+1] == nameTokens[1]) &&
                                (skin[k+2] == nameTokens[2]) )
                            {
                                visible = true;
                            };
                        };
                    };
                    node->SetActive(visible);
                };
            };
        };
    };

    return result;
};

//------------------------------------------------------------------------------
/**
*/
void
nCharacter3Node::UnloadResources()
{
    if(this->AreResourcesValid())
    {
        nSceneNode::UnloadResources();
    };
};

//------------------------------------------------------------------------------
/**
*/
int
nCharacter3Node::GetRenderContextCharacterSetIndex() const
{
    return this->characterSetIndex;
};


//------------------------------------------------------------------------------
/**
*/
nArray<nString>
nCharacter3Node::GetNamesOfLoadedSkins() const
{
    nArray<nString> result;
    int i;
    for( i = 0; i < this->loadedSkins.Size(); i++)
    {
        result.Append(this->loadedSkins[i]->GetName());
    };
    return result;
};

//------------------------------------------------------------------------------
/**
*/
nArray<nString>
nCharacter3Node::GetFullNamesOfLoadedSkins() const
{
    return this->loadedSkinName;
};


//------------------------------------------------------------------------------
/**
*/
nCharacter3SkinAnimator*
nCharacter3Node::FindMySkinAnimator()
{
    // Find nCharacter3SkinAnimator Class
    nClass* nCharacter3SkinAnimatorClass = this->kernelServer->FindClass("ncharacter3skinanimator");
    nCharacter3SkinAnimator* firstFoundNode = (nCharacter3SkinAnimator*)this->FindFirstInstance(this, nCharacter3SkinAnimatorClass);
    n_assert(firstFoundNode);
    return firstFoundNode;
};

//------------------------------------------------------------------------------
/**
*/
nArray<nString>
nCharacter3Node::GetNamesOfLoadedAnimations()
{
    nCharacter3SkinAnimator* animator = this->FindMySkinAnimator();
    return animator->GetNamesOfLoadedAnimations();
};


//------------------------------------------------------------------------------
/**
*/
nArray<nString>
nCharacter3Node::GetNamesOfLoadedVariations()
{
    nCharacter3SkinAnimator* animator = this->FindMySkinAnimator();
    return animator->GetNamesOfLoadedVariations();
};


//------------------------------------------------------------------------------
/**
*/
void
nCharacter3Node::LoadSkinsFromSubfolder(nString path)
{
    nDirectory* dir = nFileServer2::Instance()->NewDirectoryObject();
    if (dir->Open(path.Get()))
    {
        if (dir->SetToFirstEntry()) do
        {
            if (dir->GetEntryType() == nDirectory::FILE)
            {
                nString curPath = dir->GetEntryName();
                if(curPath.CheckExtension("n2"))
                {
//                    n_printf("Loading %s ... ",curPath.Get());
                    // load new object

                    nKernelServer* kernelServer = nKernelServer::Instance();
                    kernelServer->PushCwd(this);
                    nRoot* nObj = (nRoot*)kernelServer->Load(curPath.Get());
                    n_assert(nObj);
                    kernelServer->PopCwd();

                    this->loadedSkins.Append(nObj);

                    nString name;
                    nArray<nString> directories;
                    int tokens = curPath.Tokenize("/",directories);
                    if(tokens >= 3)
                    {
                        name.Append(directories[directories.Size()-3]);
                        name.Append("/");
                    };
                    if(tokens >= 2)
                    {
                        name.Append(directories[directories.Size()-2]);
                        name.Append("/");
                    };
                    name.Append(curPath.ExtractFileName());
                    name.StripExtension();
                    this->loadedSkinName.Append(name);

//                    n_printf("Done!\n");
                };
            }
            else if (dir->GetEntryType() == nDirectory::DIRECTORY)
            {
                // recurse
                this->LoadSkinsFromSubfolder(path + "/" + dir->GetEntryName().ExtractFileName());
            }
        } while (dir->SetToNextEntry());
        dir->Close();
    };
    delete dir;
};

//------------------------------------------------------------------------------
/**
    Recursively Find instance of nClass, used to search for lights, skin
    animators, etc...
*/
nRoot*
nCharacter3Node::FindFirstInstance(nRoot* node, nClass* classType)
{
    nRoot* resultNode = NULL;
    if (node == NULL)
    {
        resultNode = NULL;
    }
    else
    {
        if (node->IsInstanceOf(classType))
        {
            resultNode = node;
        }
        else
        {
            resultNode = FindFirstInstance(node->GetSucc(), classType);
            if (resultNode == NULL)
            {
                resultNode = FindFirstInstance(node->GetHead(), classType);
            }
        }
    }
    return resultNode;
}

//------------------------------------------------------------------------------
/**
    read skinlist from a XML file
*/
bool
nCharacter3Node::ReadCharacterStuffFromXML(nString fileName,nString &resultSkinList,nString &resultVariation)
{
    resultVariation = "";
    resultSkinList = "";

    fileName = nFileServer2::Instance()->ManglePath(fileName);
    if(!nFileServer2::Instance()->FileExists(fileName))
    {
        return false;
    };

    nString skins;

    nStream stream;
    stream.SetFilename(fileName);
    stream.Open(nStream::Read);
    if((!stream.IsOpen()) || (!stream.HasNode("/skins")))
    {
        // something went wrong, return
        return false;
    };

    stream.SetToNode("/skins");
    if(stream.SetToFirstChild())
    {
        do
        {
            nString current = stream.GetCurrentNodeName();
            skins += current + nString(" ");

            if(!stream.SetToFirstChild())
            {
                // something went wrong, return
                return false;
            };

            current = stream.GetCurrentNodeName();
            skins += current + nString(" ");

            if(!stream.SetToFirstChild())
            {
                // something went wrong, return
                return false;
            };

            current = stream.GetCurrentNodeName();
            skins += current + nString(" ");

            stream.SetToParent();
            stream.SetToParent();

        } while(stream.SetToNextChild());
    };

    if(stream.HasNode("/variation"))
    {
        stream.SetToNode("/variation");
        if(stream.SetToFirstChild())
        {
            resultVariation = stream.GetCurrentNodeName();
        };
    };

    stream.Close();

    resultSkinList = skins;

    return true;
};

//------------------------------------------------------------------------------
/**
    save skinlist to a XML file
*/
bool
nCharacter3Node::WriteCharacterStuffFromXML(nString fileName, nString skins,nString variation)
{
    fileName = nFileServer2::Instance()->ManglePath(fileName);
    nArray<nString> skin;
    skins.Tokenize(" ",skin);

    nStream stream;
    stream.SetFilename(fileName);
    stream.Open(nStream::Write);
    if(!stream.IsOpen())
    {
        // something went wrong, return
        return false;
    };

    stream.BeginNode("skins");
    int i;
    for(i = 0; i < skin.Size(); i += 3)
    {
        stream.BeginNode(skin[i]);
        stream.BeginNode(skin[i+1]);
        stream.BeginNode(skin[i+2]);
        stream.EndNode();
        stream.EndNode();
        stream.EndNode();
    };
    stream.EndNode();
    if(variation.IsValid())
    {
        stream.BeginNode("variation");
            stream.BeginNode(variation);
            stream.EndNode();
        stream.EndNode();
    };
    stream.Close();

    return true;
};
