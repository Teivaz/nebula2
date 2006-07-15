//------------------------------------------------------------------------------
//  managers/focusmanager.cc
//  (C) 2005 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "managers/focusmanager.h"
#include "properties/inputproperty.h"
#include "properties/cameraproperty.h"
#include "properties/videocameraproperty.h"
#include "managers/entitymanager.h"
#include "game/entity.h"

namespace Managers
{
ImplementRtti(Managers::FocusManager, Game::Manager);
ImplementFactory(Managers::FocusManager);

FocusManager* FocusManager::Singleton = 0;

using namespace Game;
using namespace Properties;

//------------------------------------------------------------------------------
/**
*/
FocusManager::FocusManager()
{
    n_assert(0 == Singleton);
    Singleton = this;
}

//------------------------------------------------------------------------------
/**
*/
FocusManager::~FocusManager()
{
    n_assert(!this->inputFocusEntity.isvalid());
    n_assert(!this->cameraFocusEntity.isvalid());
    n_assert(!this->newInputFocusEntity.isvalid());
    n_assert(!this->newCameraFocusEntity.isvalid());
    n_assert(Singleton);
    Singleton = 0;
}

//------------------------------------------------------------------------------
/**
    Sets the input and camera focus to the given entity. The entity pointer
    may be 0 to clear the input and camera focus. The entity must have both
    a InputProperty and CameraProperty attached, otherwise the method
    will fail.
*/
void
FocusManager::SetFocusEntity(Entity* entity)
{
    this->SetInputFocusEntity(entity);
    this->SetCameraFocusEntity(entity);
}

//------------------------------------------------------------------------------
/**
    Returns the current common focus entity. This method will fail if the
    current input focus entity and camera focus entity are not the same. The
    method may return 0 if there is no current focus entity.
*/
Entity*
FocusManager::GetFocusEntity() const
{
    if (this->cameraFocusEntity.isvalid())
    {
        n_assert(this->cameraFocusEntity == this->inputFocusEntity);
        return this->cameraFocusEntity;
    }
    return 0;
}

//------------------------------------------------------------------------------
/**
    General "set focus to next entity". Can be camera and/or input focus.
*/
void
FocusManager::SetToNextEntity(bool cameraFocus, bool inputFocus)
{
    n_assert(cameraFocus || inputFocus);

    // get array of active entities
    const nArray<Ptr<Entity> >& entityArray = EntityManager::Instance()->GetEntities();

    // get start entity
    nArray<Ptr<Entity> >::iterator iter = entityArray.Begin();
    if (cameraFocus)
    {
        if (this->cameraFocusEntity.isvalid())
        {
            iter = entityArray.Find(this->cameraFocusEntity);
        }
    }
    else
    {
        if (this->inputFocusEntity.isvalid())
        {
            iter = entityArray.Find(this->inputFocusEntity);
        }
    }
    nArray<Ptr<Entity> >::iterator start = iter;
    if (iter) do
    {
        iter++;

        // wrap around
        if (iter == entityArray.End())
        {
            iter = entityArray.Begin();
        }
        Entity* entity = *iter;
        //TODO that's HACK for DSA that Camera will not switch to NPC view
        if (entity->FindProperty(CameraProperty::RTTI)
            && entity->FindProperty(CameraProperty::RTTI)->IsInstanceOf(CameraProperty::RTTI))
            continue;

        bool hasCameraProperty = (0 != entity->FindProperty(CameraProperty::RTTI));
        bool hasInputProperty  = (0 != entity->FindProperty(InputProperty::RTTI));
        if (cameraFocus && inputFocus && hasCameraProperty && hasInputProperty)
        {
            this->SetFocusEntity(entity);
            return;
        }
        else if (cameraFocus && (!inputFocus) && hasCameraProperty)
        {
            this->SetCameraFocusEntity(entity);
            return;
        }
        else if (inputFocus && (!cameraFocus) && hasInputProperty)
        {
            this->SetInputFocusEntity(entity);
            return;
        }
    }
    while (iter != start);
}

//------------------------------------------------------------------------------
/**
    Set focus to next entity which has both an InputProperty and
    CameraProperty attached. If no current focus entity exists, the method
    will start to iterate with the first entity. The method will wrap around. 
    The method will return false if no entities exist which have both an 
    InputProperty and CameraProperty attached.
*/
void
FocusManager::SetFocusToNextEntity()
{
    this->SetToNextEntity(true, true);
}

//------------------------------------------------------------------------------
/**
    This method is called once per frame by the game server and 
    actually handles focus entity switches.
*/
void
FocusManager::OnFrame()
{
    this->SwitchFocusEntities();
}

//------------------------------------------------------------------------------
/**
    Actually switch focus entities. A focus entity switch doesn't happen
    immediately, but only once per frame. This is to prevent chain-reactions
    and circular reactions when 2 or more entities think they have the
    focus in a single frame.
*/
void
FocusManager::SwitchFocusEntities()
{
    if (this->newInputFocusEntity.isvalid())
    {
        this->inputFocusEntity = this->newInputFocusEntity;
        this->newInputFocusEntity = 0;
        InputProperty* inputProp = (InputProperty*) this->inputFocusEntity->FindProperty(InputProperty::RTTI);
        n_assert(inputProp);
        inputProp->OnObtainFocus();
    }
    if (this->newCameraFocusEntity.isvalid())
    {
        this->cameraFocusEntity = this->newCameraFocusEntity;
        this->newCameraFocusEntity = 0;
        CameraProperty* cameraProp = (CameraProperty*) this->cameraFocusEntity->FindProperty(CameraProperty::RTTI);
        n_assert(cameraProp);
        cameraProp->OnObtainFocus();
    }
}

//------------------------------------------------------------------------------
/**
    Set input focus entity to the given entity. The entity pointer can be
    0, this will clear the current input focus. The entity must have
    an InputProperty attached for this to work.
*/
void
FocusManager::SetInputFocusEntity(Entity* entity)
{
    // clear input focus on all existing focus entities
    // (there may be cases where more then one entity has the input focus flag set,
    // mainly when a new level is loaded)
    EntityManager* entityManager = EntityManager::Instance();
    int i;
    int num = entityManager->GetNumEntities();
    for (i = 0; i < num; i++)
    {
        Entity* entity = entityManager->GetEntityAt(i);
        InputProperty* inputProperty = (InputProperty*) entity->FindProperty(InputProperty::RTTI);
        if (inputProperty && inputProperty->HasFocus())
        {
            inputProperty->OnLoseFocus();
        }
    }
    this->inputFocusEntity = 0;
    this->newInputFocusEntity = entity;
}

//------------------------------------------------------------------------------
/**
    Get current input focus entity. This method may return 0 if no input
    focus entity is set.
*/
Entity*
FocusManager::GetInputFocusEntity() const
{
    return this->inputFocusEntity.get_unsafe();
}

//------------------------------------------------------------------------------
/**
    Set input focus to the next entity which has an InputProperty attached.
*/
void
FocusManager::SetInputFocusToNextEntity()
{
    this->SetToNextEntity(false, true);
}

//------------------------------------------------------------------------------
/**
    Set camera focus entity to the given entity. The entity pointer can be
    0, this will clear the current camera focus. The entity must have
    a CameraProperty attached for this to work.
*/
void
FocusManager::SetCameraFocusEntity(Entity* entity)
{
    // clear camera focus on all existing focus entities
    // (there may be cases where more then one entity has the camera focus flag set,
    // mainly when a new level is loaded)
    EntityManager* entityManager = EntityManager::Instance();
    int i;
    int num = entityManager->GetNumEntities();
    for (i = 0; i < num; i++)
    {
        Entity* entity = entityManager->GetEntityAt(i);
        CameraProperty* cameraProperty = (CameraProperty*) entity->FindProperty(CameraProperty::RTTI);
        if (cameraProperty && cameraProperty->HasFocus())
        {
            cameraProperty->OnLoseFocus();
        }
    }
    this->cameraFocusEntity = 0;
    this->newCameraFocusEntity = entity;
}

//------------------------------------------------------------------------------
/**
    Get current camera focus entity. This method may return 0 if no input
    focus entity is set.
*/
Entity*
FocusManager::GetCameraFocusEntity() const
{
    return this->cameraFocusEntity.get_unsafe();
}

//------------------------------------------------------------------------------
/**
    Set camera focus to next entity which has a CameraProperty attached.
*/
void
FocusManager::SetCameraFocusToNextEntity()
{
    this->SetToNextEntity(true, false);
}

//------------------------------------------------------------------------------
/**
    Set camera focus to first Entity with CameraFocus property.
    @result true, if proper entity found, false if not
*/
bool 
FocusManager::SwitchToFirstCameraFocusEntity()
{
   const nArray<Ptr<Entity> >& entityArray = EntityManager::Instance()->GetEntities();
   nArray<Ptr<Entity> >::iterator iter = entityArray.Begin();

   bool priorityCameraFound = false;
   bool cameraPropertyEntityFound = false;

   while ( iter != entityArray.End() )
   {
        Entity* entity = *iter;
        CameraProperty* cameraProperty = (CameraProperty*) entity->FindProperty(CameraProperty::RTTI);
        bool isPriorityCamera = ( 0 != entity->FindProperty(VideoCameraProperty::RTTI) );
    
        if ( 0 != cameraProperty )
        {
            if ( cameraProperty->HasFocus() ) cameraProperty->OnLoseFocus();
            
            if 
            (
                !cameraPropertyEntityFound
                || ( !priorityCameraFound && isPriorityCamera )
            )
            {
                this->newCameraFocusEntity = entity;
                cameraPropertyEntityFound = true;
                priorityCameraFound = isPriorityCamera;
            }
        }

        iter++;
   }

   return cameraPropertyEntityFound;
}

//------------------------------------------------------------------------------
/**
    Set camera focus to first Entity with "GrabCameraFocus = true" or if none
    found to first "CameraFocus = true" Entity.
*/
void 
FocusManager::SetDefaultFocus()
{
    SwitchToFirstCameraFocusEntity();
}

} // namespace Managers
