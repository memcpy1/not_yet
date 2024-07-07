#pragma once
#include <SDL.h>
#include <box2d/b2_draw.h>

extern void SDL_DrawPolygon(SDL_Point *v, int n, const b2Color& c);
extern void SDL_FillPolygon(SDL_Surface* s, SDL_Point *v, int n, const b2Color& c);


