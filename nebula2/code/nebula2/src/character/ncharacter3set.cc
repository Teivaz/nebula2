//------------------------------------------------------------------------------
//  ncharacter3set_main.cc
//  (C) 2005 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "character/ncharacter3set.h"

//------------------------------------------------------------------------------
/**
*/
nCharacter3Set::nCharacter3Set():
    isSetUp(false)
{
}

//------------------------------------------------------------------------------
/**
*/
nCharacter3Set::~nCharacter3Set()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nCharacter3Set::Init(nCharacter3Node* characterNode)
{
    n_assert(characterNode);
    n_assert(characterNode->AreResourcesValid());

    this->selectedSkins.Clear();
    this->skinNames.Clear();

    this->skinNames = characterNode->GetNamesOfLoadedSkins();
    this->variationNames = characterNode->GetNamesOfLoadedVariations();
    this->numAvailableSkins = this->skinNames.Size();
    this->numAvailableVariations = this->variationNames.Size();

    this->selectedVariation = -1;
    int i;
    for( i = 0; i < this->numAvailableSkins; i++)
    {
        this->selectedSkins.Append(false);
    };

    this->isSetUp = true;
};

//------------------------------------------------------------------------------
/**
*/
bool
nCharacter3Set::LoadCharacterSetFromXML(nCharacter3Node* characterNode, const nString& fileName)
{
    n_assert(this->IsValid());

    // try to load the default skin list
    nArray<nString> loadedSkins = characterNode->GetFullNamesOfLoadedSkins();
    nString skinList;
    nString variation;
    if(nCharacter3Node::ReadCharacterStuffFromXML(fileName,skinList,variation))
    {
        if(variation.IsValid())
        {
            this->SetCurrentVariation(variation);
        };

        nArray<nString> skin;
        skinList.Tokenize(" ",skin);

        // set skins
        int i,k;
        for( i = 0; i < this->numAvailableSkins; i++)
        {
            nArray<nString> nameTokens;
            int numTokens = loadedSkins[i].Tokenize("/",nameTokens);

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
            this->selectedSkins[i] = visible;
        };
        return true;
    };
    return false;
};
