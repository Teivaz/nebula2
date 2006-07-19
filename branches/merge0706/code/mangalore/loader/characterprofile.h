#ifndef LOADER_CHARACTERPROFILE_H
#define LOADER_CHARACTERPROFILE_H
//------------------------------------------------------------------------------
/**
    @class Loader::CharacterProfile

    A character profile stores general data about the player/character.

    Character profiles are stored in "user:[appname]/character/[profilename]".
        
    (C) 2006 Radon Labs GmbH
*/

#include "loader/userprofile.h"

//------------------------------------------------------------------------------
namespace Loader
{
class  CharacterProfile : public Loader::UserProfile
{
    DeclareRtti;
	DeclareFactory(CharacterProfile);

public:
    /// constructor
    CharacterProfile();
    /// CharacterProfile
    virtual ~CharacterProfile();
    
   /// open the profile, profile will be created if it doesn't exist yet
    virtual bool Open(const nString& characterProfileName);
   
};

RegisterFactory(CharacterProfile);


};
//------------------------------------------------------------------------------
#endif