#include "hexagonField.hpp"
#include <iostream>

HexagonField::HexagonField(int q, int r, int s, int size, int horizontalOffset, int verticalOffset, int playerIDToAssignTo, std::string trenchImagePath, std::string mobileImagePath, std::string fontPath): q(q), r(r), s(s)
{
    assert (q + r + s == 0);
    idOfPlayerControlling = playerIDToAssignTo;
    this->gui = new HexagonGUI(size, horizontalOffset, verticalOffset, playerIDToAssignTo, trenchImagePath, mobileImagePath, fontPath);
}

HexagonField::HexagonField(int q, int r, int s): q(q), r(r), s(s)
{
    assert (q + r + s == 0);
}

HexagonField HexagonField::hexagonAdd(HexagonField a, HexagonField b)
{
    return HexagonField(a.q + b.q, a.r + b.r, a.s + b.s);
}

void HexagonField::changeOwner(int idOfPlayer)
{
    idOfPlayerControlling = idOfPlayer;
    gui->updateImageToPlayer(idOfPlayerControlling);
}

int HexagonField::getOwner()
{
    return idOfPlayerControlling;
}

void HexagonField::changeUnitNumbers(int tanks, int footmen)
{
    nOfFootmenPresent = footmen;
    nOfTanksPresent = tanks;
    gui->updateTextString(nOfTanksPresent, nOfFootmenPresent);
}

int HexagonField::getNOfFootmen()
{
    return nOfFootmenPresent;
}

int HexagonField::getNOfTanks()
{
    return nOfTanksPresent;
}

void HexagonField::redrawGUI()
{
    gui->draw();
}

HexagonField HexagonField::hexagonSubtract(HexagonField a, HexagonField b)
{
    return HexagonField(a.q - b.q, a.r - b.r, a.s - b.s);
}

HexagonField HexagonField::hexagonMultiply(HexagonField a, int k)
{
    return HexagonField(a.q * k, a.r * k, a.s * k);
}

int HexagonField::hexagonLength(HexagonField hex) {
    return int((abs(hex.q) + abs(hex.r) + abs(hex.s)) / 2);
}

int HexagonField::hexagonDistance(HexagonField a, HexagonField b) {
    return hexagonLength( hexagonSubtract(a, b));
}
