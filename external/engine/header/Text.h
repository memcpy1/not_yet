#include <iostream>
#include <box2d/b2_math.h>

#if defined(SDL_TTF_MAJOR_VERSION)
	void RenderText(const b2Vec2& vec2, const std::string& text, TTF_Font*& font, const SDL_Color& textColor, const float& angle,
		SDL_Point* center, SDL_RendererFlip flip);
#endif