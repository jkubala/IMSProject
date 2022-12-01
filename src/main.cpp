#include <iostream>
#include <unistd.h>
#include <libgen.h>
#include <vector>

#include "window.hpp"
#include "hexagonGUI.hpp"
#include "text.hpp"

// Max characters of the path that leads to png resources
#define PATH_MAX 1024
#define WINDOW_SIZE_X 1920
#define WINDOW_SIZE_Y 1080
#define HEX_SIZE 96
#define N_OF_ROWS 20
#define N_OF_COLS 10
void pollEvents(Window& window) {
    SDL_Event event;

    /* For polling events on multiple objects,
    add it the same way that window is added*/
    if(SDL_PollEvent(&event))
    {
        window.pollEvents(event);
    }
}

void CreateHexagonRow(int nOfHexagons, int rowStartX, int rowStartY, const std::string& imagePath, const std::string& fontPath, std::vector<HexagonGUI*>& hexagons)
{
    int horizontalOffset = rowStartX;
    for(int col = 0; col < nOfHexagons; col++)
    {
        // Offset the next hexagon from starting position 
        horizontalOffset = rowStartX + (HEX_SIZE + (HEX_SIZE / 2)) * col;
        HexagonGUI* newHexagon = new HexagonGUI(HEX_SIZE, HEX_SIZE, horizontalOffset, rowStartY, imagePath, fontPath);
        hexagons.emplace_back(newHexagon);
    }
}

int main(int argc, char **argv)
{
    // Prepare window, anchors so that the map is centered, get png resources and generate hexGrid
    Window window(WINDOW_SIZE_X, WINDOW_SIZE_Y, "World map");
    int xAnchor = (WINDOW_SIZE_X - (N_OF_COLS * (HEX_SIZE + (HEX_SIZE / 2)))) / 2;
    int yAnchor = (WINDOW_SIZE_Y - ((N_OF_ROWS + 1) * HEX_SIZE / 2)) / 2;
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) == NULL)
    {
       std::cerr << "Unable to get current folder!\n";
       return 1;
    }

    std::vector<HexagonGUI*> hexagons;
    const std::string friendlyHexagonPath = std::string(dirname(cwd)) + "/src/resources/hexagonFriendly.png";
    const std::string enemyHexagonPath = std::string(dirname(cwd)) + "/src/resources/hexagonEnemy.png";
    const std::string fontPath = "/home/jankubala/Downloads/IMSProject/src/resources/arial.ttf";

    int horizontalOffset = xAnchor;
    int verticalOffset = yAnchor;
    int rowShift = HEX_SIZE * 0.75f;

    for (int row = 0; row < N_OF_ROWS; row++)
    {
        // Offset vertically by the amount of rows
        verticalOffset = yAnchor + (HEX_SIZE / 2 * row);
        // Every second row, shift hexagons to align with the row above them
        row % 2 == 0 ? horizontalOffset = xAnchor : horizontalOffset += rowShift;
        CreateHexagonRow(10, horizontalOffset, verticalOffset, friendlyHexagonPath, fontPath, hexagons);
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