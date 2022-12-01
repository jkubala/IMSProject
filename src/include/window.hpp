#pragma once
#include <string>
#include <SDL2Win64/SDL.h>
#include <SDL2Win64/SDL_image.h>
class Window
{
public:
    Window(int width, int height, const std::string &title);
    ~Window();

    void pollEvents(SDL_Event &event);
    void clear() const;

    bool isClosed() const { return closed; }

private:
    bool init();

    int width = 800;
    int height = 600;
    std::string title;
    bool closed = false;

    SDL_Window *window = nullptr;

protected:
    SDL_Renderer *renderer = nullptr;
};