#pragma once
//STL
#include <set>
//box2D
#include <box2d/b2_api.h>
#include <box2d/b2_contact.h>
#include <box2d/b2_world_callbacks.h>
//engine
#include <ECS.h>

class CollisionListener : public b2ContactListener
{
    std::set<b2Fixture*> FixtureUnderPlayer;

    void BeginContact(b2Contact* contact);
    void EndContact(b2Contact* contact); 
};


