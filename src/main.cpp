#include <iostream>
#include <unistd.h>
#include <libgen.h>
#include <vector>
#include <chrono>
#include <thread>
#include <algorithm>
#include "hexagonField.hpp"
#include "battle.hpp"

// Max characters of the path that leads to png resources
#define PATH_MAX 1024
#define WINDOW_SIZE_X 1370
#define WINDOW_SIZE_Y 830
#define HEX_SIZE 70
#define N_OF_ROWS 10
#define N_OF_COLS 26

std::string trenchHexagonPath;
std::string tankFocusedHexagonPath;
std::vector<HexagonField> map;
std::vector<HexagonField> trenchFocusedHexes;
std::vector<HexagonField> tankFocusedHexes;

bool playerOnTurnID = 0;
bool initialUnitsSpawned = false;
bool simulationOver = false;
// Speificies the lowest possible number of units on a friendly hexagon to a neighbouring enemy hexagon with the most units
// 0.5 means that there must be at least half the number of friendly units, than on a neighbouring enemy hexagon with most units
float friendlyToEnemyRetreatRatio = 0.5;
float trenchTankToInfantryRatio = 0.25;

void pollEvents(Window &window)
{
    SDL_Event event;

    /* For polling events on multiple objects,
    add it the same way that window is added*/
    if (SDL_PollEvent(&event))
    {
        window.pollEvents(event);
    }
}

void redrawGUI(Window *window)
{
    for (HexagonField hexagonField : map)
    {
        hexagonField.redrawGUI();
    }
    window->clear();
}

void getFieldFromMapAndAssignAsNeighbour(HexagonField &hexagonToSearchFor, HexagonField &hexagonToInsertInto, int neighbourToInsertInto)
{
    std::vector<HexagonField>::iterator it;
    it = std::find(map.begin(), map.end(), hexagonToSearchFor);
    if (it != map.end())
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
    for (HexagonField &hexagon : map)
    {
        // Uncomment for displaying Q and R coordinates instead of units on hex grid. Do not forget to comment out the AI timer in that case
        // hexagon.changeUnitNumbers(hexagon.q, hexagon.r);
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

void updateFrontlineAndUnitCount(std::vector<HexagonField *> &frontline, int &totalTanks, int &totalInfantry)
{
    frontline.clear();
    for (HexagonField &hexagon : map)
    {
        if (hexagon.getOwner() == playerOnTurnID)
        {
            totalTanks += hexagon.getNOfTanks();
            totalInfantry += hexagon.getNOfFootmen();
            bool isOnFrontLine = false;
            for (HexagonField *neighbour : hexagon.neighbours)
            {
                if (neighbour != nullptr && neighbour->getOwner() != playerOnTurnID)
                {
                    isOnFrontLine = true;
                }
            }
            if (isOnFrontLine)
            {
                HexagonField *hexagonToAdd = &hexagon;
                frontline.push_back(hexagonToAdd);
            }
        }
    }
}

int setNeededUnitsForAllFrontlineHexesAndReturnTotal(std::vector<HexagonField *> &frontline)
{
    int totalUnitsRequiredToSecureThisFrontline = 0;
    for (HexagonField *frontlineHexagon : frontline)
    {
        float highestEnemyConcentration = 0;
        for (HexagonField *neighbouringHex : frontlineHexagon->neighbours)
        {
            if (neighbouringHex != nullptr)
            {
                if (neighbouringHex->getOwner() != playerOnTurnID)
                {
                    int enemyConcentrationOnHex = neighbouringHex->getNOfFootmen() + neighbouringHex->getNOfTanks();
                    if (enemyConcentrationOnHex > highestEnemyConcentration)
                    {
                        highestEnemyConcentration = enemyConcentrationOnHex;
                    }
                }
            }
        }
        if (highestEnemyConcentration > 0)
        {
            frontlineHexagon->neededAmountOfUnitsToSecure = ceil(highestEnemyConcentration * friendlyToEnemyRetreatRatio);
        }
        else
        {
            // Even if there are no enemies on the neighbouring enemy hexagons, put at least 1 unit there
            frontlineHexagon->neededAmountOfUnitsToSecure = 1;
        }
        totalUnitsRequiredToSecureThisFrontline += frontlineHexagon->neededAmountOfUnitsToSecure;
    }
    return totalUnitsRequiredToSecureThisFrontline;
}

void secureFrontlineEvenlyWithAllUnits(std::vector<HexagonField *> &frontline)
{
    int totalNumberOfTanks = 0;
    int totalNumberOfInfantry = 0;
    updateFrontlineAndUnitCount(frontline, totalNumberOfTanks, totalNumberOfInfantry);
    // Clear all frontline hexes first once the soldiers are counted
    for (HexagonField *hexagon : frontline)
    {
        hexagon->changeUnitNumbers(0, 0);
    }
    setNeededUnitsForAllFrontlineHexesAndReturnTotal(frontline);

    int tanksPerHexagon = totalNumberOfTanks / frontline.size();
    int tanksRemainder = totalNumberOfTanks % frontline.size();
    int infantryPerHexagon = totalNumberOfInfantry / frontline.size();
    int infantryRemainder = totalNumberOfInfantry % frontline.size();

    for (HexagonField *hexagon : frontline)
    {
        int numberOfTanksToAdd = tanksPerHexagon;
        int numberOfInfantryToAdd = infantryPerHexagon;
        if (tanksRemainder > 0)
        {
            numberOfTanksToAdd++;
            tanksRemainder--;
        }
        if (infantryRemainder > 0)
        {
            numberOfInfantryToAdd++;
            infantryRemainder--;
        }
        hexagon->changeUnitNumbers(numberOfTanksToAdd, numberOfInfantryToAdd);
    }
}

void secureFrontlineWithMinimumOfUnits(std::vector<HexagonField *> &frontline, int &tanksLeftForAttack, int &infantryLeftForAttack)
{
    int totalAvailableTanks = 0;
    int totalAvailableInfantry = 0;

    updateFrontlineAndUnitCount(frontline, totalAvailableTanks, totalAvailableInfantry);

    // Compute needed soldiers for each of held territories (at least 30% of enemy units in most populated neighbouring enemy tile)
    // and total number of friendly soldiers
    bool frontlineFound = false;
    float totalUnitsRequiredToSecureThisFrontline = 0;
    // while(!frontlineFound) =============================================================================== UNCOMMENT AFTER IMPLEMENTING BACKUP FRONTLINE ALGORITHM
    {
        totalUnitsRequiredToSecureThisFrontline = setNeededUnitsForAllFrontlineHexesAndReturnTotal(frontline);
        std::cout << "Total units required to secure this frontline: " << totalUnitsRequiredToSecureThisFrontline << std::endl;
        std::cout << "Total tanks: " << totalAvailableTanks << "\nTotalInfantry: " << totalAvailableInfantry << std::endl;

        // If needed soldiers exceed the total number of soldiers, find a backup frontline in friendly territories,
        // if no such frontline can be established, the AI surrenders
        if (totalUnitsRequiredToSecureThisFrontline > totalAvailableInfantry + totalAvailableTanks)
        {
            std::cout << "This is player " << playerOnTurnID << ", failed to make a frontline, retrating!" << std::endl;
            secureFrontlineEvenlyWithAllUnits(frontline); // PLACEHOLDER FOR THE ALGORITHM. REMOVE AFTER BACKUP RETREATING IMPLEMENTED
            return;
            // Algorithm for backup frontline TODO Honza Brudný
            //  Try to find a frontline with the length of frontline.size() -1

            // Switch owner of all the hexagons that have been lost - the new frontline that is calculated in spreadAllUnitsEvenly... is made by checking owner of all hexagons

            // Spread units evenly across the new frontline
            // secureFrontlineEvenlyWithAllUnits(frontline);

            // Spread enemy units evenly across the new frontline
            playerOnTurnID = !playerOnTurnID;
            // secureFrontlineEvenlyWithAllUnits(frontline);
            playerOnTurnID = !playerOnTurnID;

            /* The while loop repeats until an acceptable frontline is found, or the frontline
            length reaches 0 and the AI will surrender, so make something indicating the second option to the main to close the program */
        }
        else
        {
            frontlineFound = true;
            std::cout << "This is player " << playerOnTurnID << ", frontline established!" << std::endl;
        }
    }

    // Spreading units in either trench, or tank focused AI's prefference across frontline
    // Try mix of tanks and infantry, if trench
    if (playerOnTurnID == 0)
    {
        // Ideally, tanks should make tankToInfantryRatio of the defending forces
        int idealNumberOfTanksUsed = ceil(totalUnitsRequiredToSecureThisFrontline * trenchTankToInfantryRatio);
        // Get either the desired number of tanks, or less, if there are not as many available
        int actualNumberOfTanksUsed = std::min(idealNumberOfTanksUsed, totalAvailableTanks);
        // Check how many (if any) tanks are missing
        int numberOfTanksThatAreMissing = idealNumberOfTanksUsed - actualNumberOfTanksUsed;

        // Ideally, infantry should make 1 - tankToInfantryRatio of the defending forces
        int idealNumberOfInfantryUsed = ceil(totalUnitsRequiredToSecureThisFrontline * (1.0 - trenchTankToInfantryRatio));
        // Get either the desired number of tanks, or less, if there are not as many available
        int actualNumberOfInfantryUsed = std::min(idealNumberOfInfantryUsed, totalAvailableInfantry);
        // Check how many (if any) infantry are missing
        int numberOfInfantryThatIsMissing = idealNumberOfInfantryUsed - actualNumberOfInfantryUsed;

        // Fill either one from the other
        if (numberOfTanksThatAreMissing > 0)
        {
            if (totalAvailableInfantry - (actualNumberOfInfantryUsed + numberOfTanksThatAreMissing) < 0)
            {
                std::cerr << "ERROR: TRENCH BASED AI DID NOT HAVE ENOUGH INFANTRY FOR SECURING FRONTLINE, EVEN THOUGH IT PASSED CHECK FOR IT";
                return;
            }
            actualNumberOfInfantryUsed += numberOfTanksThatAreMissing;
        }
        else if (numberOfInfantryThatIsMissing > 0)
        {
            if (totalAvailableTanks - (actualNumberOfTanksUsed + numberOfInfantryThatIsMissing) < 0)
            {
                std::cerr << "ERROR: TRENCH BASED AI DID NOT HAVE ENOUGH TANKS FOR SECURING FRONTLINE, EVEN THOUGH IT PASSED CHECK FOR IT";
                return;
            }
            actualNumberOfTanksUsed += numberOfInfantryThatIsMissing;
        }

        std::cout << "I will be using " << actualNumberOfTanksUsed << " tanks and " << actualNumberOfInfantryUsed << " infantry" << std::endl;

        int provisionOfTanksForOneTile = actualNumberOfTanksUsed / frontline.size();
        int remainderOfTanks = actualNumberOfTanksUsed % frontline.size();

        int provisionOfInfantryForOneTile = actualNumberOfInfantryUsed / frontline.size();
        int remainderOfInfantry = actualNumberOfInfantryUsed % frontline.size();

        for (HexagonField *frontlineHexagon : frontline)
        {
            int providedTanks = provisionOfTanksForOneTile;
            int providedInfantry = provisionOfInfantryForOneTile;

            // Redistribute the remainder
            if (remainderOfTanks > 0 && providedInfantry + providedTanks != frontlineHexagon->neededAmountOfUnitsToSecure)
            {
                providedTanks++;
                remainderOfTanks--;
            }
            if (remainderOfInfantry > 0 && providedInfantry + providedTanks != frontlineHexagon->neededAmountOfUnitsToSecure)
            {
                providedInfantry++;
                remainderOfInfantry--;
            }

            // Subtract the given resources from the total reserves
            totalAvailableInfantry -= providedInfantry;
            totalAvailableTanks -= providedTanks;

            // If there is 0 of one thing, or the other, get it from reserves if available
            if (totalAvailableTanks > 0 && providedTanks == 0)
            {
                providedTanks++;
                totalAvailableTanks--;
            }
            if (totalAvailableInfantry > 0 && providedInfantry == 0)
            {
                providedInfantry++;
                totalAvailableInfantry--;
            }
            std::cout << "Allocating " << providedTanks << " tanks and " << providedInfantry << " infantry" << std::endl;
            frontlineHexagon->changeUnitNumbers(providedTanks, providedInfantry);
        }
    }
    // Try infantry only, if tank focused (tanks are saved for attacks)
    else
    {
        for (HexagonField *frontlineHexagon : frontline)
        {
            // Try to get the required infantry from totalInfantry
            int providedInfantry = std::min(frontlineHexagon->neededAmountOfUnitsToSecure, totalAvailableInfantry);
            int providedTanks = 0;
            // If there was allocated less infantry, it means that we have no more infantry
            if (providedInfantry != frontlineHexagon->neededAmountOfUnitsToSecure)
            {
                providedTanks = frontlineHexagon->neededAmountOfUnitsToSecure - providedInfantry;
            }
            if (totalAvailableTanks - providedTanks < 0)
            {
                std::cerr << "ERROR: TANK FOCUSED AI DID NOT HAVE ENOUGH TANKS TO ALLOCATE TO FRONTLINE, BUT PASSED THE CHECK OF HAVING ENOUGH UNITS\n"
                          << std::endl;
                frontlineHexagon->changeUnitNumbers(0, 0);
                return;
            }

            totalAvailableInfantry -= providedInfantry;
            totalAvailableTanks -= providedTanks;
            std::cout << "Allocating " << providedTanks << " tanks and " << providedInfantry << " infantry" << std::endl;
            frontlineHexagon->changeUnitNumbers(providedTanks, providedInfantry);
        }
    }

    tanksLeftForAttack = totalAvailableTanks;
    infantryLeftForAttack = totalAvailableInfantry;
    std::cout << "This is player " << playerOnTurnID << ", I have " << tanksLeftForAttack << " tanks left and " << infantryLeftForAttack << " infantry left" << std::endl;
}

void spawnUnits(std::vector<HexagonField *> &frontline)
{
    // Trench focused AI
    map[0].changeUnitNumbers(150, 500);
    secureFrontlineEvenlyWithAllUnits(frontline);
    map[0].changeUnitNumbers(0, 0);
    // Tank focused AI
    playerOnTurnID = 1;
    map[map.size() - 1].changeUnitNumbers(350, 200);
    secureFrontlineEvenlyWithAllUnits(frontline);
    map[map.size() - 1].changeUnitNumbers(0, 0);

    playerOnTurnID = 0;
}

unsigned int aiStep(unsigned int interval, void *param)
{
    Window *window = static_cast<Window *>(param);
    if (window->change)
    {
        return interval;
    }
    std::vector<HexagonField *> frontline;
    if (!initialUnitsSpawned)
    {
        spawnUnits(frontline);
        initialUnitsSpawned = true;
    }
    std::cout << "Turn of player:" << playerOnTurnID << std::endl;
    // Step 1: Securing the front =====================
    int tanksRemainingForAttack = 0;
    int infantryRemainingForAttack = 0;
    secureFrontlineWithMinimumOfUnits(frontline, tanksRemainingForAttack, infantryRemainingForAttack);

    if (frontline.size() == 0)
    {
        printf("player %d lost\n", playerOnTurnID);
        simulationOver = true;
        return interval;
    }
    if (playerOnTurnID == 0)
    {
        map[0].changeUnitNumbers(tanksRemainingForAttack, infantryRemainingForAttack);
    }
    else
    {
        map[map.size() - 1].changeUnitNumbers(tanksRemainingForAttack, infantryRemainingForAttack);
    }

    // Step 2: Offensive operations ==================== TODO Honza Kubala
    // Find the spot with the least amount of enemy units
    // If the ratio of remaining units to enemy is at least 125% begin battle
    // If the battle was won, acquire the tile
    // If the enemy units retreated, but there is no enemy neighbouring tile, destroy them
    // If there are any units on neighbouring tile, which are now surrounded only by friendly tiles, move those to the newly conquered tile (so that they do not stay on a non-frontline tile)
    // If tank focused and there are any tanks left, divide them by the number of neighbouring enemy territories and advance into them and do it one last time from the those tiles
    std::vector<HexagonField *> enemyfrontline;
    HexagonField *toAttack = nullptr;
    for (HexagonField &hexagon : map)
    {
        if (hexagon.getOwner() != playerOnTurnID)
        {
            bool isOnFrontLine = false;
            for (HexagonField *neighbour : hexagon.neighbours)
            {
                if (neighbour != nullptr && neighbour->getOwner() == playerOnTurnID)
                {
                    isOnFrontLine = true;
                }
            }
            if (isOnFrontLine)
            {
                if (toAttack == nullptr)
                {
                    toAttack = &hexagon;
                }
                else if (toAttack->getNOfFootmen() + toAttack->getNOfTanks() > hexagon.getNOfFootmen() + hexagon.getNOfTanks())
                {
                    toAttack = &hexagon;
                }

                HexagonField *hexagonToAdd = &hexagon;
                enemyfrontline.push_back(hexagonToAdd);
            }
        }
    }

    if (playerOnTurnID == 0)
    {
        map[0].changeUnitNumbers(0, 0);
    }
    else
    {
        map[map.size() - 1].changeUnitNumbers(0, 0);
    }

    battle_result result = battle::result(tanksRemainingForAttack, infantryRemainingForAttack, toAttack->getNOfTanks(), toAttack->getNOfFootmen());

    if (result.attackers_won)
    {
        bool attackers_unit_placed = false;
        toAttack->changeOwner(playerOnTurnID);
        toAttack->changeUnitNumbers(result.attack_t, result.attack_p);
        for (HexagonField *neighbour : toAttack->neighbours)
        {
            if (neighbour != nullptr && neighbour->getOwner() != playerOnTurnID)
            {
                neighbour->changeUnitNumbers(neighbour->getNOfTanks() + result.defend_t, neighbour->getNOfFootmen() + result.defend_p);
                break;
            }
        }
        bool allfriendly = true;
        for (HexagonField *neighbour : toAttack->neighbours)
        {
            if (neighbour != nullptr && neighbour->getOwner() != playerOnTurnID)
            {
                allfriendly = false;
                break;
            }
        }
        if (allfriendly)
        {
            for (HexagonField *hexagon : frontline)
            {
                bool break_this = false;
                for (HexagonField *neighbour : hexagon->neighbours)
                {
                    if (neighbour != nullptr && neighbour->getOwner() != playerOnTurnID)
                    {
                        hexagon->changeUnitNumbers(toAttack->getNOfTanks()+hexagon->getNOfTanks(), toAttack->getNOfFootmen() + hexagon->getNOfFootmen());
                        toAttack->changeUnitNumbers(0, 0);
                        attackers_unit_placed = true;
                        break_this = true;
                        break;
                    }
                    if (break_this)
                    {
                        break;
                    }
                    
                }
            }
        }
        for (HexagonField *neighbour : toAttack->neighbours)
        {
            if (neighbour != nullptr && neighbour->getOwner() == playerOnTurnID)
            {
                allfriendly = true;
                for (HexagonField *neighbourfriendly : neighbour->neighbours)
                {
                    if (neighbourfriendly != nullptr && neighbourfriendly->getOwner() != playerOnTurnID)
                    {
                        allfriendly = false;
                        break;
                    }
                }
                if (allfriendly && !attackers_unit_placed)
                {
                    attackers_unit_placed = true;
                    toAttack->changeUnitNumbers(toAttack->getNOfTanks() + neighbour->getNOfTanks(),
                                                toAttack->getNOfFootmen() + neighbour->getNOfFootmen());
                    neighbour->changeUnitNumbers(0, 0);
                }
            }
        }
    }
    else
    {
        toAttack->changeUnitNumbers(result.defend_t, result.defend_p);
        for (HexagonField *neighbour : toAttack->neighbours)
        {
            if (neighbour != nullptr && neighbour->getOwner() == playerOnTurnID)
            {
                neighbour->changeUnitNumbers(neighbour->getNOfTanks() + result.attack_t, neighbour->getNOfFootmen() + result.attack_p);
                break;
            }
        }
    }

    playerOnTurnID = !playerOnTurnID;
    redrawGUI(window);

    return interval;
}

int main(int argc, char **argv)
{
    // Prepare window, anchors so that the map is centered, get png resources and generate hexGrid
    Window window(WINDOW_SIZE_X, WINDOW_SIZE_Y, "World map");
    int xAnchor = (WINDOW_SIZE_X - (HEX_SIZE * 0.75f * (N_OF_COLS + 1.0f / 3.0f))) / 2;
    int yAnchor = (WINDOW_SIZE_Y - ((N_OF_ROWS + 0.5f) * HEX_SIZE)) / 2;

    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) == NULL)
    {
        std::cerr << "Unable to get current folder!\n";
        return 1;
    }
    std::string localFolderPath = std::string(dirname(cwd));

    trenchHexagonPath = localFolderPath + "/src/resources/hexagonFriendly.png";
    tankFocusedHexagonPath = localFolderPath + "/src/resources/hexagonEnemy.png";
    const std::string fontPath = localFolderPath + "/src/resources/arial.ttf";
    int horizontalOffset = xAnchor;
    int verticalOffset = yAnchor;
    int colShift = HEX_SIZE * 0.5;
    int lastColOfTrenchPlayer = N_OF_COLS / 2;
    int playerIDToAssignTo = 0;
    for (int q = 0; q < N_OF_COLS; q++)
    {
        horizontalOffset = xAnchor + (HEX_SIZE * 0.75 * q);

        int colshift = q % 2 == 0 ? 0 : HEX_SIZE * 0.5;
        int qOffset = floor(q / 2.0);
        for (int r = 0 - qOffset, rGridPos = 0; r < N_OF_ROWS - qOffset; r++, rGridPos++)
        {

            if ((q + 1) > lastColOfTrenchPlayer)
            {
                playerIDToAssignTo = 1;
            }
            else
            {
                playerIDToAssignTo = 0;
            }
            verticalOffset = yAnchor + (HEX_SIZE * rGridPos) + colshift;
            HexagonField newHexagonField(q, r, -q - r, HEX_SIZE, horizontalOffset, verticalOffset, playerIDToAssignTo, trenchHexagonPath, tankFocusedHexagonPath, fontPath);
            if (playerIDToAssignTo == 0)
            {
                trenchFocusedHexes.push_back(newHexagonField);
            }
            else
            {
                tankFocusedHexes.push_back(newHexagonField);
            }
            map.push_back(newHexagonField);
        }
    }

    initHexagonNeighbours();
    SDL_TimerID aiStepTimer = SDL_AddTimer(100, aiStep, &window);
    while (!window.isClosed())
    {
        if (simulationOver)
        {
            window.change = true;
            break;
        }
        pollEvents(window);
    }

    SDL_RemoveTimer(aiStepTimer);
    redrawGUI(&window);
    while (window.change && !window.isClosed())
    {
        pollEvents(window);
    }

    return 0;
}
