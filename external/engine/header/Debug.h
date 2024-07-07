#pragma once
//STL
#include <vector>
#include <iostream>
//box2D
#include <box2d/b2_draw.h>


class DebugDrawSDL : public b2Draw
{
private:
	const float RadiansToDegrees = 180 / 3.14159265359f;
public: 
    DebugDrawSDL() {};
    /// [!] Provided in CCW order. [!]
	virtual void DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color);
	virtual void DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color);
	virtual void DrawCircle(const b2Vec2& center, float radius, const b2Color& color);
	virtual void DrawSolidCircle(const b2Vec2& center, float radius, const b2Vec2& axis, const b2Color& color);
    /// Draws a line.
    virtual void DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color);

	/// Draws a transform. Choose your own length scale.
	/// @param xf a transform.
	virtual void DrawTransform(const b2Transform& xf);
	virtual void DrawPoint(const b2Vec2& p, float size, const b2Color& color);

	b2Vec2 Box2DtoSDLCoords(const b2Vec2& pVertices);
	b2Vec2 SDLtoBox2DCoords(const b2Vec2& vec2);
	float Box2DtoSDLCoordsf(const float& f);

	void DrawGridline(const int& pGridSize);
	void DrawCartesianAxis();
};
