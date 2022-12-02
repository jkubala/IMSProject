#include <iostream>
#include <unistd.h>
#include <libgen.h>
#include <vector>
#include <chrono>
#include <thread>
#include "hexagonField.hpp"

// Max characters of the path that leads to png resources
#define PATH_MAX 1024
#define WINDOW_SIZE_X 1920
#define WINDOW_SIZE_Y 1080
#define HEX_SIZE 96
#define N_OF_ROWS 10
#define N_OF_COLS 15
void pollEvents(Window& window) {
    SDL_Event event;

    /* For polling events on multiple objects,
    add it the same way that window is added*/
    if(SDL_PollEvent(&event))
    {
        window.pollEvents(event);
    }
}

unsigned int aiStep(unsigned int interval, void * param)
{
    std::vector<HexagonField*>* map = static_cast<std::vector<HexagonField*>*>(param);
    (*map)[0]->gui->updateTextString((*map)[0]->q, (*map)[0]->r);
    return interval;
}

int main(int argc, char **argv)
{
    // Prepare window, anchors so that the map is centered, get png resources and generate hexGrid
    Window window(WINDOW_SIZE_X, WINDOW_SIZE_Y, "World map");
    int xAnchor = (WINDOW_SIZE_X - (HEX_SIZE * 0.75 * N_OF_COLS)) / 2;
    int yAnchor = (WINDOW_SIZE_Y - ((N_OF_ROWS + 0.5) * HEX_SIZE)) / 2;
    
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) == NULL)
    {
       std::cerr << "Unable to get current folder!\n";
       return 1;
    }

    const std::string friendlyHexagonPath = std::string(dirname(cwd)) + "/src/resources/hexagonFriendly.png";
    const std::string enemyHexagonPath = std::string(dirname(cwd)) + "/src/resources/hexagonEnemy.png";
    const std::string fontPath = "/home/jankubala/Downloads/IMSProject/src/resources/arial.ttf";

    int horizontalOffset = xAnchor;
    int verticalOffset = yAnchor;
    int colShift = HEX_SIZE * 0.5f;

    std::vector <HexagonField*> map;
    for (int q = 0; q < N_OF_ROWS; q++)
    {
        verticalOffset = yAnchor + (HEX_SIZE * q);
        for (int r = 0; r < N_OF_COLS; r++)
        {
            // Shift every second column in a row down by half a hexagon
            r % 2 == 1 ? verticalOffset = yAnchor + (q * HEX_SIZE) + colShift : verticalOffset = yAnchor + (q * HEX_SIZE);
            horizontalOffset = xAnchor + (r * (HEX_SIZE * 0.75));
            HexagonField* newHexagonField = new HexagonField(q, r, -q-r, HEX_SIZE, horizontalOffset, verticalOffset, friendlyHexagonPath, fontPath);
            map.push_back(newHexagonField);
        }
    }


    SDL_TimerID aiStepTimer = SDL_AddTimer(3000, aiStep, &map);
    while(!window.isClosed())
    {
        for(HexagonField* hexagonField : map)
        {
            if(hexagonField->gui != nullptr)
            {
                hexagonField->gui->draw();
            }
            else
            {
                std::cerr << "hexagonField with nullptr as gui detected!";
                return 1;
            }
        }
        window.clear();
        pollEvents(window);
    }
    SDL_RemoveTimer(aiStepTimer);
    for(HexagonField* hexagonField : map)
    {
        delete(hexagonField);
    }

    return 0;
}

