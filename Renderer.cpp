#include "Renderer.h"

void Renderer::setColor(SDL_Renderer* renderer, Color color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
}

void Renderer::drawSquare(SDL_Renderer* renderer, SquarePos pos, Color color) {
    SDL_FRect rect = { pos.x, pos.y, SQUARE_SIZE, SQUARE_SIZE };
    setColor(renderer, color);
    SDL_RenderFillRect(renderer, &rect);
}

void Renderer::drawCircle(SDL_Renderer* renderer, int centerX, int centerY, int radius, Color color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    for (int w = 0; w < radius * 2; w++) {
        for (int h = 0; h < radius * 2; h++) {
            int dx = radius - w;
            int dy = radius - h;
            if ((dx * dx + dy * dy) <= (radius * radius)) {
                SDL_RenderPoint(renderer, centerX + dx, centerY + dy);
            }
        }
    }
}
