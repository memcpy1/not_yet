#include "ECS.h"
#include "Engine.h"

#include <fstream>
#include <stdio.h>
#include <stdlib.h>


#pragma region GRAPHICS

int _gfxCompareInt(const void *a, const void *b)
{
	return (*(int*) a) - (*(int*) b);
}

void System::Visual::Update(Registry& reg)
{
    for (std::size_t e = 1; e <= Engine::Get()->GetMaxEntity(); e++)
    {
        if (reg.regGraphics.count(e))
        {
			if (reg.regPhysics.count(e) && (reg.regPhysics[e].body->GetType() == b2_dynamicBody))
			{
				reg.regGraphics[e].Dst.x = Engine::Get()->Box2DSDL(reg.regPhysics[e].body->GetPosition()).x 
            	- (reg.regGraphics[e].TextureDimensions.x / 2);
            	reg.regGraphics[e].Dst.y = 
            	(Engine::Get()->GetWindowSurface()->h - Engine::Get()->Box2DSDL(reg.regPhysics[e].body->GetPosition()).y) - 
            	(reg.regGraphics[e].TextureDimensions.y / 2);

            	reg.regGraphics[e].Dst.w = reg.regGraphics[e].TextureDimensions.x;
            	reg.regGraphics[e].Dst.h = reg.regGraphics[e].TextureDimensions.y;
			} 
		
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
		if (reg.regGraphics.count(e)) {
			if (reg.regGraphics[e].Animated)
			{								
				SDL_RenderCopyEx(Engine::Get()->GetRenderer(), GetTexturePtr(e), &reg.regGraphics[e].Frames[reg.regGraphics[e].CurrentFrame], 
				&reg.regGraphics[e].Dst, 0, 0, (SDL_RendererFlip)reg.regGraphics[e].Facing);																
			}
			else
				SDL_RenderCopy(Engine::Get()->GetRenderer(), GetTexturePtr(e), 0, &reg.regGraphics[e].Dst);	
        }
		else if (reg.regSolid.count(e))
		{	
			if (ColorAngleA < 90.0)
				ColorAngleA += 0.001;
			else
				ColorAngleA = ColorAngleB;
			
			if (ColorAngleB > -90.0)
				ColorAngleB -= 0.001;
			else
				ColorAngleB = ColorAngleA;

			FillPolygon(Engine::Get()->GetRenderer(), reg.regSolid[e].SDLVerteces, reg.regSolid[e].nVerteces, 255 * sin(ColorAngleA), 255, 
			255 * sin(ColorAngleB), 200);
		} 
    }
}

void System::Visual::RenderBackground(Registry &reg)
{
	SDL_RenderCopy(Engine::Get()->GetRenderer(), GetTexturePtr(997), 0, 0);
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

int System::Visual::hline(SDL_Renderer* renderer, Sint16 x1, Sint16 x2, Sint16 y)
{
	return SDL_RenderDrawLine(renderer, x1, y, x2, y);;
}

//Algorithm shamelessly stolen from Sabdul Khabir [https://github.com/sabdul-khabir/SDL3_gfx]	
void System::Visual::FillPolygon(SDL_Renderer* renderer, b2Vec2* verteces, int n, Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
	int result;
	int i;
	int y, xa, xb;
	int miny, maxy;
	int x1, y1;
	int x2, y2;
	int ind1, ind2;
	int ints;
	int *gfxPrimitivesPolyInts = NULL;
	int *gfxPrimitivesPolyIntsNew = NULL;
	int gfxPrimitivesPolyAllocated = 0;


	if (verteces == NULL) 
		std::cout << "[FillPolygon]: Verteces array is empty" << '\n';
	if (n < 3) 
		std::cout << "[FillPolygon]: Less than 3 verteces passed" << '\n';

	
	//Allocate temp array, only grow array 
	if (!gfxPrimitivesPolyAllocated) {
		gfxPrimitivesPolyInts = (int *) malloc(sizeof(int) * n);
		gfxPrimitivesPolyAllocated = n;
	} else {
		if (gfxPrimitivesPolyAllocated < n) {
			gfxPrimitivesPolyIntsNew = (int *) realloc(gfxPrimitivesPolyInts, sizeof(int) * n);
			if (!gfxPrimitivesPolyIntsNew) {
				if (!gfxPrimitivesPolyInts) {
					free(gfxPrimitivesPolyInts);
					gfxPrimitivesPolyInts = NULL;
				}
				gfxPrimitivesPolyAllocated = 0;
			} else {
				gfxPrimitivesPolyInts = gfxPrimitivesPolyIntsNew;
				gfxPrimitivesPolyAllocated = n;
			}
		}
	}

	//Check temp array
	if (gfxPrimitivesPolyInts==NULL)       
		gfxPrimitivesPolyAllocated = 0;
	
	miny = verteces[0].y;
	maxy = verteces[0].y;
	for (i = 1; (i < n); i++) {
		if (verteces[i].y < miny) {
			miny = verteces[i].y;
		} else if (verteces[i].y > maxy) {
			maxy = verteces[i].y;
		}
	}

	result = 0;
	for (y = miny; (y <= maxy); y++) {
		ints = 0;
		for (i = 0; (i < n); i++) {
			if (!i) {
				ind1 = n - 1;
				ind2 = 0;
			} else {
				ind1 = i - 1;
				ind2 = i;
			} 
			y1 = verteces[ind1].y;
			y2 = verteces[ind2].y;
			if (y1 < y2) {
				x1 = verteces[ind1].x;
				x2 = verteces[ind2].x;
			} else if (y1 > y2) {
				y2 = verteces[ind1].y;
				y1 = verteces[ind2].y;
				x2 = verteces[ind1].x;
				x1 = verteces[ind2].x;
			} else {
				continue;
			}
			if ( ((y >= y1) && (y < y2)) || ((y == maxy) && (y > y1) && (y <= y2)) ) {
				gfxPrimitivesPolyInts[ints++] = ((65536 * (y - y1)) / (y2 - y1)) * (x2 - x1) + (65536 * x1);
			} 	    
		}

		qsort(gfxPrimitivesPolyInts, ints, sizeof(int), _gfxCompareInt);

		result = 0;
	    result |= SDL_SetRenderDrawBlendMode(renderer, (a == 255) ? SDL_BLENDMODE_NONE : SDL_BLENDMODE_BLEND);
		result |= SDL_SetRenderDrawColor(renderer, r, g, b, a);	

		for (i = 0; (i < ints); i += 2) {
			xa = gfxPrimitivesPolyInts[i] + 1;
			xa = (xa >> 16) + ((xa & 32768) >> 15);
			xb = gfxPrimitivesPolyInts[i+1] - 1;
			xb = (xb >> 16) + ((xb & 32768) >> 15);
			result |= hline(renderer, xa, xb, y);
		}
	}
}


#pragma endregion GRAPHICS

#pragma region PLAYER
void System::Player::Update(const std::size_t& ID, Registry& reg)
{
	
	if (Engine::Get()->GetEventHandler()->IsKeyDown(SDL_SCANCODE_RIGHT))
    {
		reg.regPlayer[ID].MoveState = PlayerMoveX::MOVE_RIGHT;
		reg.regGraphics[ID].Facing = 0;
    }
	else if (Engine::Get()->GetEventHandler()->IsKeyDown(SDL_SCANCODE_LEFT))
    {
		reg.regPlayer[ID].MoveState = PlayerMoveX::MOVE_LEFT;
		reg.regGraphics[ID].Facing = 1;
	}
    else if (Engine::Get()->GetEventHandler()->IsKeyDown(SDL_SCANCODE_RIGHT) && Engine::Get()->GetEventHandler()->IsKeyDown(SDL_SCANCODE_LEFT))   
	{
		reg.regPlayer[ID].MoveState = PlayerMoveX::STOP;
	}
    else
	{
		reg.regPlayer[ID].MoveState = PlayerMoveX::STOP;
	}
	
	if (reg.regPlayer[ID].OnGround)
	{
		if (reg.regPlayer[ID].MoveState == PlayerMoveX::STOP)
		{
			reg.regGraphics[ID].AnimationType = Animation::PLAYER_STILL;
			reg.regGraphics[ID].FrameCount = 1;
		}
		else
		{
			reg.regGraphics[ID].AnimationType = Animation::PLAYER_WALK;
			reg.regGraphics[ID].FrameCount = 4;
		}
	}
	else if (reg.regPhysics[ID].body->GetLinearVelocity().y > 0.1f)
	{
		reg.regGraphics[ID].AnimationType = Animation::PLAYER_MIDAIR;
		reg.regGraphics[ID].FrameCount = 1;
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
    
	unsigned int nScreens = 0;
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
            Stage.read((char*)&ImportedScreens[i].Platforms[j].Force, sizeof(float));
            Stage.read((char*)&ImportedScreens[i].Platforms[j].ForceDirection.x, sizeof(float));
            Stage.read((char*)&ImportedScreens[i].Platforms[j].ForceDirection.y, sizeof(float));
            Stage.read((char*)&ImportedScreens[i].Platforms[j].TeleporterDst.x, sizeof(float));
            Stage.read((char*)&ImportedScreens[i].Platforms[j].TeleporterDst.y, sizeof(float));
        }
    }

    Stage.close();

	CurrentStage.nScreens = nScreens;
	CurrentStage.Screens = ImportedScreens;
	std::cout << CurrentStage.Screens[0].StartPosition.x << " | " << CurrentStage.Screens[0].StartPosition.y << std::endl;
	CurrentScreen = 0;
	LoadScreen(CurrentStage.Screens[CurrentScreen]);
}

void System::Load::LoadScreen(const Screen& screen)
{
	for (unsigned int i = 0; i < screen.nPlatforms; i++)
	{
		PlatformIDs.push_back(Engine::Get()->RegisterSolid(screen.Platforms[i]));
	}
	
	if(screen.StartPosition.x != -8.0f && screen.StartPosition.y != 4.5f)
	{
		Teleport(PlayerID, Engine::Get()->GetRegistry(), screen.StartPosition);
	}
		
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

	if (reg.regPhysics[ID].body->GetPosition().y < -5.0f || reg.regPhysics[ID].body->GetPosition().x < -8.2f || reg.regPhysics[ID].body->GetPosition().x > 8.2f)
		reg.regStage[ID].Restart = 1;

	if (reg.regStage[ID].Restart)
	{
		std::cout << "Restart" << std::endl;
		Teleport(ID, &reg, CurrentStage.Screens[CurrentScreen].StartPosition);
		reg.regStage[ID].Restart = 0;
	}

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
		}
		else
		{
			Level++;
        	std::string Filename = "Level_";
        	Filename += std::to_string(Level);
			std::cout << Filename << '\n';
			LoadStage("../../assets/level/" + Filename + ".bin");
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

void System::Sound::LoadMusic()
{
	Track1 = Mix_LoadMUS("../../assets/audio/Track1.mp3");
	Mix_Volume(1, (MIX_MAX_VOLUME * 50) / 100);
	Mix_PlayMusic(Track1, 3);
}
