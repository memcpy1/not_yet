#include "Engine.h"
#include "Collision.h"
#include "ECS.h"

void CollisionListener::BeginContact(b2Contact* contact)
{
    Component::Box2DUserData* UserData = reinterpret_cast<Component::Box2DUserData*>
    (contact->GetFixtureA()->GetUserData().pointer);

    if (UserData->GroundCheck && contact->GetFixtureA()->IsSensor())
    {
        if (UserData->Fatal)
        {
            Engine::Get()->GetRegistry()->regStage[UserData->ECS_ID].Restart = 1;
            std::cout << "Dead!" << '\n';
        }
        
        Engine::Get()->GetRegistry()->regPlayer[UserData->ECS_ID].GroundContacts++;
        Engine::Get()->GetRegistry()->regPlayer[UserData->ECS_ID].DoubleJump = 1;
        Engine::Get()->GetRegistry()->regPlayer[UserData->ECS_ID].OnGround = true;
    } 

    else if (UserData->Anchor == 1 && contact->GetFixtureA()->IsSensor())
    {
        Engine::Get()->GetLevelLoader()->FlagNext();
    }

    UserData = reinterpret_cast<Component::Box2DUserData*>(contact->GetFixtureB()->GetUserData().pointer);
    if (UserData->GroundCheck && contact->GetFixtureB()->IsSensor())
    {
        Engine::Get()->GetRegistry()->regPlayer[UserData->ECS_ID].GroundContacts++;
        Engine::Get()->GetRegistry()->regPlayer[UserData->ECS_ID].DoubleJump = 1;
        Engine::Get()->GetRegistry()->regPlayer[UserData->ECS_ID].OnGround = true;
        
        if (UserData->Fatal)
        {
            Engine::Get()->GetRegistry()->regStage[UserData->ECS_ID].Restart = 1;
            std::cout << "Dead!" << '\n';
        }
    }

    else if (UserData->Anchor == 1 && contact->GetFixtureB()->IsSensor())
    {
        Engine::Get()->GetLevelLoader()->FlagNext();
    }
}

void CollisionListener::EndContact(b2Contact* contact)
{
    Component::Box2DUserData* UserData = reinterpret_cast<Component::Box2DUserData*>
    (contact->GetFixtureA()->GetUserData().pointer);
    if (UserData->GroundCheck && contact->GetFixtureA()->IsSensor())
    {
        Engine::Get()->GetRegistry()->regPlayer[UserData->ECS_ID].GroundContacts--;
        Engine::Get()->GetRegistry()->regPlayer[UserData->ECS_ID].OnGround = false;
    }

    UserData = reinterpret_cast<Component::Box2DUserData*>(contact->GetFixtureB()->GetUserData().pointer);
    if (UserData->GroundCheck && contact->GetFixtureB()->IsSensor())
    {
        Engine::Get()->GetRegistry()->regPlayer[UserData->ECS_ID].GroundContacts--;
        Engine::Get()->GetRegistry()->regPlayer[UserData->ECS_ID].OnGround = false;
    }
}