#pragma once
//STL
#include <unordered_map>
#include <iostream>
#include <algorithm>
#include <fstream>
//box2D
#include <box2d/b2_math.h>
#include <box2d/b2_body.h>
#include <box2d/b2_fixture.h>
#include <box2d/b2_world.h>
//Engine
#include "Debug.h"
#include "Timer.h"
//SDL
#include <SDL_ttf.h>


enum PlayerMoveX
{
    MOVE_LEFT = -1,
    STOP = 0,
    MOVE_RIGHT = 1
};

namespace Animation
{
    const int8_t PLAYER_STILL[1] = {8};
    const int8_t PLAYER_WALK[8] = {0, 1, 2, 3, 4, 5, 6, 7};
    const int8_t PLAYER_RUN[3] = {0, 6, 2};
    const int8_t PLAYER_MIDAIR[1] = {3};
};

struct Res
{
    TTF_Font* fontPlayFair;
};

struct Vector2D
{
    float x;
    float y;
};

enum Material
{
    MAIN = 0,
    KILL = 1,
    GLASS = 2,
    CLOUD = 3
};

enum PlatformType
{
    STATIC = 0,
    ANCHOR = 1
};

struct Box2DPlatform
{
    unsigned int nVerteces;
    Vector2D* Verteces;
    unsigned int Type;
    unsigned int Mat;
};

struct Screen
{
    Vector2D StartPosition;
    unsigned int nPlatforms;
    Box2DPlatform* Platforms;
};

struct Stage
{
    unsigned int nScreens;
    Screen* Screens;
};

struct Component
{
    struct Visual
    {   
        b2Vec2 TextureDimensions;
        SDL_Rect Dst;

        bool Animated;
        bool Facing; //0->right, 1->left
        const int8_t* AnimationType;
        unsigned int CurrentFrame;
        
        SDL_Rect* Frames;
        unsigned int FrameCount; //The amount of separate frames in the sprite sheet file.
        int Delay;
    };

    struct Box2DUserData
    {
        std::size_t ECS_ID;
        Material MATERIAL_ID;
        bool GroundCheck;
        bool Fatal;
        bool Anchor;
    };

    struct Physics
    {
        b2Vec2 Dimensions;
        b2Body* body;

        struct Actor
        {    
            b2Vec2 SmoothedPosition;
            b2Vec2 PreviousPosition;
            
            void b2BodyToInterpolation(b2Body*& body);
        };

        struct Solid
        {
            Material Mat;
        };
    };

    struct Player
    {
        b2Vec2 Dimensions;
        b2Vec2 Velocity; 
        PlayerMoveX MoveState;

        SDLTimer JumpTime;
        SDLTimer CoyoteTime;
        unsigned int DoubleJump;
        int GroundContacts;

        int* Animations = nullptr;
    };

    struct Load
    {
        bool Restart;
    };
};

struct Registry
{
    std::unordered_map<std::size_t, Component::Visual> regGraphics;
    std::unordered_map<std::size_t, Component::Player> regPlayer;
    std::unordered_map<std::size_t, Component::Physics> regPhysics;
    std::unordered_map<std::size_t, Component::Physics::Actor> regActor;
    std::unordered_map<std::size_t, Component::Physics::Solid> regSolid; 
    std::unordered_map<std::size_t, Component::Box2DUserData> regUser;
    std::unordered_map<std::size_t, Component::Load> regStage;
};
struct System 
{
    struct Visual
    {
    private:
	    std::unordered_map<std::size_t, SDL_Texture*> TextureMap;

        unsigned int TextureCount = 0;
    public:
	    const std::size_t& LoadFromFile(const std::size_t& ID, const char* path);
	    void Drop(const std::size_t& ID);

	    void RenderTexture(const std::size_t& ID, const SDL_Rect& pDst, const float& angle, SDL_Point* center,
	    SDL_RendererFlip flip);

	    void SetBlending(const std::size_t& ID, const SDL_BlendMode& blend);	
	    void SetAlpha(const std::size_t& ID, const Uint8& alpha);	
	    void Tint(const std::size_t& ID, const SDL_Color& tint);
	
	    void LoadSpriteSheetFromFile(const std::size_t EntityID, const std::string& filepath, const int& ClipNumber, 
        const int& ClipHeight, const int& ClipWidth, const int& ClipsInARow);
	    void PlayAnimation(const std::size_t& EntityID, const SDL_Rect& dst, const int& frames, 
        const int& delay, const double& angle, SDL_Point* center, SDL_RendererFlip flip);

	    SDL_Texture* GetTexturePtr(const std::size_t& ID);

	    ~Visual();
        Visual() {}
    public:
        void Render(Registry& reg);
        void Update(Registry& pReg);
    };
    

    struct Physics
    {
    private:
        int32_t VelocityIterations;
        int32_t PositionIterations;

        float TimestepAccumulator;
        float TimestepAccumulatorRatio;

        float FixedTimestep;
    public:
        b2World* World;
        void Update(const float& Dt);
        void Interpolate(Registry& reg, const double& alpha);
    public:
        Physics(const float& timestepFixed, const float& gravity);

        void SetTimestep(const float& timestep)
        {
            FixedTimestep = timestep;
        }

        b2Body* GetBodyList()
        {
            return World->GetBodyList();
        }

        unsigned int GetBodyCount()
        {
            return World->GetBodyCount();
        }

        b2World* GetWorld()
        {
            return World;
        }
    };

    struct Player
    {
        const float MaxJumpHeight = 0.1f;
        const float MaxFallSpeed = 0.1f;
        const float Gravity = 90.0f;
        const float DecelerationSpeed = 0.1f;
    
        void Update(const std::size_t& ID, Registry& reg);
    };

    struct Load
    {
    private:
        unsigned int CurrentScreen;
        Stage CurrentStage;
        Stage PreviousStage;
        b2Vec2 StartPos;
        bool GoToNext = 0;
        unsigned int Level = 1;
        std::vector<std::size_t> PlatformIDs;
        std::size_t PlayerID;
    public:
        void LoadStage(const std::string& filepath);
        void LoadScreen(const Screen& screen);
        void UnloadScreen(const Screen& screen);
        void LoadNext();
        void FlagNext();
        void Print(const Screen& screen);
        void Teleport(const std::size_t& ID, Registry* reg, const Vector2D& position);
        void SetPlayerID(const std::size_t& ID);
        void Update(const std::size_t& ID, Registry& reg);
    };
};













