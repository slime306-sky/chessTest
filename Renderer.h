#pragma once

#include "useFullStuff.h"

class Renderer {
public:
    static void setColor(SDL_Renderer* renderer, Color color);
    static void drawSquare(SDL_Renderer* renderer, SquarePos pos, Color color);
    static void drawCircle(SDL_Renderer* renderer, int centerX, int centerY, int radius, Color color);
};
