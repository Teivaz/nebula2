//------------------------------------------------------------------------------
//  physics/util.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "physics/physicsutil.h"
#include "physics/server.h"

namespace Physics
{

//------------------------------------------------------------------------------
/**
    Do a ray bundle stabbing check (4 spaced out rays) into the world and
    return whether a contact happened, and if yes where.
    If no contact has happened, outContactDist contains the distance
    between to and from.

    @param  from            the origin
    @param  to              the target point
    @param  upVec           the up-vector for bundle construction
    @param  leftVec         the left-vector for bundle construction
    @param  bundleRadius    the radius of the ray bundle
    @param  excludeSet      exclusion set definition
    @param  outContactDist  [out] const distance if contact has happened
    @return                 true if a contact has happened
*/
bool
PhysicsUtil::RayBundleCheck(const vector3& from, const vector3& to, const vector3& upVec, const vector3& leftVec, float bundleRadius, const FilterSet& excludeSet, float& outContactDist)
{
    Physics::Server* physicsServer = (Physics::Server*) Physics::Server::Instance();

    vector3 stabVector = to - from;
    vector3 offset;

    outContactDist = vector3(to - from).len();
    bool contact = false;
    float distance = 0.0f;
    for (int i = 0; i < 4; i++)
    {
        switch (i)
        {
            case 0: offset = upVec * bundleRadius; break;
            case 1: offset = -upVec * bundleRadius; break;
            case 2: offset = leftVec * bundleRadius; break;
            case 3: offset = -leftVec * bundleRadius; break;
            default: break;
        }

        // do ray check
        physicsServer->RayCheck(from + offset, stabVector, excludeSet);

        // collided?
        const nArray<Physics::ContactPoint>& contacts = physicsServer->GetContactPoints();
        int j;
        for (j = 0; j < physicsServer->GetContactPoints().Size(); j++)
        {
            distance = vector3(contacts[j].GetPosition() - from).len();

            // Stay away as far as possible
            if (distance < outContactDist)
            {
                outContactDist = distance;
                contact = true;
            }
        }
    }
    return contact;
}

//------------------------------------------------------------------------------
/**
    Do a normal ray check and return the closest contact.

    @param  from            the origin
    @param  to              the target point
    @param  excludeSet      physics material to ignore on contact
    @param  outContact      [out] the resulting contact point and normal
    @return                 true if a contact has happened
*/
bool
PhysicsUtil::RayCheck(const vector3& from, const vector3& to, const FilterSet& excludeSet, ContactPoint& outContact)
{
    Physics::Server* physicsServer = (Physics::Server*) Physics::Server::Instance();

    vector3 stabVector = to - from;

    // Do ray check
    physicsServer->RayCheck(from, stabVector, excludeSet);

    // get clostest contact
    bool contact = false;
    float minDistance = 100000.0f;
    const nArray<Physics::ContactPoint>& contacts = physicsServer->GetContactPoints();
    int j;
    for (j = 0; j < physicsServer->GetContactPoints().Size(); j++)
    {
        float curDistance = vector3(contacts[j].GetPosition() - from).len();

        // Stay away as far as possible
        if (curDistance < minDistance)
        {
            minDistance = curDistance;
            outContact = contacts[j];
            contact = true;
        }
    }
    return contact;
}

} // namespace Physics
