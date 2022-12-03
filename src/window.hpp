#ifndef WINDOW_H
#define WINDOW_H

#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
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

public:
    static SDL_Renderer *renderer;
    bool change = false;
};
#endif