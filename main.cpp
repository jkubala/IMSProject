#include <iostream>
#include "window.h"

int main(int argc, char **argv)
{
    Window window(800, 600, "World map");

    while(!window.isClosed())
    {
        window.pollEvents();
        window.clear();
    }

    return 0;
}