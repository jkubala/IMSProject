#include "hexagonGUI.hpp"
#include <iostream>

HexagonGUI::HexagonGUI(int w, int h, int x, int y, const std::string& imagePath) : w(w), h(h), x(x), y(y)
{
    SDL_Surface* surface = IMG_Load(imagePath.c_str());
    if(!surface)
    {
        std::cerr << "Failed to create surface! " << imagePath.c_str() << "\nError: " << IMG_GetError() <<  std::endl;
    }

    hexagonTexture = SDL_CreateTextureFromSurface(Window::renderer, surface);
    if(!hexagonTexture)
    {
        std::cerr << "Failed to create texture!\n";
    }
    SDL_FreeSurface(surface);
}

HexagonGUI::~HexagonGUI()
{
    SDL_DestroyTexture(hexagonTexture);
}

void HexagonGUI::draw() const
{
    SDL_Rect rect = { x, y, w, h};
    if(hexagonTexture)
    {
        SDL_RenderCopy(Window::renderer, hexagonTexture, nullptr, &rect);
    }
}