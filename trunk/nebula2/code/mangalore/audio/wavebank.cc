//------------------------------------------------------------------------------
//  audio/wavebank.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "audio3/naudioserver3.h"
#include "audio/waveresource.h"
#include "audio/wavebank.h"
#include "kernel/nfileserver2.h"
#include "foundation/factory.h"

namespace Audio
{
ImplementRtti(Audio::WaveBank, Foundation::RefCounted);
ImplementFactory(Audio::WaveBank);

//------------------------------------------------------------------------------
/**
*/
WaveBank::WaveBank() :
    waveArray(128, 128)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
WaveBank::~WaveBank()
{
    if (this->IsOpen())
    {
        this->Close();
    }
}

//------------------------------------------------------------------------------
/**
    This create a nSound3 object from a xml table row using the defined 
    file name column.
*/
nSound3*
WaveBank::CreateSoundObjectFromXmlTable(const nXmlTable& xmlTable, int rowIndex, const nString& fileColumnName)
{
    nString filename;
    filename.Format("sound:%s", xmlTable.Cell(rowIndex, fileColumnName.Get()).AsString().Get());
    n_assert2(nFileServer2::Instance()->FileExists(filename), filename.Get());

    nSound3* snd = nAudioServer3::Instance()->NewSound();
    snd->SetName(xmlTable.Cell(rowIndex, "Name").AsString().Get());
    snd->SetFilename(filename);
    snd->SetAmbient(xmlTable.Cell(rowIndex, "Ambient").AsBool());
    snd->SetStreaming(xmlTable.Cell(rowIndex, "Stream").AsBool());
    snd->SetLooping(xmlTable.Cell(rowIndex, "Loop").AsBool());
    snd->SetPriority(xmlTable.Cell(rowIndex, "Pri").AsInt());
    snd->SetCategory(nAudioServer3::StringToCategory(xmlTable.Cell(rowIndex, "Category").AsString()));
    snd->SetNumTracks(xmlTable.Cell(rowIndex, "Tracks").AsInt());
    snd->SetMinDist(xmlTable.Cell(rowIndex, "MinDist").AsFloat());
    snd->SetMaxDist(xmlTable.Cell(rowIndex, "MaxDist").AsFloat());
    bool loaded = snd->Load();
    n_assert2(loaded, filename.Get());
    return snd;
}

//------------------------------------------------------------------------------
/**
	Open an audio xls table and parse it.

    @return         true if succed, false if not
*/
bool
WaveBank::Open()
{
    if (!this->xmlSpreadSheet.Open())
    {
		n_error("Audio::WaveBank::Open(): failed to load '%s'", this->xmlSpreadSheet.GetFilename());
        return false;
    }

    nXmlTable& xmlTable = this->xmlSpreadSheet.TableAt(0);
    int numRows = xmlTable.NumRows();
	n_assert(numRows >= 1);
	int numSounds = numRows - 1;
    
    int soundIndex;
    for (soundIndex = 0; soundIndex < numSounds; soundIndex++)
    {
        int rowIndex = soundIndex + 1;

		// create a new wave resource
        const nString& resName = xmlTable.Cell(rowIndex, "Name").AsString();
        WaveResource* wave = this->CreateResource(resName);
        n_assert(wave);

        n_assert(xmlTable.HasColumn("File"));
        wave->AddSoundObject(this->CreateSoundObjectFromXmlTable(xmlTable, rowIndex, "File"));
        if (xmlTable.HasColumn("File2") && xmlTable.Cell(rowIndex, "File2").IsValid())
        {
            wave->AddSoundObject(this->CreateSoundObjectFromXmlTable(xmlTable, rowIndex, "File2"));
        }
        if (xmlTable.HasColumn("File3") && xmlTable.Cell(rowIndex, "File3").IsValid())
        {
            wave->AddSoundObject(this->CreateSoundObjectFromXmlTable(xmlTable, rowIndex, "File3"));
        }
        if (xmlTable.HasColumn("File4") && xmlTable.Cell(rowIndex, "File4").IsValid())
        {
            wave->AddSoundObject(this->CreateSoundObjectFromXmlTable(xmlTable, rowIndex, "File4"));
        }
        wave->SetVolume(xmlTable.Cell(rowIndex, "Volume").AsFloat() * 0.01f);
        this->AddResource(wave);
    }
    return true;
}


//------------------------------------------------------------------------------
/**
	Close the audio xls table.
*/
void
WaveBank::Close()
{
	this->xmlSpreadSheet.Close();
}

//------------------------------------------------------------------------------
/**
    Find a wave resource by name. Return 0 if not found.

    @param  name    a wave resource name
    @return         pointer to WaveResource, or 0 if not found
*/
WaveResource*
WaveBank::FindResource(const nString& name)
{
    n_assert(name.IsValid());
    int num = this->GetNumResources();
    int i;
    for (i = 0; i < num; i++)
    {
        if (0 == strcmp(this->waveArray[i]->GetName().Get(), name.Get()))
        {
            return this->waveArray[i];
        }
    }
    return 0;
}

//------------------------------------------------------------------------------
/**
    Create a wave resource.
*/
WaveResource*
WaveBank::CreateResource(const nString& name)
{
    n_assert(name.IsValid());
    WaveResource* newWave = WaveResource::Create();
    newWave->SetName(name);
    return newWave;
}

//------------------------------------------------------------------------------
/**
    Add wave resource to the wave bank. Increments refcount of the 
    wave resource.
*/
void
WaveBank::AddResource(WaveResource* wave)
{
    n_assert(wave);
    n_assert(!this->FindResource(wave->GetName()));
    this->waveArray.Append(wave);
}


//------------------------------------------------------------------------------
/**
*/
int
WaveBank::GetNumResources() const
{
    return this->waveArray.Size();
}

//------------------------------------------------------------------------------
/**
*/
WaveResource*
WaveBank::GetResourceAt(int index) const
{
    n_assert(waveArray[index] != 0);
    return this->waveArray[index];
}

} // namespace Audio