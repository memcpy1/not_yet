#pragma once
#include <SDL.h>
#include <unordered_map>
 
class EventHandler
{
private:
    SDL_Event Event;
    uint8_t* Keystates;
    std::unordered_map<SDL_Scancode, bool> FrozenKeys;
public:
    static EventHandler* GetInstance();
 
    void Poll();
    bool IsKeyDown(SDL_Scancode key);
    EventHandler();
private:
    void KeyDown();
    void KeyUp();        
};