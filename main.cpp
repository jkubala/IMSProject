#include <iostream>
#include "window.hpp"
#include "hexagonGUI.hpp"

void pollEvents(Window& window) {
    SDL_Event event;

    /* For polling events on multiple objects,
    add it the same way that window is added*/
    if(SDL_PollEvent(&event))
    {
        window.pollEvents(event);
    }
}

int main(int argc, char **argv)
{
    Window window(800, 600, "World map");
    HexagonGUI hexagonGUI(window, 128, 110, 100, 100, 100, 100, 100, 255);
    while(!window.isClosed())
    {
        hexagonGUI.draw();
        window.clear();
        pollEvents(window);
    } 

    return 0;
}