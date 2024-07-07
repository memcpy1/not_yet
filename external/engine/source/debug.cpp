#include "Debug.h"
#include "Engine.h"

#include "vendor/SDL_Primitives/SDL_prims.h"

float DebugDrawSDL::Box2DtoSDLCoordsf(const float& f)
{
   return f * 80;
}

b2Vec2 DebugDrawSDL::SDLtoBox2DCoords(const b2Vec2& vec2)
{
   return b2Vec2(vec2.x / 80 - 8, -(vec2.y / 80 - 4.5f));
}

b2Vec2 DebugDrawSDL::Box2DtoSDLCoords(const b2Vec2& vec2)
{
   return b2Vec2((vec2.x + 8) * 80, -((vec2.y - 4.5f) * 80));
}


void DebugDrawSDL::DrawPoint(const b2Vec2& pPosition, float pRadius, const b2Color& pColor)
{
   b2Vec2 Position = Box2DtoSDLCoords(pPosition);
   float Radius = Box2DtoSDLCoordsf(pRadius);

   int32_t x = (Radius - 1);
   int32_t y = 0;
   int32_t tx = 1;
   int32_t ty = 1;
   int32_t error = (tx - (Radius * 2));

   SDL_SetRenderDrawColor(Engine::Get()->GetRenderer(), pColor.r * 255, pColor.g * 255, pColor.b * 255, pColor.a * 255);
   while (x >= y)
   {
      //  Each of the following renders an octant of the circle
      SDL_RenderDrawPoint(Engine::Get()->GetRenderer(), Position.x + x, Position.y - y);
      SDL_RenderDrawPoint(Engine::Get()->GetRenderer(), Position.x + x, Position.y + y);
      SDL_RenderDrawPoint(Engine::Get()->GetRenderer(), Position.x - x, Position.y - y);
      SDL_RenderDrawPoint(Engine::Get()->GetRenderer(), Position.x - x, Position.y + y);
      SDL_RenderDrawPoint(Engine::Get()->GetRenderer(), Position.x + y, Position.y - x);
      SDL_RenderDrawPoint(Engine::Get()->GetRenderer(), Position.x + y, Position.y + x);
      SDL_RenderDrawPoint(Engine::Get()->GetRenderer(), Position.x - y, Position.y - x);
      SDL_RenderDrawPoint(Engine::Get()->GetRenderer(), Position.x - y, Position.y + x);

      if (error <= 0)
      {
         ++y;
         error += ty;
         ty += 2;
      }

      if (error > 0)
      {
         --x;
         tx += 2;
         error += (tx - (Radius * 2));
      }
   }

}

void DebugDrawSDL::DrawCircle(const b2Vec2& pCenter, float pRadius, const b2Color& pColor)
{
   b2Vec2 Center = Box2DtoSDLCoords(pCenter);
   float Radius = Box2DtoSDLCoordsf(pRadius);

   int32_t x = (Radius - 1);
   int32_t y = 0;
   int32_t tx = 1;
   int32_t ty = 1;
   int32_t error = (tx - (Radius * 2));

   SDL_SetRenderDrawColor(Engine::Get()->GetRenderer(), pColor.r * 255, pColor.g * 255, pColor.b * 255, pColor.a * 255);
   while (x >= y)
   {
      //  Each of the following renders an octant of the circle
      SDL_RenderDrawPoint(Engine::Get()->GetRenderer(), Center.x + x, Center.y - y);
      SDL_RenderDrawPoint(Engine::Get()->GetRenderer(), Center.x + x, Center.y + y);
      SDL_RenderDrawPoint(Engine::Get()->GetRenderer(), Center.x - x, Center.y - y);
      SDL_RenderDrawPoint(Engine::Get()->GetRenderer(), Center.x - x, Center.y + y);
      SDL_RenderDrawPoint(Engine::Get()->GetRenderer(), Center.x + y, Center.y - x);
      SDL_RenderDrawPoint(Engine::Get()->GetRenderer(), Center.x + y, Center.y + x);
      SDL_RenderDrawPoint(Engine::Get()->GetRenderer(), Center.x - y, Center.y - x);
      SDL_RenderDrawPoint(Engine::Get()->GetRenderer(), Center.x - y, Center.y + x);

      if (error <= 0)
      {
         ++y;
         error += ty;
         ty += 2;
      }

      if (error > 0)
      {
         --x;
         tx += 2;
         error += (tx - (Radius * 2));
      }
   }

}

void DebugDrawSDL::DrawSolidCircle(const b2Vec2& pCenter, float pRadius, const b2Vec2& pAxis, const b2Color& pColor)
{
   b2Vec2 Center = Box2DtoSDLCoords(pCenter);
   b2Vec2 Axis = Box2DtoSDLCoords(pAxis);
   float Radius = Box2DtoSDLCoordsf(pRadius);


   SDL_SetRenderDrawColor(Engine::Get()->GetRenderer(), pColor.r * 255, pColor.g * 255, pColor.b * 255, pColor.a * 255);
   for (int w = 0; w < Radius * 2; w++)
   {
      for (int h = 0; h < Radius * 2; h++)
      {
         int dx = Radius - w;   //horizontal offset
         int dy = Radius - h;   //vertical offset
         if ((dx*dx + dy*dy) <= (Radius * Radius))
         {
            SDL_RenderDrawPoint(Engine::Get()->GetRenderer(), Center.x + dx, Center.y + dy);
         }
      }

      b2Vec2 Position = Center + (Radius * Axis);
      DrawSegment(Center, Position, pColor);
   }

}

void DebugDrawSDL::DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& pColor)
{
   b2Vec2 P1 = Box2DtoSDLCoords(p1);
   b2Vec2 P2 = Box2DtoSDLCoords(p2);

   SDL_SetRenderDrawColor(Engine::Get()->GetRenderer(), pColor.r * 255, pColor.g * 255, pColor.b * 255, pColor.a * 255);
   SDL_RenderDrawLine(Engine::Get()->GetRenderer(), P1.x, P1.y, P2.x, P2.y);
}

void DebugDrawSDL::DrawPolygon(const b2Vec2* pVertices, int32 pVertexCount, const b2Color& pColor)
{
   std::vector<SDL_Point> vertices;
   for(int i = 0; i < pVertexCount; i++)
   {
      vertices.push_back({(int)(Box2DtoSDLCoords(pVertices[i]).x), (int)(Box2DtoSDLCoords(pVertices[i]).y)});
   }


   SDL_DrawPolygon(vertices.data(), pVertexCount, pColor);
}

void DebugDrawSDL::DrawSolidPolygon(const b2Vec2* pVertices, int32 pVertexCount, const b2Color& pColor)
{
   std::vector<SDL_Point> vertices;
   for(int i = 0; i < pVertexCount; i++)
   {
      vertices.push_back({(int)(Box2DtoSDLCoords(pVertices[i]).x), (int)(Box2DtoSDLCoords(pVertices[i]).y)});
   }


   SDL_FillPolygon(Engine::Get()->GetWindowSurface(), vertices.data(), pVertexCount, pColor);
}

void DebugDrawSDL::DrawTransform(const b2Transform& xf)
{
   b2Vec2 TransX = xf.p + (0.5f * xf.q.GetXAxis());
   b2Vec2 TransY = xf.p + (0.5f * xf.q.GetYAxis());

   DrawSegment(xf.p, TransX, {1.0f, 0.0f, 0.0f, 1.0f}); //x
   DrawSegment(xf.p, TransY, {0.0f, 1.0f, 0.0f, 1.0f}); //y
}

void DebugDrawSDL::DrawCartesianAxis()
{
   SDL_SetRenderDrawColor(Engine::Get()->GetRenderer(), 255, 0, 0, 0xFF);
   SDL_RenderDrawLine(Engine::Get()->GetRenderer(), 0, 720 / 2, 1280, 720 / 2);
   SDL_SetRenderDrawColor(Engine::Get()->GetRenderer(), 0, 255, 60, 0xFF);
   SDL_RenderDrawLine(Engine::Get()->GetRenderer(), 1280 / 2, 0, 1280 / 2, 720);
}

void DebugDrawSDL::DrawGridline(const int& pGridSize)
{
	SDL_SetRenderDrawColor(Engine::Get()->GetRenderer(), 33, 33, 33, 255);

   for (int x = 0; x < 1 + 32 * pGridSize; x += pGridSize) 
   {
      SDL_RenderDrawLine(Engine::Get()->GetRenderer(), x, 0, x, 720);
   }

   for (int y = 0; y < 1 + 18 * pGridSize; y += pGridSize) 
   {
      SDL_RenderDrawLine(Engine::Get()->GetRenderer(), 0, y, 1280, y);
   }
}