//------------------------------------------------------------------------------
//  graphics/animtable.cc
//  (C) 2005 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "graphics/animtable.h"

namespace Graphics
{
ImplementRtti(Graphics::AnimTable, Foundation::RefCounted);
ImplementFactory(Graphics::AnimTable);

AnimTable* AnimTable::Singleton = 0;

//------------------------------------------------------------------------------
/**
*/
AnimTable::AnimTable() :
    isOpen(false)
{
    n_assert(0 == Singleton);
    Singleton = this;
}

//------------------------------------------------------------------------------
/**
*/
AnimTable::~AnimTable()
{
    n_assert(Singleton);
    Singleton = 0;
}

//------------------------------------------------------------------------------
/**
*/
bool
AnimTable::Open()
{
    n_assert(!this->IsOpen());

    // create an xml spreadsheet object
    nXmlSpreadSheet xmlSpreadSheet;
    xmlSpreadSheet.SetFilename(this->filename);
    if (xmlSpreadSheet.Open())
    {
        n_assert(xmlSpreadSheet.NumTables() >= 1);
        const nXmlTable& xmlTable = xmlSpreadSheet.TableAt(0);

        // build column index mapper table
        n_assert(xmlTable.NumColumns() >= 2);
        n_assert(xmlTable.NumRows() >= 2);
        int colIndex;
        for (colIndex = 1; colIndex < xmlTable.NumColumns(); colIndex++)
        {
            this->columnIndexMap.Append(xmlTable.Cell(0, colIndex).AsString());
        }

        // build row index mapper table
        int rowIndex;
        for (rowIndex = 1; rowIndex < xmlTable.NumRows(); rowIndex++)
        {
            this->rowIndexMap.Append(xmlTable.Cell(rowIndex, 0).AsString());
        }

        // build anim info table
        this->animInfos.SetSize(this->columnIndexMap.Size(), this->rowIndexMap.Size());

        for (rowIndex = 0; rowIndex < xmlTable.NumRows() - 1; rowIndex++)
        {
            for (colIndex = 0; colIndex < xmlTable.NumColumns() - 1; colIndex++)
            {
                // get content of current row
                nString content = xmlTable.Cell(rowIndex + 1, colIndex + 1).AsString();

                // parse content, it's either just the name of an animation,
                // or a string of the form
                // attr=value; attr=value
                nArray<nString> tokens;
                content.Tokenize(";", tokens);
                
                int i;
                int animCounter=0;

                for (i = 0; i < tokens.Size(); i++)
                {
                    nArray<nString> attrValueTokens;
                    tokens[i].Tokenize("= \t", attrValueTokens);
                    if (attrValueTokens.Size() > 1)
                    {
                        // attr=value type string
                        if (attrValueTokens[0] == "anim")
                        {
                            //this->animInfos.At(colIndex, rowIndex).animName = attrValueTokens[1];

                            AnimInfo info;
                            n_assert( attrValueTokens[1].IsValid() );
                            info.animName=attrValueTokens[1];

                            // Check if next token has a hotspot for this anim
                            if(tokens.Size() > (i+1) )
                            {
                                nString nextToken = tokens.At(i+1);
                                nArray<nString> nextAttrValueTokens;
                                nextToken.Tokenize("= \t", nextAttrValueTokens);
                                if(nextAttrValueTokens[0] == "hotspot")
                                {
                                    n_assert( nextAttrValueTokens[1].IsValid() );
                                    info.hotSpotTime = nextAttrValueTokens[1].AsFloat();
                                    i++;
                                }
                            }
                            
                            n_assert(info.GetAnimName().IsValid());
                            n_assert(info.GetHotSpotTime() >= 0.0f );
                            this->animInfos.At(colIndex, rowIndex).At(animCounter) = info;
                            animCounter++;

                        }
                        /*else if (attrValueTokens[0] == "hotspot")
                        {
                            this->animInfos.At(colIndex, rowIndex).hotSpotTime = attrValueTokens[1].AsFloat();
                        }*/
                        else
                        {
                            n_error("Invalid keyword in '%s' at row %d, column %d: '%s'\n", this->filename.Get(), rowIndex, colIndex, content.Get());
                            return false;
                        }
                    }
                    else
                    {
                        // old style string, just the anim name
                        AnimInfo info;
                        n_assert( tokens[i].IsValid() );
                        info.animName = tokens[i];
                        this->animInfos.At(colIndex, rowIndex).Append(info);
                    }
                }
            }
        }
        xmlSpreadSheet.Close();
        this->isOpen = true;
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
/**
*/
void
AnimTable::Close()
{
    n_assert(this->IsOpen());
    this->isOpen = false;
    this->columnIndexMap.Clear();
    this->rowIndexMap.Clear();
    this->animInfos.SetSize(0, 0);
}

//------------------------------------------------------------------------------
/** lookup anim info by name
 *  By default chooses randomly from the existing anim-variations
*/
const AnimTable::AnimInfo&
AnimTable::Lookup(const nString& id, const nString& animName, bool random) const
{
    n_assert(id.IsValid());
    n_assert(animName.IsValid());

    // lookup the column
    int colIndex = this->columnIndexMap.FindIndex(id);
    if (-1 == colIndex)
    {
        n_error("AnimTable::Lookup(): animation category '%s' not found in '%s'!", id.Get(), this->filename.Get());
    }
    int rowIndex = this->rowIndexMap.FindIndex(animName);
    if (-1 == rowIndex)
    {
        n_error("AnimTable::Lookup(): animation name '%s' not found in '%s'!", animName.Get(), this->filename.Get());
    }

    int idx=0;
    if(random == true)
    {
        idx = rand() % this->animInfos.At(colIndex, rowIndex).Size();
        //idx = n_rand(1.0f, (float) this->animInfos.At(colIndex, rowIndex).Size());
    }

    return this->animInfos.At(colIndex, rowIndex).At(idx);
}

} // namespace Graphics
