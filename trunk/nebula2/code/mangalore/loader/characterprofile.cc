//------------------------------------------------------------------------------
//  loader/characterprofile.cc
//  (C) 2006 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "loader/characterprofile.h"
#include "loader/server.h"

namespace Loader
{
ImplementRtti(Loader::CharacterProfile, Loader::UserProfile);
ImplementFactory(Loader::CharacterProfile);

//------------------------------------------------------------------------------
/**
*/
CharacterProfile::CharacterProfile()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
CharacterProfile::~CharacterProfile()
{
    if (this->IsOpen())
    {
        this->Close();
    }
}

//------------------------------------------------------------------------------
/**
    Open the profile for reading and writing. This will open the embedded
    stream object.
*/
bool
CharacterProfile::Open(const nString& characterProfileName)
{
    // build filename of character profile file
    nString filename = Server::Instance()->GetUserProfile()->GetProfileDirectory();
    filename += "/character/";
    filename += characterProfileName;
    filename += ".xml";

    this->stream.SetFilename(filename);
    if (this->stream.Open(nStream::ReadWrite))
    {
        // if new file, write a root node
        if (this->stream.FileCreated())
        {
            this->stream.BeginNode("Profile");
            this->stream.EndNode();
        }
        this->stream.SetToNode("/Profile");
        return true;
    }
    return false;
}

} // namespace Loader



