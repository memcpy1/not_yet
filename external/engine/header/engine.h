#pragma once
//STL
#include <iostream>
#include <string>
//vendor
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <sstream>
//Engine
#include "Timer.h"
#include "Text.h"
#include "Debug.h"
#include "Event.h"
#include "ECS.h"
#include "Collision.h"
//box2D
#include <box2d/b2_polygon_shape.h>
#include <box2d/b2_fixture.h>

class Engine
{
private:
    SDL_Window* Window;
    SDL_Renderer* Renderer;
    static Engine* sInstance;
    
    Registry EngineRegistry;
    Res Resources;
    std::size_t MaxEntities;

    System::Visual GraphicsSystem;
    System::Player PlayerSystem;
    System::Physics PhysicsSystem;
    DebugDrawSDL PhysicsDebugger;

    CollisionListener collisionListener;
    EventHandler KeyboardInput;
    std::size_t Player;

    unsigned int Width;
    unsigned int Height;

    SDLTimer EngineTimer;
    SDLTimer FPSLimiter;
    std::stringstream DisplayFPS;
    uint32_t FrameCount = 0;

    uint64_t CurrentTick;
    uint64_t LastTick;
    float DeltaTime;

    bool Running = 1;
public:
    float Gravity;  
    int FixedFPS = 60;
    int TicksPerFrame = 1000 / FixedFPS;
    float PhysicsTimestep = 1.0f / (float)FixedFPS;
public:
    static Engine* Get();
    bool Initialize(std::string title, const unsigned int& width, const unsigned int& height, bool vsync);

    void Update();
    void PollEvents();
    void Render();
    void Quit();

    void ToggleVisualDebugging();

    bool IsRunning();

    SDLTimer* GetTimer();
    SDL_Renderer* GetRenderer();
    SDL_Surface* GetWindowSurface();
    EventHandler* GetEventHandler();
    
    Registry* GetRegistry();

    b2Vec2 SDLBox2D(const b2Vec2& vec2);
	b2Vec2 Box2DSDL(const b2Vec2& vec2);
	float Box2DSDLf(const float& f);
	float SDLBox2Df(const float& f);

    //ECS
    std::size_t RegisterSolid(const b2Vec2& position, const b2Vec2& dimensions);
    std::size_t RegisterActor(const b2Vec2& position, const b2Vec2& dimensions, const bool& kinematic, const float& angle, const float& density, const float& frictionCoeff);
    std::size_t RegisterPlayer(const b2Vec2& position, const b2Vec2& dimensions, const std::string& texturePath);
    void DestroySolid(const std::size_t& dimensions);
    std::size_t CreateEntity();
    void DestroyEntity();
    std::size_t GetMaxEntity();
private:
    Engine() 
        : PhysicsSystem(System::Physics(1.0f / FixedFPS, 0.94f)) {}
};