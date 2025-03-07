#include "ECS.h"
#include "Engine.h"

#include <fstream>

#pragma region GRAPHICS
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
		}
	}
	else 
		printf("[LoadSpriteSheetFromFile() :   Texture does not exist!]");
}

#pragma endregion GRAPHICS

#pragma region PLAYER
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
            if (reg.regPlayer[ID].DoubleJump > 0 && reg.regPlayer[ID].JumpTime.GetTicks() > 300)
            {
                if(reg.regPhysics[ID].body->GetLinearVelocity().y < 0)
				{
					reg.regPhysics[ID].body->ApplyLinearImpulse(b2Vec2(0.1f * reg.regPlayer[ID].MoveState, 0.19f + abs(reg.regPhysics[ID].body->GetLinearVelocity().y) * 0.09f), 
                	reg.regPhysics[ID].body->GetWorldCenter(), 0);
				}
				else
				reg.regPhysics[ID].body->ApplyLinearImpulse(b2Vec2(0.1f * reg.regPlayer[ID].MoveState, 0.12f), 
				reg.regPhysics[ID].body->GetWorldCenter(), 0);
        
                reg.regPlayer[ID].DoubleJump--;
                reg.regPlayer[ID].JumpTime.Start();
            }
            else
                return;
        }
        else
        {
            reg.regPlayer[ID].JumpTime.Start();
            reg.regPhysics[ID].body->ApplyLinearImpulse(b2Vec2(0, 0.10f), reg.regPhysics[ID].body->GetWorldCenter(), 0);
        }      
    }

    b2Vec2 Vel = reg.regPhysics[ID].body->GetLinearVelocity();
    float VelChangeX = reg.regPlayer[ID].MoveState * 0.5f - Vel.x;
 
    reg.regPhysics[ID].body->ApplyLinearImpulse(b2Vec2((VelChangeX * reg.regPhysics[ID].body->GetMass()), 0), 
    reg.regPhysics[ID].body->GetWorldCenter(), 0);
}

#pragma endregion PLAYER

#pragma region STAGE

void System::Load::LoadStage(const std::string& filepath)
{
    std::fstream Stage;
    Stage.open(filepath, std::ios::in | std::ios::binary);
	
	if(Stage.fail())
		std::cout << "[!] File not found!" << '\n';
    
	unsigned int nScreens = 0; //doesnt read the second file :(
	Stage.read((char*)&nScreens, sizeof(unsigned int));
    Screen* ImportedScreens = new Screen[nScreens];
    
	for (unsigned int i = 0; i < nScreens; i++)
    {
        Stage.read((char*)&ImportedScreens[i].StartPosition.x, sizeof(float));
        Stage.read((char*)&ImportedScreens[i].StartPosition.y, sizeof(float));  
        
		Stage.read((char*)&ImportedScreens[i].nPlatforms, sizeof(unsigned int));
        ImportedScreens[i].Platforms = new Box2DPlatform[ImportedScreens[i].nPlatforms];
        
        for (unsigned int j = 0; j < ImportedScreens[i].nPlatforms; j++)
        {
            Stage.read((char*)&ImportedScreens[i].Platforms[j].nVerteces, sizeof(unsigned int));
            ImportedScreens[i].Platforms[j].Verteces = new Vector2D[ImportedScreens[i].Platforms[j].nVerteces];
            
			for (unsigned int l = 0; l < ImportedScreens[i].Platforms[j].nVerteces; l++)
            {
                Stage.read((char*)&ImportedScreens[i].Platforms[j].Verteces[l].x, sizeof(float));
                Stage.read((char*)&ImportedScreens[i].Platforms[j].Verteces[l].y, sizeof(float));
            }
                
            Stage.read((char*)&ImportedScreens[i].Platforms[j].Type, sizeof(unsigned int));
            Stage.read((char*)&ImportedScreens[i].Platforms[j].Mat, sizeof(unsigned int));
        }
    }

    Stage.close();

	CurrentStage.nScreens = nScreens;
	CurrentStage.Screens = ImportedScreens;
	std::cout << CurrentStage.nScreens << '\n';
	CurrentScreen = 0;
	LoadScreen(CurrentStage.Screens[CurrentScreen]);
}

void System::Load::LoadScreen(const Screen& screen)
{
	for (unsigned int i = 0; i < screen.nPlatforms; i++)
	{
		PlatformIDs.push_back(Engine::Get()->RegisterSolid(screen.Platforms[i]));
	}
	
	if(CurrentScreen <= 0)
		Teleport(PlayerID, Engine::Get()->GetRegistry(), screen.StartPosition);
}

void System::Load::UnloadScreen(const Screen& screen)
{
	for (int i = 0; i < PlatformIDs.size(); i++)
	{
		Engine::Get()->DestroySolid(PlatformIDs[i]);
	}

	PlatformIDs.clear();
}

void System::Load::Update(const std::size_t& ID, Registry& reg)
{	
	if (reg.regStage[ID].Restart)
	{
		std::cout << "dead" << '\n';
	}
		//Teleport(ID, &reg, {StartPos.x, StartPos.y});

	else if (GoToNext)
	{
		if (CurrentScreen > 0)
			UnloadScreen(CurrentStage.Screens[CurrentScreen - 1]);
		else
			UnloadScreen(CurrentStage.Screens[CurrentScreen]);
		
		if (CurrentScreen < CurrentStage.nScreens - 1)
		{
			CurrentScreen++;
			LoadScreen(CurrentStage.Screens[CurrentScreen]);
			StartPos = reg.regPhysics[ID].body->GetPosition();
		}
		else
		{
			Level++;
        	std::string Filename = "Level_";
        	Filename += std::to_string(Level);
			std::cout << Filename << '\n';
			LoadStage("../../res/lvl/" + Filename + ".bin");
		}

		GoToNext = false;
	}
}

void System::Load::FlagNext()
{
	GoToNext = true;
}

void System::Load::Teleport(const std::size_t& ID, Registry* reg, const Vector2D& position)
{
	reg->regPhysics[ID].body->SetTransform({position.x, position.y}, 0);
}

void System::Load::SetPlayerID(const std::size_t &ID)
{
	PlayerID = ID;
}

#pragma endregion STAGE

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

#pragma endregion PHYSICS

