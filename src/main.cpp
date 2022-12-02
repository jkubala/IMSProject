#include <iostream>
#include <unistd.h>
#include <libgen.h>
#include <vector>
#include <unordered_set>
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

void CreateHexagonRow(int nOfHexagons, int rowStartX, int rowStartY, const std::string& imagePath, const std::string& fontPath, std::vector<HexagonGUI*>& hexagons)
{

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

    std::unordered_set <HexagonField> map;
    for (int q = 0; q < N_OF_ROWS; q++)
    {
        verticalOffset = yAnchor + (HEX_SIZE * q);
        for (int r = 0; r < N_OF_COLS; r++)
        {
            // Shift every second column in a row down by half a hexagon
            r % 2 == 1 ? verticalOffset = yAnchor + (q * HEX_SIZE) + colShift : verticalOffset = yAnchor + (q * HEX_SIZE);
            horizontalOffset = xAnchor + (r * (HEX_SIZE * 0.75));
            HexagonGUI* newHexagonGUI = new HexagonGUI(HEX_SIZE, HEX_SIZE, horizontalOffset, verticalOffset, friendlyHexagonPath, fontPath);
            HexagonField newHexagonField = HexagonField(q, r, -q-r);
            newHexagonField.gui = newHexagonGUI;
            map.insert(newHexagonField);
        }
    }

    while(!window.isClosed())
    {
        for(HexagonField hexagonField : map)
        {
            if(hexagonField.gui != nullptr)
            {
                hexagonField.gui->draw();
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

    return 0;
}

