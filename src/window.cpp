#include "window.hpp"
#include <iostream>

SDL_Renderer* Window::renderer = nullptr;

Window::Window(int width, int height, const std::string &title) : width(width), height(height), title(title)
{
    closed = !init();
}

Window::~Window()
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();
}

bool Window::init()
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        std::cerr << "Failed to initialize SDL!\n";
        return false;
    }

    if(IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG)
    {
        std::cerr << "Failed to initialize SDL_iamge!\n";
        return false;
    }

    window = SDL_CreateWindow(title.c_str(),
                              SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED,
                              width, height, 0);

    if (window == nullptr)
    {
        std::cerr << "Failed to create window!\n";
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);

    if (renderer == nullptr)
    {
        std::cerr << "Failed to create renderer!\n";
        return false;
    }

    return true;
}

void Window::pollEvents(SDL_Event &event)
{
    switch (event.type)
    {
    case SDL_QUIT:
        closed = true;
        break;
    case SDL_KEYDOWN:

        break;
    default:
        break;
    }
}

void Window::clear() const
{
    SDL_RenderPresent(renderer);
    SDL_SetRenderDrawColor(renderer, 127, 127, 127, 255);
    SDL_RenderClear(renderer);
}