#ifndef HEXAGONGUI_H
#define HEXAGONGUI_H

#include "window.hpp"
#include "text.hpp"
#include <SDL2/SDL_ttf.h>


class HexagonGUI
{
public:
    HexagonGUI(int size, int x, int y, int lastFieldOfTrenchPlayer, const std::string& aTrenchImagePath, const std::string& aMobileImagePath, const std::string& fontPath);
    ~HexagonGUI();

    void draw();

private:
    std::string getLongerLine (std::string& stringToAnalyze);

    int size, x, y;
    mutable int textXOffset, textYOffset;
    SDL_Texture* hexagonTexture = nullptr;
    std::string stringTanks = "T: 0";
    std::string stringFootmen = "P: 0";
    std::string mobileImagePath = nullptr;
    std::string trenchImagePath = nullptr;

public:
    void updateTextString(int nOfTanks, int nOfSoldiers);
    void updateImageToPlayer(int idOfPlayer);
    Text* textElement = nullptr;
};

#endif