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

std::string friendlyHexagonPath;
std::string enemyHexagonPath;
std::vector <HexagonField*> map;
bool playerOnTurn = 0;


void pollEvents(Window& window) {
    SDL_Event event;

    /* For polling events on multiple objects,
    add it the same way that window is added*/
    if(SDL_PollEvent(&event))
    {
        window.pollEvents(event);
    }
}

void redrawGUI(Window* window)
{
    for(HexagonField* hexagonField : map)
    {
        hexagonField->gui->draw();
    }
    window->clear();
}

unsigned int aiStep(unsigned int interval, void * param)
{
    std::cout << "Turn of player:" << playerOnTurn << std::endl;
    Window* window = static_cast<Window*>(param);
    if(window->change)
    {
        map[0]->gui->updateTextString(0, 0);
        map[0]->gui->updateImage(enemyHexagonPath);
    }
    else
    {
        map[0]->gui->updateTextString(10, 10);
        map[0]->gui->updateImage(friendlyHexagonPath);
    }

    playerOnTurn = !playerOnTurn;
    redrawGUI(window);
    
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
    std::string localFolderPath = std::string(dirname(cwd));

    friendlyHexagonPath = localFolderPath + "/src/resources/hexagonFriendly.png";
    enemyHexagonPath = localFolderPath + "/src/resources/hexagonEnemy.png";
    const std::string fontPath =  localFolderPath + "/src/resources/arial.ttf";
    int horizontalOffset = xAnchor;
    int verticalOffset = yAnchor;
    int colShift = HEX_SIZE * 0.5f;

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

    SDL_TimerID aiStepTimer = SDL_AddTimer(1000, aiStep, &window);
    while(!window.isClosed())
    {
        pollEvents(window);
    }

    SDL_RemoveTimer(aiStepTimer);
    for(HexagonField* hexagonField : map)
    {
        delete(hexagonField);
    }

    return 0;
}

