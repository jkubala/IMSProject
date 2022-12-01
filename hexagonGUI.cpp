#include "hexagonGUI.hpp"

HexagonGUI::HexagonGUI(const Window &window, int w, int h, int x, int y, int r, int g, int b, int a) : Window(window), w(w), h(g), x(x), y(y), r(r), g(g), b(b), a(a)
{
}

void HexagonGUI::draw() const
{
    SDL_Rect rect;
    rect.w = w;
    rect.h = h;
    rect.x = x;
    rect.y = y;

    SDL_SetRenderDrawColor(renderer, r, g, b, a);
    SDL_RenderFillRect(renderer, &rect);
}