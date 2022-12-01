#ifndef WINDOW_H
#define WINDOW_H
#include <string>
#include <SDL2/SDL.h>
class Window
{
public:
    Window(int width, int height, const std::string &title);
    ~Window();

    void pollEvents();
    void clear() const;

    bool isClosed() const { return closed; }

private:
    bool init();

    int width = 800;
    int height = 600;
    std::string title;
    bool closed = false;

    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
};
#endif