#include "Engine.h"
#include "Collision.h"

void CollisionListener::BeginContact(b2Contact* contact)
{
    Component::Box2DUserData* UserData = reinterpret_cast<Component::Box2DUserData*>
    (contact->GetFixtureA()->GetUserData().pointer);
    if (UserData->GroundCheck && contact->GetFixtureA()->IsSensor())
    {
        Engine::Get()->GetRegistry()->regPlayer[UserData->ECS_ID].GroundContacts++;
        FixtureUnderPlayer.insert(contact->GetFixtureB());
    }

    UserData = reinterpret_cast<Component::Box2DUserData*>(contact->GetFixtureB()->GetUserData().pointer);
    if (UserData->GroundCheck && contact->GetFixtureB()->IsSensor())
    {
        std::cout << "Ground Collision" << std::endl;
        Engine::Get()->GetRegistry()->regPlayer[UserData->ECS_ID].GroundContacts++;
        FixtureUnderPlayer.insert(contact->GetFixtureA());
    }
}

void CollisionListener::EndContact(b2Contact* contact)
{
    Component::Box2DUserData* UserData = reinterpret_cast<Component::Box2DUserData*>
    (contact->GetFixtureA()->GetUserData().pointer);
    if (UserData->GroundCheck && contact->GetFixtureA()->IsSensor())
    {
        Engine::Get()->GetRegistry()->regPlayer[UserData->ECS_ID].GroundContacts--;
        FixtureUnderPlayer.erase(contact->GetFixtureB());
    }

    UserData = reinterpret_cast<Component::Box2DUserData*>(contact->GetFixtureB()->GetUserData().pointer);
    if (UserData->GroundCheck && contact->GetFixtureB()->IsSensor())
    {
        Engine::Get()->GetRegistry()->regPlayer[UserData->ECS_ID].GroundContacts--;
        FixtureUnderPlayer.erase(contact->GetFixtureA());
    }
}