#ifndef AUDIO_WAVEBANK_H
#define AUDIO_WAVEBANK_H
//------------------------------------------------------------------------------
/**
    @class Audio::WaveBank

    Holds a number of sound resources. There is usually one wave bank
    associated with a game level.

    (C) 2003 RadonLabs GmbH
*/
#include "foundation/refcounted.h"
#include "foundation/ptr.h"
#include "util/narray.h"
#include "xml/nxmlspreadsheet.h"

// windows.h hack
#ifdef FindResource
#undef FindResource
#endif

class nSound3;

//------------------------------------------------------------------------------
namespace Audio
{
class WaveResource;

class WaveBank : public Foundation::RefCounted
{
    DeclareRtti;
	DeclareFactory(WaveBank);

public:
    /// constructor
    WaveBank();
    /// destructor
    virtual ~WaveBank();
	/// set audio xls table filename
	void SetFilename(const nString& fileName);
	/// get audio xls table filename
	const nString& GetFilename() const;
	/// open an audio xls table and parse it
	bool Open();
	/// close the audio xls table
	void Close();
    /// return true if currently open
    bool IsOpen() const;
    /// create a wave resource
    WaveResource* CreateResource(const nString& name);
    /// add a wave resource to the wave bank (incr refcount)
    void AddResource(WaveResource* res);
    /// find a wave resource by name
    WaveResource* FindResource(const nString& name);
    /// get number of wave resources
    int GetNumResources() const;
    /// get wave resource at
    WaveResource* GetResourceAt(int index) const;

private:
    /// create a sound object from an xml table row
    nSound3* CreateSoundObjectFromXmlTable(const nXmlTable& xmlTable, int rowIndex, const nString& fileColumnName);

    nArray< Ptr<WaveResource> > waveArray;
	nXmlSpreadSheet xmlSpreadSheet;
};

RegisterFactory(WaveBank);

//------------------------------------------------------------------------------
/**
*/
inline
bool
WaveBank::IsOpen() const
{
    return this->xmlSpreadSheet.IsOpen();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
WaveBank::SetFilename(const nString& fileName)
{
	if (fileName.IsEmpty())
	{
		n_error("Audio::WaveBank::SetFilename(): got no filename!");
	}

	this->xmlSpreadSheet.SetFilename(fileName);
}


//------------------------------------------------------------------------------
/**
*/
inline
const nString&
WaveBank::GetFilename() const
{
	return this->xmlSpreadSheet.GetFilename();
}


} // namespace Audio
//------------------------------------------------------------------------------
#endif
