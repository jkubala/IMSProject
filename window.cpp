#include "window.h"
#include "iostream"

Window::Window(int width, int height, const std::string &title)
{
    this->width = width;
    this->height = height;
    this->title = title;
    closed = !init();
}

Window::~Window()
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

bool Window::init()
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        std::cerr << "Failed to initialize SDL!\n";
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

void Window::pollEvents()
{
    SDL_Event event;

    if (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_QUIT:
            closed = true;
            break;
        default:
            break;
        }
    }
}

void Window::clear() const
{
    SDL_SetRenderDrawColor(renderer, 127, 127, 127, 255);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
}