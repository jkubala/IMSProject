#include <iostream>
#include <unistd.h>
#include <libgen.h>
#include <vector>
#include <chrono>
#include <thread>
#include <algorithm>
#include "hexagonField.hpp"

// Max characters of the path that leads to png resources
#define PATH_MAX 1024
#define WINDOW_SIZE_X 1920
#define WINDOW_SIZE_Y 1080
#define HEX_SIZE 96
#define N_OF_ROWS 10
#define N_OF_COLS 26

std::string trenchHexagonPath;
std::string mobileHexagonPath;
std::vector <HexagonField> map;
std::vector <HexagonField> mobileHexes;
std::vector <HexagonField> trenchHexes;

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
    for(HexagonField hexagonField : map)
    {
        hexagonField.redrawGUI();
    }
    window->clear();
}

void getFieldFromMapAndAssignAsNeighbour(HexagonField& hexagonToSearchFor, HexagonField& hexagonToInsertInto, int neighbourToInsertInto)
{
    std::vector<HexagonField>::iterator it;
    it = std::find(map.begin(), map.end(), hexagonToSearchFor);
    if(it != map.end())
    {
        hexagonToInsertInto.neighbours[neighbourToInsertInto] = &map[it - map.begin()];
        std::cout << "INSERTING THIS" << std::endl;
        std::cout << map[it - map.begin()].q << std::endl;
        std::cout << map[it - map.begin()].r << std::endl;
        std::cout << map[it - map.begin()].s << std::endl;
    }
}

void initHexagonNeighbours()
{
    for(HexagonField hexagon: map)
    {
        hexagon.changeUnitNumbers(hexagon.q, hexagon.r);
        HexagonField hexagonNeighbourTop(hexagon.q, hexagon.r - 1, hexagon.s + 1);
        HexagonField hexagonNeighbourRightTop(hexagon.q + 1, hexagon.r - 1, hexagon.s);
        HexagonField hexagonNeighbourRightDown(hexagon.q + 1, hexagon.r, hexagon.s - 1);
        HexagonField hexagonNeighbourDown(hexagon.q, hexagon.r + 1, hexagon.s - 1);
        HexagonField hexagonNeighbourLeftDown(hexagon.q - 1, hexagon.r + 1, hexagon.s);
        HexagonField hexagonNeighbourLeftTop(hexagon.q - 1, hexagon.r, hexagon.s + 1);

        getFieldFromMapAndAssignAsNeighbour(hexagonNeighbourTop, hexagon, 0);
        getFieldFromMapAndAssignAsNeighbour(hexagonNeighbourRightTop, hexagon, 1);
        getFieldFromMapAndAssignAsNeighbour(hexagonNeighbourRightDown, hexagon, 2);
        getFieldFromMapAndAssignAsNeighbour(hexagonNeighbourDown, hexagon, 3);
        getFieldFromMapAndAssignAsNeighbour(hexagonNeighbourLeftDown, hexagon, 4);
        getFieldFromMapAndAssignAsNeighbour(hexagonNeighbourLeftTop, hexagon, 5);
    }
}

unsigned int aiStep(unsigned int interval, void * param)
{
    std::vector <HexagonField> friendlyHexes;
    std::vector <HexagonField> enemyHexes;
    std::cout << "Turn of player:" << playerOnTurn << std::endl;
    if(playerOnTurn == 0)
    {
        friendlyHexes = trenchHexes;
        enemyHexes = mobileHexes;
    }
    else
    {
        friendlyHexes = mobileHexes;
        enemyHexes = trenchHexes;
    }
    // Step 1: Securing the front =====================
    // Get list of all the enemy territorries that border any one of them - this gives back the frontline

    // Compute needed soldiers for each of held territories (at least 30% of enemy units harboring this territory)
    // and total number of friendly soldiers

    // If needed soldiers exceed the total number of soldiers, find a backup frontline in friendly territories,
    // if no such frontline can be established, the AI surrenders

    // Provide territorries with required units (prefferably soldiers, if tank focused, even distribution of soldiers and tanks otherwise)

    // Step 2: Offensive operations ====================
    // Get all the remaining units
    // Find the spot with the least amount of enemy units
    // If the ratio of remaining units + units from the secured bordering friendly tiles to enemy is at least 125% begin battle


    // Step 3: Afterbattle reorganization
    // If the battle was won, acquire the tile and put one unit on it (reduce remainingUnits by 1)
    // MAYBE If mobile warfare and there are any tanks left, divide them by the number of neighbouring enemy territories and advance
    // into them and do it again from the new tiles up to two times
    // If the enemy units retreated, but there is no enemy neighbouring tile, destroy them
    // Secure the front (step 1), but distribute units evenly if trench



    Window* window = static_cast<Window*>(param);
    if(!window->change)
    {
        //map[21].changeUnitNumbers(0, 0);
        //std::cout << map[35].neighbours[3]->q << std::endl;
        map[21].changeOwner(0);
        //map[21].neighbours[1]->changeUnitNumbers(0, 0);
        //map[21].neighbours[0]->changeOwner(0);
    }
    else
    {
        //map[21].changeUnitNumbers(10, 10);
        map[21].changeOwner(1);
        //map[21].neighbours[1]->changeUnitNumbers(10, 10);
        map[21].neighbours[0]->changeOwner(1);
    }

    playerOnTurn = !playerOnTurn;
    redrawGUI(window);
    
    return interval;
}

int main(int argc, char **argv)
{
    // Prepare window, anchors so that the map is centered, get png resources and generate hexGrid
    Window window(WINDOW_SIZE_X, WINDOW_SIZE_Y, "World map");
    int xAnchor = (WINDOW_SIZE_X - (HEX_SIZE * 0.75f * (N_OF_COLS + 1.0f/3.0f))) / 2;
    int yAnchor = (WINDOW_SIZE_Y - ((N_OF_ROWS + 0.5f) * HEX_SIZE)) / 2;
    
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) == NULL)
    {
       std::cerr << "Unable to get current folder!\n";
       return 1;
    }
    std::string localFolderPath = std::string(dirname(cwd));

    trenchHexagonPath = localFolderPath + "/src/resources/hexagonFriendly.png";
    mobileHexagonPath = localFolderPath + "/src/resources/hexagonEnemy.png";
    const std::string fontPath =  localFolderPath + "/src/resources/arial.ttf";
    int horizontalOffset = xAnchor;
    int verticalOffset = yAnchor;
    int colShift = HEX_SIZE * 0.5;
    int lastColOfTrenchPlayer = N_OF_COLS / 2;
    int playerIDToAssignTo = 0;
    for (int q = 0; q < N_OF_COLS; q++)
    {   
        horizontalOffset = xAnchor + (HEX_SIZE * 0.75 * q);
        
        int colshift = q % 2 == 0 ? 0 : HEX_SIZE * 0.5; 
        int qOffset = floor(q/2.0);
        for (int r = 0 - qOffset,  rGridPos = 0; r < N_OF_ROWS - qOffset; r++, rGridPos++)
        {
            
            if((q+1) > lastColOfTrenchPlayer)
            {
                playerIDToAssignTo = 1;
            }
            else
            {
                playerIDToAssignTo = 0;
            }
            verticalOffset = yAnchor + (HEX_SIZE * rGridPos) + colshift;
            HexagonField newHexagonField(q, r, -q-r, HEX_SIZE, horizontalOffset, verticalOffset, playerIDToAssignTo, trenchHexagonPath, mobileHexagonPath, fontPath);
            if(playerIDToAssignTo == 0)
            {
                trenchHexes.push_back(newHexagonField);
            }
            else
            {
                mobileHexes.push_back(newHexagonField);
            }
            map.push_back(newHexagonField);
        }
    }

    initHexagonNeighbours();
    SDL_TimerID aiStepTimer = SDL_AddTimer(1000, aiStep, &window);
    while(!window.isClosed())
    {
        pollEvents(window);
    }

    SDL_RemoveTimer(aiStepTimer);

    return 0;
}

