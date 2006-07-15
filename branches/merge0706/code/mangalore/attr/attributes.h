#ifndef GAME_ATTRIBUTES_H
#define GAME_ATTRIBUTES_H
//------------------------------------------------------------------------------
/**
    This is the central attribute registry for Mangalore. For more information
    on attributes, see Db::Attribute.
    To add your own attributes to your project, create your own attributes.h
    and attributes.cc files, and compile them into your project.

    (C) 2005 Radon Labs GmbH
*/
#include "attr/attributeid.h"

//------------------------------------------------------------------------------
namespace Attr
{
    DeclareFloat(TestFloatAttr);
    DeclareInt(TestIntAttr);
    DeclareString(TestStringAttr);
    DeclareVector3(TestVector3Attr);
    DeclareVector4(TestVector4Attr);
    DeclareBool(TestBoolAttr);
    DeclareString(_Dummy);
    DeclareString(_Type);
    DeclareString(_Category);
    DeclareString(_Level);
    DeclareString(_ID);
    DeclareBool(StartLevel);
    DeclareString(GUID);
    DeclareVector3(Center);
    DeclareVector3(Extents);
    DeclareString(CurrentLevel);
    DeclareString(Id);
    DeclareString(Name);
    DeclareString(Placeholder);
    DeclareMatrix44(Transform);
    DeclareInt(AnimIndex);
    DeclareString(Physics);
    DeclareString(Audio);
    DeclareBool(Collide);
    DeclareBool(AllowScale);
    DeclareString(PhysicMaterial);
    DeclareFloat(AudioInnerRadius);
    DeclareFloat(AudioOuterRadius);
    DeclareBool(AudioFadein);
    DeclareInt(TargetEntityId);
    DeclareFloat(Time);
    DeclareString(File);
    DeclareString(EntityGroup);

    // PhysicsProperty
    DeclareVector3(VelocityVector);    // the current absolute velocity vector

    // ActorPhysicsProperty
    DeclareFloat(RelVelocity);       // the currently set relative velocity (0..1)
    DeclareFloat(MaxVelocity);       // the maximum velocity
    DeclareBool(Following);         // true if currently following somebody
    DeclareBool(Moving);            // true if currently moving

    // (simple) graphics property
    DeclareString(Graphics);    // name of the graphics resource

    // actor graphics property
    DeclareString(AnimSet);     // name of the animation set
    DeclareString(CharacterSet);// name of the character 3 charset

} // namespace Attr
//------------------------------------------------------------------------------
#endif
