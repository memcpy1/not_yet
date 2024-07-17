#include <Engine.h>
#include <stdlib.h>
#undef main

int main()
{
    Engine::Get()->Initialize("game", 1280, 720, 1);
    
    while (Engine::Get()->IsRunning())
    {
        Engine::Get()->PollEvents();
        Engine::Get()->Update();
        Engine::Get()->Render();
    }
} 