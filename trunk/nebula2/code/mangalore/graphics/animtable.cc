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
AnimTable::AnimTable()
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
    return this->xmlSpreadsheet.Open();
}

//------------------------------------------------------------------------------
/**
*/
void
AnimTable::Close()
{
    this->xmlSpreadsheet.Close();
}

//------------------------------------------------------------------------------
/**
*/
const nString&
AnimTable::Lookup(const nString& id, const nString& animName) const
{
    nXmlTable& xmlTable = this->xmlSpreadsheet.TableAt(0);
    int idColIndex = xmlTable.FindColIndex(id);
    n_assert(idColIndex >= 0);

    int row;
    int num = xmlTable.NumRows();
    for (row = 1; row < num; row++)
    {
        if (xmlTable.Cell(row, 0).AsString() == animName)
        {
            return xmlTable.Cell(row, idColIndex).AsString();
        }
    }
    n_error("AnimTable::Lookup(): could not resolve id(%s), animName(%s) in file '%s'!", id.Get(), animName.Get(), this->GetFilename().Get());
    static const nString dummyStr;
    return dummyStr;
}

}; // namespace Graphics