#ifndef HEXAGON_H
#define HEXAGON_H
#include <assert.h>
#include <vector>
#include "hexagonGUI.hpp"

class HexagonField
{
public:
    int q, r, s;
    HexagonField(int q, int r, int s);
    HexagonField(int q, int r, int s, int size, int horizontalOffset, int verticalOffset, int playerIDToAssignTo, std::string trenchImagePath, std::string mobileImagePath, std::string fontPath);
    HexagonField hexagonAdd(HexagonField a, HexagonField b);
    HexagonField hexagonSubtract(HexagonField a, HexagonField b);
    HexagonField hexagonMultiply(HexagonField a, int k);
    int hexagonLength(HexagonField hex);
    int hexagonDistance(HexagonField a, HexagonField b);
    HexagonField hexagonDirection(int direction /* 0 to 5 */);
    HexagonField hexagonNeighbor(HexagonField hex, int direction);

    static const std::vector<HexagonField> hexagonDirections;

    bool operator == (const HexagonField comparedHex) const
    {
        return this->q == comparedHex.q && this->r == comparedHex.r && this->s == comparedHex.s;
    }
    bool operator != (const HexagonField comparedHex) const
    {
        return !(*this == comparedHex);
    }

    HexagonField* neighbours[6];

    int getOwner();
    void changeOwner(int idOfPlayer);
    void changeUnitNumbers(int tanks, int footmen);
    int getNOfTanks();
    int getNOfFootmen();
    void redrawGUI();
    int neededAmountOfUnitsToSecure = 0;
private:
    HexagonGUI* gui = nullptr;
    int idOfPlayerControlling = 0;
    int nOfTanksPresent = 0;
    int nOfFootmenPresent = 0;
};
#endif