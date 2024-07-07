#include "ECS.h"
#include "Engine.h"

void System::Visual::Update(Registry& reg)
{
    for (std::size_t e = 1; e <= Engine::Get()->GetMaxEntity(); e++)
    {
        if (reg.regGraphics.count(e) && reg.regPhysics.count(e))
        {
			reg.regGraphics[e].Dst.x = Engine::Get()->Box2DSDL(reg.regPhysics[e].body->GetPosition()).x 
            - (reg.regGraphics[e].TextureDimensions.x / 2);
            reg.regGraphics[e].Dst.y = 
            (Engine::Get()->GetWindowSurface()->h - Engine::Get()->Box2DSDL(reg.regPhysics[e].body->GetPosition()).y) - 
            (reg.regGraphics[e].TextureDimensions.y / 2);
            reg.regGraphics[e].Dst.w = reg.regGraphics[e].TextureDimensions.x;
            reg.regGraphics[e].Dst.h = reg.regGraphics[e].TextureDimensions.y;

			if (reg.regGraphics[e].Animated)
			{
				reg.regGraphics[e].CurrentFrame = 
				reg.regGraphics[e].AnimationType[(int)((SDL_GetTicks64() / reg.regGraphics[e].Delay) % reg.regGraphics[e].FrameCount)];
			}
        }
    }
}

void System::Visual::Render(Registry& reg)
{
    for (std::size_t e = 1; e <= Engine::Get()->GetMaxEntity(); e++)
    {
        if(reg.regGraphics.count(e))
        {
			if (reg.regGraphics[e].Animated)
			{								
				SDL_RenderCopyEx(Engine::Get()->GetRenderer(), GetTexturePtr(e), &reg.regGraphics[e].Frames[reg.regGraphics[e].CurrentFrame], 
				&reg.regGraphics[e].Dst, 0, 0, (SDL_RendererFlip)reg.regGraphics[e].Facing);																
			}
			else
            	SDL_RenderCopy(Engine::Get()->GetRenderer(), GetTexturePtr(e), 0, 
				&reg.regGraphics[e].Dst);
        }   
    }
}

const std::size_t& System::Visual::LoadFromFile(const std::size_t& ID, const char* path)
{
	SDL_Surface* surface = IMG_Load(path);

	if (!surface)
		std::cout << "[SDL2]: Image could not be loaded into surface!   : " << SDL_GetError() << std::endl;

	TextureMap[ID] = SDL_CreateTextureFromSurface(Engine::Get()->GetRenderer(), surface);
	if (!TextureMap[ID])
		std::cout << "[SDL2]: Surface could not be loaded into texture!   : " << SDL_GetError() << std::endl;

	SDL_FreeSurface(surface);
	return ID;
}

void System::Visual::RenderTexture(const std::size_t& ID, const SDL_Rect& pDst, const float& angle, SDL_Point* center,
	SDL_RendererFlip flip)
{
	SDL_Rect src = {};
	src.w;
	src.h;

	SDL_QueryTexture(TextureMap[ID], nullptr, nullptr, &src.w, &src.h);

	SDL_Rect dst;
	dst.x = pDst.x;
	dst.y = pDst.y;
	dst.w = src.w;
	dst.h = src.h;

	SDL_RenderCopyEx(Engine::Get()->GetRenderer(), TextureMap[ID], &src, &dst, angle, center, flip);
}

void System::Visual::SetBlending(const std::size_t& ID, const SDL_BlendMode& blend)
{
	SDL_SetTextureBlendMode(TextureMap[ID], blend);
}
void System::Visual::SetAlpha(const std::size_t& ID, const Uint8& alpha)
{
	SDL_SetTextureAlphaMod(TextureMap[ID], alpha);
}
void System::Visual::Tint(const std::size_t& ID, const SDL_Color& tint)
{
	SDL_SetTextureColorMod(TextureMap[ID], tint.r, tint.g, tint.b);
}

void System::Visual::Drop(const std::size_t& ID)
{
	SDL_DestroyTexture(TextureMap[ID]);
	TextureMap.erase(ID);
}

System::Visual::~Visual()
{
	TextureMap.clear();
}

SDL_Texture* System::Visual::GetTexturePtr(const std::size_t& ID)
{
	return TextureMap[ID];
}

void System::Visual::PlayAnimation(const std::size_t& EntityID, const SDL_Rect& dst, const int& frames, 
const int& delay, const double& angle, SDL_Point* center, SDL_RendererFlip flip)
{
	SDL_RenderCopyEx(Engine::Get()->GetRenderer(), TextureMap[EntityID], 
	&Engine::Get()->GetRegistry()->regGraphics[EntityID].Frames[static_cast<int>((SDL_GetTicks64() / delay) % frames)], 
	&dst, angle, center, flip);
}

void System::Visual::LoadSpriteSheetFromFile(const std::size_t EntityID, const std::string& filepath, 
const int& ClipNumber, const int& ClipHeight, const int& ClipWidth, const int& ClipsInARow)
{
	LoadFromFile(EntityID, filepath.c_str());
	
	Engine::Get()->GetRegistry()->regGraphics[EntityID].Frames = new SDL_Rect[ClipNumber];

	if (TextureMap.count(EntityID))
	{
		for (int i = 0; i < ClipNumber; i++)
		{
			if (!(i >= ClipsInARow))
			{
				if (i == 0)
				{
					Engine::Get()->GetRegistry()->regGraphics[EntityID].Frames[i].x = 0;
					Engine::Get()->GetRegistry()->regGraphics[EntityID].Frames[i].y = 0;
				}
				else
				{
					Engine::Get()->GetRegistry()->regGraphics[EntityID].Frames[i].x = ClipWidth * i;
					Engine::Get()->GetRegistry()->regGraphics[EntityID].Frames[i].y = 0;
				}

				Engine::Get()->GetRegistry()->regGraphics[EntityID].Frames[i].w = ClipWidth;
				Engine::Get()->GetRegistry()->regGraphics[EntityID].Frames[i].h = ClipHeight;
			}

			else if (!(i > ClipsInARow * (i - ClipsInARow + 1)))
			{
				if (i == ClipsInARow)
				{
					Engine::Get()->GetRegistry()->regGraphics[EntityID].Frames[i].x = 0;
					Engine::Get()->GetRegistry()->regGraphics[EntityID].Frames[i].y = ClipHeight;
				}
				else
				{
					Engine::Get()->GetRegistry()->regGraphics[EntityID].Frames[i].x = ClipWidth * (i - ClipsInARow);
					if (ClipsInARow == 1)
						Engine::Get()->GetRegistry()->regGraphics[EntityID].Frames[i].x = 0;
					Engine::Get()->GetRegistry()->regGraphics[EntityID].Frames[i].y = ClipHeight * (i - ClipsInARow);
					if (ClipsInARow == 1)
						Engine::Get()->GetRegistry()->regGraphics[EntityID].Frames[i].y = i * ClipHeight;
				}

				Engine::Get()->GetRegistry()->regGraphics[EntityID].Frames[i].w = ClipWidth;
				Engine::Get()->GetRegistry()->regGraphics[EntityID].Frames[i].h = ClipHeight;
			}

			std::cout << "Clip Extracted" << std::endl;
			std::cout << Engine::Get()->GetRegistry()->regGraphics[EntityID].Frames[i].x << " " 
			<< Engine::Get()->GetRegistry()->regGraphics[EntityID].Frames[i].y << std::endl;
		}
	}
	else 
		printf("[LoadSpriteSheetFromFile() :   Texture does not exist!]");
}

void System::Player::Update(const std::size_t& ID, Registry& reg)
{
	
	if (Engine::Get()->GetEventHandler()->IsKeyDown(SDL_SCANCODE_RIGHT))
    {
		reg.regPlayer[ID].MoveState = PlayerMoveX::MOVE_RIGHT;
		reg.regGraphics[ID].AnimationType = Animation::PLAYER_WALK;
		reg.regGraphics[ID].FrameCount = 4;
		reg.regGraphics[ID].Facing = 0;
    }
	else if (Engine::Get()->GetEventHandler()->IsKeyDown(SDL_SCANCODE_LEFT))
    {
		reg.regPlayer[ID].MoveState = PlayerMoveX::MOVE_LEFT;
		reg.regGraphics[ID].AnimationType = Animation::PLAYER_WALK;
		reg.regGraphics[ID].FrameCount = 4;
		reg.regGraphics[ID].Facing = 1;
	}
    else if (Engine::Get()->GetEventHandler()->IsKeyDown(SDL_SCANCODE_RIGHT) && Engine::Get()->GetEventHandler()->IsKeyDown(SDL_SCANCODE_LEFT))   
	{
		reg.regPlayer[ID].MoveState = PlayerMoveX::STOP;
		reg.regGraphics[ID].FrameCount = 1;
		reg.regPlayer[ID].MoveState = PlayerMoveX::MOVE_LEFT;
		reg.regGraphics[ID].AnimationType = Animation::PLAYER_STILL;
	}
    else
	{
		reg.regPlayer[ID].MoveState = PlayerMoveX::STOP;
		reg.regGraphics[ID].FrameCount = 1;
		reg.regGraphics[ID].AnimationType = Animation::PLAYER_STILL;
	}
        
    if (Engine::Get()->GetEventHandler()->IsKeyDown(SDL_SCANCODE_C))
    {   
        if (reg.regPlayer[ID].GroundContacts < 1)
        {
            if (reg.regPlayer[ID].DoubleJump != 0 && reg.regPlayer[ID].JumpTime.GetTicks() > 300)
            {
                reg.regPhysics[ID].body->ApplyLinearImpulse(b2Vec2(0, 0.001f * reg.regPlayer[ID].JumpTime.GetTicks()), 
                reg.regPhysics[ID].body->GetWorldCenter(), 0);
        
                reg.regPlayer[ID].DoubleJump--;
                reg.regPlayer[ID].JumpTime.Start();
            }
            else
                return;
        }

        else
        {
			std::cout << "Jump" << std::endl;
            reg.regPlayer[ID].DoubleJump = 1;
            reg.regPlayer[ID].JumpTime.Start();
            reg.regPhysics[ID].body->ApplyLinearImpulse(b2Vec2(0, 0.28f), reg.regPhysics[ID].body->GetWorldCenter(), 0);
        }      
    }


    b2Vec2 Vel = reg.regPhysics[ID].body->GetLinearVelocity();
    float VelChangeX = reg.regPlayer[ID].MoveState * 0.5f - Vel.x;
 
    reg.regPhysics[ID].body->ApplyLinearImpulse(b2Vec2((VelChangeX * reg.regPhysics[ID].body->GetMass()), 0), 
    reg.regPhysics[ID].body->GetWorldCenter(), 0);
}

void System::Stage::LoadNext(const std::size_t& PlayerID, const std::size_t& AnchorID)
{
	Unload(Screens[CurrentStage]);
	Load(Screens[CurrentStage++], PlayerID, AnchorID);
}

void System::Stage::Load(Screen& screen, const std::size_t& PlayerID, const std::size_t& AnchorID)
{
	std::vector<std::size_t> IDs;

	for (int i = 0; i < screen.StaticGeometry.size(); i++)
	{
		IDs.push_back(Engine::Get()->RegisterSolid(screen.StaticGeometry[i].Position, screen.StaticGeometry[i].Dimensions));
	}

	screen.StaticGeometryIDs = IDs;

	Engine::Get()->GetRegistry()->regPhysics[PlayerID].body->SetTransform(screen.PlayerPosition, 0);
	Engine::Get()->GetRegistry()->regPhysics[AnchorID].body->SetTransform(screen.AnchorPosition, 0);
}

void System::Stage::Unload(const Screen& screen)
{
	for (int i = 0; i < screen.StaticGeometry.size(); i++)
	{
		Engine::Get()->DestroySolid(screen.StaticGeometryIDs[i]);
	}
}

#pragma region PHYSICS
System::Physics::Physics(const float& timestepFixed, const float& gravity)
    : FixedTimestep(timestepFixed), TimestepAccumulator(0), TimestepAccumulatorRatio(0), VelocityIterations(6), PositionIterations(2)
{
    World = new b2World(b2Vec2(0, -gravity));
}

void System::Physics::Update(const float& Dt)
{  
	// Maximum number of steps, to avoid degrading to an halt.
	const int MAX_STEPS = 5;
 
	TimestepAccumulator += Dt;
	const int nSteps = static_cast<int> (
		std::floor (TimestepAccumulator / FixedTimestep)
	);
	// To avoid rounding errors, touches TimestepAccumulator only if needed.
	if (nSteps > 0)
		TimestepAccumulator -= nSteps * FixedTimestep;

	assert (
		"Accumulator must have a value lesser than the fixed time step" &&
		TimestepAccumulator < FixedTimestep + FLT_EPSILON
	);

	TimestepAccumulatorRatio = TimestepAccumulator / FixedTimestep;
 
	// This is similar to clamp "dt":
	//	dt = std::min (dt, MAX_STEPS * FixedTimestep)
	// but it allows above calculations of fixedTimestepAccumulator_ and
	// fixedTimestepAccumulatorRatio_ to remain unchanged.
	const int nStepsClamped = std::min(nSteps, MAX_STEPS);
	
	for (int i = 0; i < nStepsClamped; i++)
	{
		World->Step(FixedTimestep, VelocityIterations, PositionIterations);
	}
}

void System::Physics::Interpolate(Registry& reg, const double& alpha)
{
    
}

void Component::Physics::Actor::b2BodyToInterpolation(b2Body*& body)
{
    PreviousPosition = body->GetPosition();
}

#pragma endregion PHYSICS

