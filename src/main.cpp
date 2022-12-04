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
float trenchTankToInfantryRatio = 0.5;

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
    }
    else
    {
        hexagonToInsertInto.neighbours[neighbourToInsertInto] = nullptr;
    }
}

void initHexagonNeighbours()
{
    for(HexagonField &hexagon: map)
    {
        //hexagon.changeUnitNumbers(hexagon.q, hexagon.r);
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

void spawnUnits()
{
    /* In the starting hexagons */
    // Player 0
    map[0].changeUnitNumbers(500, 500);
    // Player 1
    map[map.size() - 1].changeUnitNumbers(200,200);
    /* On the frontline */
    // for(HexagonField& hexagon : map)
    // {
    //     for (HexagonField* neighbour : hexagon.neighbours)
    //     {
    //         if(neighbour != nullptr && hexagon.getOwner() != neighbour->getOwner())
    //         {            
    //             if(hexagon.getOwner() == 0)
    //             {
    //                 hexagon.changeUnitNumbers(50, 50);
    //             }
    //             else
    //             {
    //                 hexagon.changeUnitNumbers(20, 20);
    //             }
    //         }
    //     }
    // }
}

unsigned int aiStep(unsigned int interval, void * param)
{
    std::vector <HexagonField> friendlyHexes;
    std::vector <HexagonField> enemyHexes;
    std::vector <HexagonField*> frontline;
    std::cout << "Turn of player:" << playerOnTurn << std::endl;
    int totalAvailableTanks = 0;
    int totalAvailableInfantry = 0;
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
    // Get list of all the friendly territorries that border any one of enemy territories - this gives back the frontline
    for(HexagonField& hexagon : map)
    {
        if(hexagon.getOwner() == playerOnTurn)
        {
            totalAvailableTanks += hexagon.getNOfTanks();
            totalAvailableInfantry += hexagon.getNOfFootmen();
            hexagon.changeUnitNumbers(0,0);
            bool isOnFrontLine = false;
            for (HexagonField* neighbour : hexagon.neighbours)
            {
                if(neighbour != nullptr && neighbour->getOwner() != playerOnTurn)
                {
                    isOnFrontLine = true;
                }
            }
            if(isOnFrontLine)
            {
                HexagonField* hexagonToAdd = &hexagon;
                frontline.push_back(hexagonToAdd); 
            }
        }
    }

    // Compute needed soldiers for each of held territories (at least 30% of enemy units in most populated neighbouring enemy tile)
    // and total number of friendly soldiers
    bool frontlineFound = false;
    //while(!frontlineFound)
    {
        int nOfTanksAvailableForThisFrontline = totalAvailableTanks;
        int nOfInfantryAvailableForThisFrontline = totalAvailableInfantry;
        int totalUnitsRequiredToSecureThisFrontline = 0;
        for(HexagonField* frontlineHexagon : frontline)
        {
            int highestEnemyConcentration = 0;
            for(HexagonField* neighbouringHex : frontlineHexagon->neighbours)
            {
                if(neighbouringHex != nullptr)
                {
                    if(neighbouringHex->getOwner() != playerOnTurn)
                    {
                        int enemyConcentrationOnHex = neighbouringHex->getNOfFootmen() + neighbouringHex->getNOfTanks();
                        if(enemyConcentrationOnHex > highestEnemyConcentration)
                        {
                            highestEnemyConcentration = enemyConcentrationOnHex;
                        }
                    }
                }
            }
            if(highestEnemyConcentration > 0)
            {
                frontlineHexagon->neededAmountOfUnitsToSecure = ceil(highestEnemyConcentration / 2.0);
            }
            else
            {
                // Even if there is no enemy there, put at least 1 unit there
                frontlineHexagon->neededAmountOfUnitsToSecure = 1;
            }
            std::cout << "Units needed to secure" << frontlineHexagon->neededAmountOfUnitsToSecure << std::endl;
            totalUnitsRequiredToSecureThisFrontline += frontlineHexagon->neededAmountOfUnitsToSecure;
        }
        std::cout << "Total units required to secure this frontline: " << totalUnitsRequiredToSecureThisFrontline << std::endl;
        std::cout << "Total tanks: " << totalAvailableTanks << "\nTotalInfantry: " << totalAvailableInfantry << std::endl;

        // If needed soldiers exceed the total number of soldiers, find a backup frontline in friendly territories,
        // if no such frontline can be established, the AI surrenders
        if(totalUnitsRequiredToSecureThisFrontline > totalAvailableInfantry + totalAvailableTanks)
        {
            std::cout << "This is player " << playerOnTurn << ", failed to make a frontline, retrating!" << std::endl;
            //TODO algorithm for backup frontline
        }
        else
        {
            frontlineFound = true;
            std::cout << "This is player " << playerOnTurn << ", frontline established!" << std::endl;
            // Provide territorries with required units
            // Try mix of tanks and infantry, if trench
            if(playerOnTurn == 0)
            {
                // Ideally, tanks should make half of the defending forces
                int idealNumberOfTanksUsed = ceil(totalUnitsRequiredToSecureThisFrontline * trenchTankToInfantryRatio);
                // Get either the desired number of tanks, or less, if there are not as many available
                int actualNumberOfTanksUsed = std::min(idealNumberOfTanksUsed, totalAvailableTanks);
                // Check how many (if any) tanks are missing
                int numberOfTanksThatAreMissing = idealNumberOfTanksUsed - actualNumberOfTanksUsed;

                // Ideally, infantry should make half of the defending forces
                int idealNumberOfInfantryUsed = ceil(totalUnitsRequiredToSecureThisFrontline * (1.0 - trenchTankToInfantryRatio));
                // Get either the desired number of tanks, or less, if there are not as many available
                int actualNumberOfInfantryUsed = std::min(idealNumberOfInfantryUsed, totalAvailableInfantry);
                // Check how many (if any) infantry are missing
                int numberOfInfantryThatIsMissing = idealNumberOfInfantryUsed - actualNumberOfInfantryUsed;

                // Fill either one from the other
                if(numberOfTanksThatAreMissing > 0 )
                {
                    actualNumberOfInfantryUsed += numberOfTanksThatAreMissing;
                }
                else if(numberOfInfantryThatIsMissing > 0)
                {
                    actualNumberOfTanksUsed += numberOfInfantryThatIsMissing;
                }
                
                std::cout << "I will be using " << actualNumberOfTanksUsed << " tanks and " << actualNumberOfInfantryUsed << " infantry" << std::endl;


                assert(actualNumberOfInfantryUsed + actualNumberOfTanksUsed == totalUnitsRequiredToSecureThisFrontline);

                int provisionOfTanksForOneTile = actualNumberOfTanksUsed / floor(frontline.size());
                int remainderOfTanks = actualNumberOfTanksUsed % frontline.size();

                int provisionOfInfantryForOneTile = actualNumberOfInfantryUsed/ floor(frontline.size());
                int remainderOfInfantry = actualNumberOfInfantryUsed % frontline.size();

                for(HexagonField* frontlineHexagon : frontline)
                {
                    int providedTanks = provisionOfTanksForOneTile;
                    int providedInfantry = provisionOfInfantryForOneTile;

                    // Redistribute the remainder
                    if(remainderOfTanks > 0 && providedInfantry + providedTanks != frontlineHexagon->neededAmountOfUnitsToSecure)
                    {
                        providedTanks++;
                        remainderOfTanks--;
                    }
                    if(remainderOfInfantry > 0 && providedInfantry + providedTanks != frontlineHexagon->neededAmountOfUnitsToSecure)
                    {
                        providedInfantry++;
                        remainderOfInfantry--;
                    }

                    totalAvailableInfantry -= providedInfantry;
                    totalAvailableTanks -= providedTanks;

                    // If there is 0 of one thing, or the other, get it from reservers if available
                    if(totalAvailableTanks > 0 && providedTanks == 0)
                    {
                        providedTanks++;
                        totalAvailableTanks--;
                    }
                    if(totalAvailableInfantry > 0 && providedInfantry == 0)
                    {
                        providedInfantry++;
                        totalAvailableInfantry--;
                    }
                    std::cout << "Allocating " << providedTanks << " tanks and " << providedInfantry << " infantry" << std::endl;
                    frontlineHexagon->changeUnitNumbers(providedTanks, providedInfantry);
                }
            }
            // Try infantry only, if mobile
            else
            {
                for(HexagonField* frontlineHexagon : frontline)
                {
                    // Try to get the required infantry from totalInfantry
                    int providedInfantry = std::min(frontlineHexagon->neededAmountOfUnitsToSecure, totalAvailableInfantry);
                    int providedTanks = 0;
                    // If there was allocated less infantry, it means that we have no more infantry
                    if(providedInfantry != frontlineHexagon->neededAmountOfUnitsToSecure)
                    {
                        providedTanks = frontlineHexagon->neededAmountOfUnitsToSecure - providedInfantry;
                    }
                    std::cout << "Allocating " << providedTanks << " tanks and " << providedInfantry << " infantry" << std::endl;
                    frontlineHexagon->changeUnitNumbers(providedTanks, providedInfantry);
                }
            }
        }
    }



    // Step 2: Offensive operations ====================
    // Get all the remaining units
    // Find the spot with the least amount of enemy units
    // If the ratio of remaining units to enemy is at least 125% begin battle
    // If the battle was won, acquire the tile
    // MAYBE If mobile warfare and there are any tanks left, divide them by the number of neighbouring enemy territories and advance
    // into them and do it again from the new tiles up to two times
    // If the enemy units retreated, but there is no enemy neighbouring tile, destroy them


    // Step 3: Afterbattle reorganization
    // Secure the front (step 1), but distribute units evenly if trench

    playerOnTurn = !playerOnTurn;
    Window* window = static_cast<Window*>(param);
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
    spawnUnits();
    SDL_TimerID aiStepTimer = SDL_AddTimer(1000, aiStep, &window);
    while(!window.isClosed())
    {
        pollEvents(window);
    }

    SDL_RemoveTimer(aiStepTimer);

    return 0;
}

