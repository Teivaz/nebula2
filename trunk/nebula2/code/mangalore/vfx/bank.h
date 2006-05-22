#ifndef VFX_BANK_H
#define VFX_BANK_H
//------------------------------------------------------------------------------
/**
    @class VFX::Bank

    An Fx bank holds templates for visual effects which can be played.

    (C) 2003 RadonLabs GmbH
*/
#include "foundation/refcounted.h"
#include "vfx/effect.h"
#include "xml/nxmlspreadsheet.h"

//------------------------------------------------------------------------------
namespace VFX
{
class Bank : public Foundation::RefCounted
{
    DeclareRtti;
	DeclareFactory(Bank);

public:
    /// constructor
    Bank();
    /// destructor
    virtual ~Bank();
	/// set effect xls table filename
	void SetFilename(const nString& fileName);
	/// get effect xls table filename
	const nString& GetFilename() const;
	/// open an effect xls table and parse it
	virtual bool Open();
	/// close the effetc xls table
	virtual void Close();
    /// add a template effect to the effect bank
    virtual void AddEffect(const nString& effectName, const nString& resourceName, nTime duration);
    /// return a new effect object as clone of an existing template
    virtual Effect* CreateEffect(const nString& templateEffectName, const matrix44& transform);

protected:
    /// find a template effect by name
    Effect* FindEffect(const nString& templateEffectName);

    nArray< Ptr<Effect> > effectArray;
	nXmlSpreadSheet xmlSpreadSheet;
};

RegisterFactory(Bank);

//------------------------------------------------------------------------------
/**
*/
inline
void
Bank::SetFilename(const nString& fileName)
{
	if(fileName.IsEmpty())
	{
		n_error("VFX::Bank::SetFilename(): got no filename!");
	}

	this->xmlSpreadSheet.SetFilename(fileName);
}


//------------------------------------------------------------------------------
/**
*/
inline
const nString&
Bank::GetFilename() const
{
	return this->xmlSpreadSheet.GetFilename();
}


}; // namespace VFX

//------------------------------------------------------------------------------
#endif
