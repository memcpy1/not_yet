#include "Text.h"
#include "Engine.h"

#if defined(SDL_TTF_MAJOR_VERSION)
void RenderText(const b2Vec2& vec2, const std::string& text, TTF_Font*& font, const SDL_Color& textColor, const float& angle,
	SDL_Point* center, SDL_RendererFlip flip)
{
	SDL_Surface* surface = TTF_RenderText_Blended(font, text.c_str(), textColor);
	if (!surface) { SDL_Log("[RenderText()] Surface failed to load"); }
	SDL_Texture* texture = SDL_CreateTextureFromSurface(Engine::Get()->GetRenderer(), surface);

	SDL_Rect src = {};
	src.w = surface->w;
	src.h = surface->h;

	SDL_Rect dst = {};
	dst.x = vec2.x;
	dst.y = vec2.y;
	dst.w = src.w;
	dst.h = src.h;

	SDL_RenderCopyEx(Engine::Get()->GetRenderer(), texture, &src, &dst, angle, center, flip);
	SDL_FreeSurface(surface);
	SDL_DestroyTexture(texture);
}
#endif

