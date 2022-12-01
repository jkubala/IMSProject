#include <iostream>
#include <unistd.h>
#include <libgen.h>
#include <vector>

#include "window.hpp"
#include "hexagonGUI.hpp"

// Max characters of the path that leads to png resources
#define PATH_MAX 1024
#define HEX_SIZE 64
#define N_OF_ROWS 20
#define N_OF_COLS 10

#define HEX_START_X 470
#define HEX_START_Y 300
void pollEvents(Window& window) {
    SDL_Event event;

    /* For polling events on multiple objects,
    add it the same way that window is added*/
    if(SDL_PollEvent(&event))
    {
        window.pollEvents(event);
    }
}

void CreateHexagonRow(int nOfHexagons, int startX, int startY, const std::string& imagePath, std::vector<HexagonGUI*>& hexagons)
{
    int horizontalOffset = startX; 
    for(int col = 0; col < nOfHexagons; col++)
    {
        // Offset the next hexagon from starting position 
        horizontalOffset = startX + (HEX_SIZE + (HEX_SIZE / 2)) * col;
        hexagons.emplace_back(new HexagonGUI(HEX_SIZE, HEX_SIZE, horizontalOffset, startY, imagePath));
    }
}

int main(int argc, char **argv)
{
    // Prepare window, hexagon list, get png resources and generate hexGrid
    Window window(1920, 1080, "World map");
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) == NULL)
    {
       std::cerr << "Unable to get current folder!\n";
       return 1;
    }

    std::vector<HexagonGUI*> hexagons;
    const std::string friendlyHexagon = strcat(dirname(cwd), "/src/resources/hexagonFriendly.png");
    const std::string enemyHexagon = strcat(dirname(cwd), "/src/resources/hexagonEnemy.png");

    int startHorizontalOffset = HEX_START_X;
    int verticalOffset = HEX_START_Y;
    int rowShift = HEX_SIZE * 0.75f;

    for (int row = 0; row < N_OF_ROWS; row++)
    {
        // Offset vertically by the amount of rows
        verticalOffset = HEX_START_Y + (HEX_SIZE / 2 * row);
        // Every second row, shift hexagons to align with the row above them
        row % 2 == 0 ? startHorizontalOffset = HEX_START_X : startHorizontalOffset += rowShift;
        CreateHexagonRow(10, startHorizontalOffset, verticalOffset, friendlyHexagon.c_str(), hexagons);
    }

    while(!window.isClosed())
    {
        for(HexagonGUI* hexagon : hexagons)
        {
            hexagon->draw();
        }
        window.clear();
        pollEvents(window);
    } 

    for(HexagonGUI* hexagon : hexagons)
    {
        delete(hexagon);
    }

    return 0;
}