#include "hexagonGUI.hpp"
#include <iostream>
#include <sstream>

HexagonGUI::HexagonGUI(int w, int h, int x, int y, const std::string& imagePath, const std::string& fontPath) : w(w), h(h), x(x), y(y)
{
    SDL_Surface* surface = IMG_Load(imagePath.c_str());
    if(!surface)
    {
        std::cerr << "Failed to create surface! " << imagePath.c_str() << "\nError: " << IMG_GetError() <<  std::endl;
    }

    hexagonTexture = SDL_CreateTextureFromSurface(Window::renderer, surface);
    if(!hexagonTexture)
    {
        std::cerr << "Failed to create texture!\n";
    }
    SDL_FreeSurface(surface);

    int fontSize = h / getLongerLine(stringWithText).length();
    textElement = new Text(Window::renderer, fontPath, fontSize, stringWithText, {255, 255, 255, 255});
}

HexagonGUI::~HexagonGUI()
{
    SDL_DestroyTexture(hexagonTexture);
}

std::string HexagonGUI::getLongerLine (std::string& stringToAnalyze)
{
    std::stringstream ss(stringToAnalyze);
    std::string currentString;
    std::string longestString = "";
    while (std::getline(ss, currentString, '\n')) {
        if(strlen(currentString.c_str()) > strlen(longestString.c_str()))
        {
            longestString = currentString;
        }
    }
    return longestString;
}

void updateTextString(int nOfTanks, int nOfSoldiers)
{

}

void HexagonGUI::draw()
{
    int stringWidth = 0;
    int stringHeight = 0;
    TTF_SizeText(textElement->font, getLongerLine(stringWithText).c_str(), &stringWidth, &stringHeight);
    textXOffset = x + ((w  - stringWidth) / 2);
    // Multiply by two to account for the two strings being above each other
    textYOffset = y + ((h - (stringHeight * 2)) / 2);
    SDL_Rect rect = { x, y, w, h};

    if(hexagonTexture)
    {
        SDL_RenderCopy(Window::renderer, hexagonTexture, nullptr, &rect);
        textElement->display(Window::renderer, textXOffset, textYOffset);
    }
}