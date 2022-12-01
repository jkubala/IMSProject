#ifndef HEXAGONGUI_H
#define HEXAGONGUI_H

#include "window.hpp"
#include "text.hpp"
#include <SDL2/SDL_ttf.h>


class HexagonGUI
{
public:
    HexagonGUI(int w, int h, int x, int y, const std::string& imagePath, const std::string& fontPath);
    ~HexagonGUI();

    void draw();

private:
    std::string getLongerLine (std::string& stringToAnalyze);

    int w, h, x, y;
    mutable int textXOffset, textYOffset;
    SDL_Texture* hexagonTexture = nullptr;
    std::string stringWithText = "T: 10\nP: 100";

public:
    void updateTextString(int nOfTanks, int nOfSoldiers);
    Text* textElement = nullptr;
};
#endif