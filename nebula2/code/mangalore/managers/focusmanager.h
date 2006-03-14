#ifndef MANAGERS_FOCUSMANAGER_H
#define MANAGERS_FOCUSMANAGER_H
//------------------------------------------------------------------------------
/**
    @class Managers::FocusManager

    The FocusManager singleton object manages the global input and camera focus
    entities. There may only be one input and camera focus entity at any 
    time, the input focus entity can be different from the camera focus entity.

    The input focus entity will be the entity which receives input, the camera
    focus entity will be the entity which may manipulate the camera.

    The FocusManager requires an EntityManager to iterate through existing
    entities, and works only on game entities, which have the InputProperty
    and/or CameraProperty (or a derived class thereof) attached.

    Please note that an actual focus switch will happen only once per-frame.
    This is to avoid chain-reactions when 2 or more objects per frame 
    think they currently have the input focus.
    
    (C) 2005 Radon Labs GmbH
*/
#include "game/manager.h"
#include "game/entity.h"
#include "foundation/ptr.h"

//------------------------------------------------------------------------------
namespace Managers
{
class FocusManager : public Game::Manager
{
    DeclareRtti;
	DeclareFactory(FocusManager);

public:
    /// constructor
    FocusManager();
    /// destructor
    virtual ~FocusManager();
    /// get instance pointer
    static FocusManager* Instance();
    /// trigger the focus manager once a frame, actual focus switches will happen here
    virtual void OnFrame();

    /// set input and camera focus to entity, null ptr allowed
    virtual void SetFocusEntity(Game::Entity* entity);
    /// get common focus entity, will fail if input != camera focus entity
    virtual Game::Entity* GetFocusEntity() const;
    /// switch input and camera focus to next entity
    virtual void SetFocusToNextEntity();
    /// switch camera focus to first Entity with VideoCameraProperty(priority) or CameraProperty
    virtual void SetDefaultFocus();

    /// set the current input focus entity, null ptr allowed
    virtual void SetInputFocusEntity(Game::Entity* entity);
    /// get the current input focus entity
    virtual Game::Entity* GetInputFocusEntity() const;
    /// set input focus to next entity
    virtual void SetInputFocusToNextEntity();

    /// set the current camera focus entity, null ptr allowed
    virtual void SetCameraFocusEntity(Game::Entity* entity);
    /// get the current camera focus entity
    virtual Game::Entity* GetCameraFocusEntity() const;
    /// set camera focus to next entity
    virtual void SetCameraFocusToNextEntity();

private:
    static FocusManager* Singleton;

    /// generalized 'set focus to next entity' method
    void SetToNextEntity(bool cameraFocus, bool inputFocus);
    /// actually switch focus entities
    void SwitchFocusEntities();
    /// set focus to first Entity with CameraProperty if an Entity with
    /// VideoCameraProperty exists it will get the focus.
    /// report if such entity is found
    bool SwitchToFirstCameraFocusEntity();

protected:
    Ptr<Game::Entity> inputFocusEntity;
    Ptr<Game::Entity> cameraFocusEntity;
    Ptr<Game::Entity> newInputFocusEntity;
    Ptr<Game::Entity> newCameraFocusEntity;
};

RegisterFactory(FocusManager);

//------------------------------------------------------------------------------
/**
*/
inline
FocusManager*
FocusManager::Instance()
{
    n_assert(Singleton);
    return Singleton;
}

}; // namespace Managers
//------------------------------------------------------------------------------
#endif

