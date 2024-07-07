#include "Event.h"
#include "Engine.h"


void EventHandler::Poll()
{
    while (SDL_PollEvent(&Event))
    {
        switch (Event.type)
        {
        case SDL_QUIT:
            Engine::Get()->Quit();
            break;
        case SDL_KEYDOWN:
            KeyDown();
            break;
        case SDL_KEYUP:
            KeyUp();
            break;
        case SDL_MOUSEMOTION:
            
            break;
        case SDL_MOUSEBUTTONDOWN:
            
            break;
        case SDL_MOUSEBUTTONUP:
            
            break;
        }
    }
}


bool EventHandler::IsKeyDown(SDL_Scancode key)
{
    return Keystates[key];
}

EventHandler::EventHandler()
    : Event() 
{
    Keystates = (uint8_t*)SDL_GetKeyboardState(0);
}
void EventHandler::KeyDown()
{
    Keystates = (uint8_t*)SDL_GetKeyboardState(0);
}
void EventHandler::KeyUp()
{
    Keystates = (uint8_t*)SDL_GetKeyboardState(0);
}
