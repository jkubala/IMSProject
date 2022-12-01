#include <iostream>
#include "window.hpp"

int main(int argc, char** argv)
{
   Window window(800, 600, "World map");

    while(!window.isClosed())
    {
        window.pollEvents();
        window.clear();
    }
    std::cout << "Hello World" << std::endl;

    return 0;
}