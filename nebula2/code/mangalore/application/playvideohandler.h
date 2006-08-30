#ifndef APPLICATION_PLAYVIDEOHANDLER_H
#define APPLICATION_PLAYVIDEOHANDLER_H
//------------------------------------------------------------------------------
/**
    @class Application::PlayVideoHandler

    Plays a video and waits for a esc or space key press, or that the video has finished.

    (C) 2005 RadonLabs GmbH
*/

#include "application/statehandler.h"

//------------------------------------------------------------------------------
namespace Application
{
class PlayVideoHandler : public StateHandler
{
    DeclareRtti;
	DeclareFactory(PlayVideoHandler);

public:
    /// constructor
    PlayVideoHandler();
    /// destructor
    virtual ~PlayVideoHandler();
    /// set the followup state
    void SetNextState(const nString& state);
    /// get the followup state
    const nString& GetNextState() const;
	/// called each frame as long as state is current, return new state
    virtual nString OnFrame();
	/// called when the state represented by this state handler is entered
    virtual void OnStateEnter(const nString& prevState);

	/// set the video file to play
    void SetVideoFile(const nString& filename);
	/// get the video file to play
	const nString& GetVideoFile() const;

    /// enable video scaling
    void SetEnableScaling(bool enable);
    /// get enable video scaling
    bool GetEnableScaling() const;

private:
    nString nextState;
    nString videoFileName;
    bool enableScaling;
};

RegisterFactory(PlayVideoHandler);

//------------------------------------------------------------------------------
/**
*/
inline
void
PlayVideoHandler::SetNextState(const nString& state)
{
    this->nextState = state;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
PlayVideoHandler::GetNextState() const
{
    return this->nextState;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
PlayVideoHandler::SetVideoFile(const nString& filename)
{
    this->videoFileName = filename;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
PlayVideoHandler::GetVideoFile() const
{
    return this->videoFileName;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
PlayVideoHandler::SetEnableScaling(bool enable)
{
    this->enableScaling = enable;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
PlayVideoHandler::GetEnableScaling() const
{
    return this->enableScaling;
}

};
//------------------------------------------------------------------------------
#endif
