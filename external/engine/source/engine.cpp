#include <Engine.h>


Engine* Engine::sInstance = nullptr;


bool Engine::Initialize(std::string title, const unsigned int& width, const unsigned int& height, bool vsync)
{
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {
        std::cout << "[SDL2]: SDL_Init() failed   : " << SDL_GetError() << '\n';
        return false;
    }
    if (!(IMG_Init(IMG_INIT_PNG || IMG_INIT_JPG)))
    {
        std::cout << "[SDL_image]: IMG_Init() failed  :" << IMG_GetError() << '\n';
        return false;
    }
    if (TTF_Init() == -1)
    {
        std::cout << "[SDL_TTF]: TTF_Init() failed   : " << TTF_GetError() << std::endl;
        return false;
    }
    if (Mix_Init(MIX_INIT_OGG) && Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) == -1)
    {
        std::cout << "[SDL_mixer]: Mix_Init() failed   : " << Mix_GetError() << std::endl;
        return false;
    }

    Window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN);
    if (!Window)
    {
        std::cout << "[SDL_CreateWindow]: Could not create window.   : " << SDL_GetError() << std::endl;
        return false;
    }

    Renderer = SDL_CreateRenderer(Window, -1, vsync == 1 ? (SDL_RENDERER_ACCELERATED + SDL_RENDERER_PRESENTVSYNC) 
    : SDL_RENDERER_ACCELERATED);

    if (!Renderer)
    {
        std::cout << "[SDL2]: Could not create renderer.   : " << SDL_GetError() << std::endl;
        return false;
    }

    if (vsync)
        PhysicsSystem.SetTimestep(1.0f / 60.0f);


    EngineTimer.Start();

    Width = width;
    Height = height;

    CurrentTick = SDL_GetPerformanceCounter();
    LastTick = 0;

    Resources.fontPlayFair = TTF_OpenFont("../../assets/font/PlayfairDisplay-Regular.ttf", 36); 
    GraphicsSystem.LoadFromFile(997, "../../assets/image/bg/cosmos.png");

    PhysicsDebugger.SetFlags(b2Draw::e_shapeBit);
    PhysicsSystem.GetWorld()->SetDebugDraw(&PhysicsDebugger);

    //Used for collision callbacks
    collisionListener = CollisionListener();
    PhysicsSystem.World->SetContactListener(&collisionListener);
    
    Player = RegisterPlayer(b2Vec2(0, 0), b2Vec2(0.30f, 0.50f), "../../assets/image/Untitled.png");
    std::cout << Player << '\n';

    LevelLoader.SetPlayerID(Player);
    LevelLoader.LoadStage("../../assets/level/Level_1.bin");
    



    return true;
}

Engine* Engine::Get()
{
    return sInstance = (sInstance != nullptr) ? sInstance : new Engine();
}

SDLTimer* Engine::GetTimer()
{
    return &EngineTimer;
}

SDL_Renderer* Engine::GetRenderer()
{
    return Renderer;
}

EventHandler* Engine::GetEventHandler()
{
    return &KeyboardInput;
}

std::size_t Engine::CreateEntity()
{
    static std::size_t Entities;
    ++Entities;

    MaxEntities = Entities;
    return Entities;
}

void Engine::DestroyEntity()
{
    static std::size_t Entities;
    --Entities;

    MaxEntities = Entities;
}

std::size_t Engine::GetMaxEntity()
{
    return MaxEntities;
}

System::Load *Engine::GetLevelLoader()
{
    return &LevelLoader;
}

SDL_Surface* Engine::GetWindowSurface()
{
    return SDL_GetWindowSurface(Window);
}

Registry* Engine::GetRegistry()
{
    return &EngineRegistry;
}

void Engine::PollEvents()
{
    KeyboardInput.Poll();
}

void Engine::Update()
{
    LastTick = CurrentTick;
    CurrentTick = SDL_GetPerformanceCounter();
    
    float Dt = (float)((CurrentTick - LastTick) * 10 / (float)SDL_GetPerformanceFrequency());
    
    PhysicsSystem.Update(Dt);
    PlayerSystem.Update(Player, EngineRegistry);
    GraphicsSystem.Update(EngineRegistry);
    LevelLoader.Update(Player, EngineRegistry);

    FrameCount++;
}

void Engine::Render()
{   
    SDL_SetRenderDrawColor(Renderer, 24, 24, 24, 0xFF);
    SDL_RenderClear(Renderer);
    
    GraphicsSystem.RenderBackground(EngineRegistry);
    GraphicsSystem.Render(EngineRegistry);
    
    SDL_RenderPresent(Renderer);
}

#pragma region ECS
std::size_t Engine::RegisterSolid(const b2Vec2& position, const b2Vec2& dimensions) //DEPRECATED
{
    //Box2D Initialization
    std::size_t Solid = CreateEntity();
    EngineRegistry.regUser[Solid].ECS_ID = Solid;

    b2BodyDef defSolid;
    defSolid.type = b2_staticBody;
    defSolid.position.Set(position.x, position.y);

    b2Body* bodySolid = PhysicsSystem.World->CreateBody(&defSolid);

    b2PolygonShape shapeSolid;
    shapeSolid.SetAsBox(dimensions.x / 2, dimensions.y / 2);

    b2FixtureDef fixtSolid;
    fixtSolid.shape = &shapeSolid;
    fixtSolid.density = 0;
    fixtSolid.userData.pointer = reinterpret_cast<uintptr_t>(&EngineRegistry.regUser[Solid]);

    b2Fixture* SolidFixture = bodySolid->CreateFixture(&fixtSolid);
        
    //Body enters the Registry
    EngineRegistry.regPhysics[Solid].body = bodySolid;
    
    
    return Solid;
}

std::size_t Engine::RegisterSolid(const Box2DPlatform& platformData)
{
    //Box2D Initialization
    std::size_t Solid = CreateEntity();
    EngineRegistry.regUser[Solid].ECS_ID = Solid;
    EngineRegistry.regUser[Solid].Anchor = 0;

    b2BodyDef defSolid;
    defSolid.type = b2_staticBody;

    b2Body* bodySolid = PhysicsSystem.World->CreateBody(&defSolid);

    b2ChainShape shapeSolid;
    b2Vec2* vec = new b2Vec2[platformData.nVerteces];
    for (int i = 0; i < platformData.nVerteces; i++)
        vec[i].Set(platformData.Verteces[i].x, platformData.Verteces[i].y);
    
    shapeSolid.CreateLoop(vec, platformData.nVerteces);

    b2FixtureDef fixtSolid;
    fixtSolid.shape = &shapeSolid;
    fixtSolid.density = 0;
    
    if (platformData.Type == PlatformType::ANCHOR)
    {
        fixtSolid.isSensor = true;
        EngineRegistry.regUser[Solid].Anchor = 1;
    }
    else if (platformData.Mat == Material::KILL)
    {
        EngineRegistry.regUser[Solid].Fatal = 1;
        GraphicsSystem.LoadSpriteSheetFromFile(Solid, "../../assets/image/static.png", 6, 16, 16, 6);
        EngineRegistry.regGraphics[Solid].TextureDimensions = b2Vec2(16, 16);
        EngineRegistry.regGraphics[Solid].Animated = true;
        int staticAnimation = rand() % 4 + 1;
        switch (staticAnimation)
        {
            case 1:
                EngineRegistry.regGraphics[Solid].AnimationType = Animation::STATIC;
            break;
            case 2:
                EngineRegistry.regGraphics[Solid].AnimationType = Animation::STATIC_2;
            break;
            case 3:
                EngineRegistry.regGraphics[Solid].AnimationType = Animation::STATIC_3;
            break;
            case 4:
                EngineRegistry.regGraphics[Solid].AnimationType = Animation::STATIC_4;
            break;
        }
        EngineRegistry.regGraphics[Solid].FrameCount = 6;
        EngineRegistry.regGraphics[Solid].CurrentFrame = 0;
        EngineRegistry.regGraphics[Solid].Delay = 100;
        std::cout << "Position of entity [" << Solid << "] in Box2D space :" 
        << platformData.Verteces[1].x << " | " << platformData.Verteces[1].y << std::endl;

        b2Vec2 Dst = Box2DSDL(b2Vec2(platformData.Verteces[1].x, platformData.Verteces[1].y));
        EngineRegistry.regGraphics[Solid].Dst.x = Dst.x;
        EngineRegistry.regGraphics[Solid].Dst.y = Height - Dst.y;
        EngineRegistry.regGraphics[Solid].Dst.w = 
        Box2DSDLf(b2Distance(b2Vec2(platformData.Verteces[1].x, platformData.Verteces[1].y), b2Vec2(platformData.Verteces[0].x, platformData.Verteces[0].y)));
        EngineRegistry.regGraphics[Solid].Dst.h = EngineRegistry.regGraphics[Solid].Dst.w;
    }
        
    
    fixtSolid.userData.pointer = reinterpret_cast<uintptr_t>(&EngineRegistry.regUser[Solid]);
    b2Fixture* SolidFixture = bodySolid->CreateFixture(&fixtSolid);
        
    //Body enters the Registry
    EngineRegistry.regPhysics[Solid].body = bodySolid;
    
    for (int i = 0; i < platformData.nVerteces; i++)
    {
        vec[i] = Box2DSDL(vec[i]);
        vec[i].y = Height - vec[i].y;
    }

    EngineRegistry.regSolid[Solid].SDLVerteces = vec; 
    EngineRegistry.regSolid[Solid].nVerteces = platformData.nVerteces; 
    EngineRegistry.regSolid[Solid].Mat = (Material)platformData.Mat;
    
    return Solid;
}

std::size_t Engine::RegisterActor(const b2Vec2& position, const b2Vec2& dimensions, const bool& kinematic, 
const float& angle, const float& density, const float& frictionCoeff)
{
    std::size_t Actor = CreateEntity();
    EngineRegistry.regUser[Actor].ECS_ID = Actor;

    
    b2BodyDef defActor;
    if (kinematic)
        defActor.type = b2_kinematicBody;
    else   
        defActor.type = b2_dynamicBody; 
    defActor.allowSleep = false;
    defActor.angle = angle;
    defActor.position.Set(position.x, position.y);

    b2PolygonShape shapeActor;
    shapeActor.SetAsBox(dimensions.x / 2, dimensions.y / 2);

    b2Body* bodyActor = PhysicsSystem.World->CreateBody(&defActor);

    b2FixtureDef fixtActor;
    fixtActor.shape = &shapeActor;
    fixtActor.userData.pointer = reinterpret_cast<uintptr_t>(&EngineRegistry.regUser[Actor]);
    
    fixtActor.density = density;
    fixtActor.friction = frictionCoeff;
    
    b2Fixture* FixtureActor = bodyActor->CreateFixture(&fixtActor);

    EngineRegistry.regPhysics[Actor].body = bodyActor;
    EngineRegistry.regActor[Actor].PreviousPosition = b2Vec2(position.x, position.y);
    
    return Actor;
}

std::size_t Engine::RegisterPlayer(const b2Vec2& position, const b2Vec2& dimensions, const std::string& spritePath)
{
    std::size_t Player = CreateEntity();
    EngineRegistry.regUser[Player].ECS_ID = Player;
    EngineRegistry.regUser[Player].GroundCheck = true;

    b2BodyDef defPlayer;
    defPlayer.type = b2_dynamicBody;
    defPlayer.fixedRotation = true;
    defPlayer.allowSleep = false;
    defPlayer.position.Set(position.x, position.y);

    b2Body* bodyPlayer = PhysicsSystem.World->CreateBody(&defPlayer);

    b2PolygonShape shapePlayer;
    shapePlayer.SetAsBox(dimensions.x / 2, dimensions.y / 2);

    b2FixtureDef fixtPlayer;
    fixtPlayer.shape = &shapePlayer;
    fixtPlayer.density = 0.9f;
    fixtPlayer.friction = 0.3f;
    fixtPlayer.restitution = 0.1f;
    fixtPlayer.userData.pointer = reinterpret_cast<uintptr_t>(&EngineRegistry.regUser[Player]);

    b2Fixture* FixturePlayer = bodyPlayer->CreateFixture(&fixtPlayer);

    b2FixtureDef fixtGroundCheck;
    shapePlayer.SetAsBox(((dimensions.x / 2) - 0.02f), 0.1f, b2Vec2(0, -((dimensions.y / 2))), 0);
    fixtGroundCheck.shape = &shapePlayer;
    fixtGroundCheck.isSensor = true;
    fixtGroundCheck.userData.pointer = reinterpret_cast<uintptr_t>(&EngineRegistry.regUser[Player]);

    FixturePlayer = bodyPlayer->CreateFixture(&fixtGroundCheck);

    GraphicsSystem.LoadSpriteSheetFromFile(Player, spritePath.c_str(), 16, 40, 24, 8);
    int width, height;
    SDL_QueryTexture(GraphicsSystem.GetTexturePtr(Player), 0, 0, &width, &height);

    EngineRegistry.regGraphics[Player].TextureDimensions = b2Vec2(24 * 2, 40 * 2);
    EngineRegistry.regGraphics[Player].Animated = true;
    EngineRegistry.regGraphics[Player].AnimationType = Animation::PLAYER_WALK;
    EngineRegistry.regGraphics[Player].FrameCount = 8;
    EngineRegistry.regGraphics[Player].CurrentFrame = 0;
    EngineRegistry.regGraphics[Player].Delay = 110;
    EngineRegistry.regPhysics[Player].body = bodyPlayer;
    EngineRegistry.regActor[Player].PreviousPosition = b2Vec2(position.x, position.y);
    EngineRegistry.regPlayer[Player].MoveState = PlayerMoveX::STOP;
    EngineRegistry.regPlayer[Player].DoubleJump = 1;

    return Player;
}

void Engine::DestroySolid(const std::size_t& solid)
{
    PhysicsSystem.World->DestroyBody(EngineRegistry.regPhysics[solid].body);
    EngineRegistry.regPhysics.erase(solid);
    EngineRegistry.regUser.erase(solid);
    delete EngineRegistry.regSolid[solid].SDLVerteces;
    EngineRegistry.regSolid.erase(solid);
    DestroyEntity();
}

#pragma endregion ECS

#pragma region SDL

b2Vec2 Engine::SDLBox2D(const b2Vec2& vec2)
{
   return b2Vec2(vec2.x / 80 - 8, vec2.y / 80 - 4.5f);
}

b2Vec2 Engine::Box2DSDL(const b2Vec2& vec2)
{	
   return b2Vec2((vec2.x + 8) * 80, (vec2.y + 4.5f) * 80);
}

float Engine::Box2DSDLf(const float& f)
{	
   return f * 80;
}

float Engine::SDLBox2Df(const float& f)
{	
   return f / 80;
}

void Engine::Quit()
{
    Running = 0;
}

bool Engine::IsRunning()
{
    return Running;
}

#pragma endregion SDL