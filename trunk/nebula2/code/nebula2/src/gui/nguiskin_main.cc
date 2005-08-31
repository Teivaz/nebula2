//------------------------------------------------------------------------------
//  nguiskin_main.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gui/nguiskin.h"
#include "gui/nguiresource.h"
#include "audio3/nsound3.h"
#include "audio3/naudioserver3.h"

nNebulaScriptClass(nGuiSkin, "nroot");

//------------------------------------------------------------------------------
/**
*/
nGuiSkin::nGuiSkin() :
    brushes(256, 256),
    activeWindowColor(1.0f, 1.0f, 1.0f, 1.0f),
    inactiveWindowColor(0.8f, 0.8f, 0.8f, 1.0f),
    titleTextColor(0.0f, 0.0f, 0.0f, 1.0f),
    buttonTextColor(0.0f, 0.0f, 0.0f, 1.0f),
    labelTextColor(0.0f, 0.0f, 0.0f, 1.0f),
    entryTextColor(0.0f, 0.0f, 0.0f, 1.0f),
    textColor(0.0f, 0.0f, 0.0f, 1.0f),
    menuTextColor(0.0f, 0.0f, 0.0f, 1.0f),
    windowFont("GuiDefault"),
    buttonFont("GuiSmall"),
    labelFont("GuiSmall")
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nGuiSkin::~nGuiSkin()
{
    // release sounds
    int i;
    int num = this->sounds.Size();
    for (i = 0; i < num; i++)
    {
        if (this->sounds[i].isvalid())
        {
            this->sounds[i]->Release();
        }
    }
}

//------------------------------------------------------------------------------
/**
    Begin adding skin brushes.
*/
void
nGuiSkin::BeginBrushes()
{
    this->brushes.Clear();
}

//------------------------------------------------------------------------------
/**
    Finish adding brushes.
*/
void
nGuiSkin::EndBrushes()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Add a new skin brush. The brush is defined by its name, the filename
    of a texture, and a rectangle within the texture in absolute texel 
    coordinates. Adding 2 brushes with identical name is a fatal error.

    @param  name    the brush name
    @param  tex     path to texture
    @param  uvPos   top left position of rectangle in uv space
    @param  uvSize  size of rectangle in uv space
    @param  color   modulation color
*/
void
nGuiSkin::AddBrush(const char* name, const char* tex, const vector2& uvPos, const vector2& uvSize, const vector4& color)
{
    n_assert(name);
    n_assert(tex);
    n_assert(!this->FindBrush(name));

    nGuiResource newRes;
    this->brushes.Append(newRes);
    nGuiResource& res = this->brushes.Back();
    res.SetName(name);
    
    // set texture name
    nString texPath = this->texPrefix;
    texPath.Append(tex);
    texPath.Append(this->texPostfix);
    res.SetTextureName(texPath.Get());

    // set uv rect
    rectangle uvRect(uvPos, uvPos + uvSize);
    res.SetAbsUvRect(uvRect);
    res.SetColor(color);
}

//------------------------------------------------------------------------------
/**
    Add a dynamic brush. Instead of a texture, the brush uses a render target.
    An outside source may render into that target (e.g. The Video Server).

    @param  name    the brush name
    @param  width   Render target width.
    @param  height  Render target height.
*/
void
nGuiSkin::AddDynamicBrush(const char* name, int width, int height)
{
    n_assert(name != 0);
    n_assert(!this->FindBrush(name));

    nGuiResource newRes;
    newRes.SetDynamic(true);
    this->brushes.Append(newRes);
    nGuiResource& res = this->brushes.Back();
    res.SetName(name);
    
    rectangle uvRect(vector2(0.0f, 0.0f), vector2(float(width), float(height)));
    res.SetAbsUvRect(uvRect);
    res.SetColor(vector4(1.0f, 1.0f, 1.0f, 1.0f));
}

//------------------------------------------------------------------------------
/**
    Find a brush's GUI resource by its name. Returns a 0 pointer if not
    found.
*/
nGuiResource*
nGuiSkin::FindBrush(const char* name)
{
    n_assert(name);
    int i;
    int num = this->brushes.Size();
    for (i = 0; i < num; i++)
    {
        if (0 == strcmp(name, this->brushes[i].GetName()))
        {
            nGuiResource* res = &(this->brushes[i]);
            return res;
        }
    }
    return 0;
}

//------------------------------------------------------------------------------
/**
    Get pointer to sound object, initialize sound on demand, returns 0 if
    sound filename not set.
*/
void
nGuiSkin::AddSoundObject(const char* name, const char* filename)
{
    n_assert(name);

    // initialize sound
    nSound3* sound = nAudioServer3::Instance()->NewSound();
    n_assert(0 != sound);

    sound->SetStreaming(false);
    sound->SetNumTracks(5);
    sound->SetLooping(false);
    sound->SetAmbient(true);
    sound->SetVolume(1.0f);
    sound->SetPriority(10);
    sound->SetMinDist(500.0f);
    sound->SetMaxDist(5000.0f);
    sound->SetFilename(filename);

    this->soundNames.Append(name);
    this->sounds.Append(sound);
}

//------------------------------------------------------------------------------
/**
    Get pointer to sound object, initialize sound on demand, returns 0 if
    sound filename not set.
*/
nSound3*
nGuiSkin::GetSoundObject(const char* name)
{
    int index = this->soundNames.FindIndex(name);
    if (-1 != index)
    {
        n_assert(this->sounds[index].isvalid());
        if (!this->sounds[index]->IsLoaded())
        {
            if (!this->sounds[index]->Load())
            {
                n_error("nGuiSkin::GetSound(): could not load sound file '%s'!", this->soundNames[index].Get());
                return 0;
            }
        }
        return this->sounds[index];
    }
    return 0;
}

//------------------------------------------------------------------------------
/**
    Get the filename associated with that sound name
*/
const char*
nGuiSkin::GetSound(const char* name) const
{
    int index = this->soundNames.FindIndex(name);
    if(-1 != index) return this->soundNames[index].Get();
    return 0;
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiSkin::SetSoundVolume(const char* name, float volume)
{
    nSound3* sound = this->GetSoundObject(name);
    if(sound)
    {
        sound->SetVolume(volume);
    }
}

//------------------------------------------------------------------------------
/**
*/
float
nGuiSkin::GetSoundVolume(const char* name)
{
    nSound3* sound = this->GetSoundObject(name);
    return sound ? sound->GetVolume() : 0.0f;
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiSkin::UnloadUntouchedGuiResources()
{
    int i;
    int num = this->brushes.Size();
    for (i = 0; i < num; i++)
    {
        nGuiResource& res = this->brushes[i];
        if (res.IsValid() && (!res.IsTouched()))
        {
            res.Unload();
        }
        res.SetTouched(false);
    }
}

