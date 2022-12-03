#include "hexagonGUI.hpp"
#include <iostream>
#include <sstream>

HexagonGUI::HexagonGUI(int size, int x, int y, int lastFieldOfTrenchPlayer, const std::string& aTrenchImagePath, const std::string& aMobileImagePath, const std::string& fontPath) : size(size), x(x), y(y)
{
    trenchImagePath = aTrenchImagePath;
    mobileImagePath = aMobileImagePath;
    updateImageToPlayer(lastFieldOfTrenchPlayer);
    std::string longerLine = stringTanks > stringFootmen ? stringTanks : stringFootmen;
    int fontSize = size /longerLine.length();
    textElement = new Text(Window::renderer, fontPath, fontSize, stringTanks + "\n" + stringFootmen);
}

void HexagonGUI::updateImageToPlayer(int idOfPlayer)
{
    if(hexagonTexture != nullptr)
    {
        SDL_DestroyTexture(hexagonTexture);
    }
    SDL_Surface* surface = nullptr;
    if(idOfPlayer == 0)
    {
        surface = IMG_Load(trenchImagePath.c_str());   
    }
    else
    {
        surface = IMG_Load(mobileImagePath.c_str());   
    }
    if(!surface)
    {
        std::cerr << "Failed to create surface! " << trenchImagePath.c_str() << "\nError: " << IMG_GetError() <<  std::endl;
    }

    hexagonTexture = SDL_CreateTextureFromSurface(Window::renderer, surface);
    if(!hexagonTexture)
    {
        std::cerr << "Failed to create texture!\n";
    }
    SDL_FreeSurface(surface);
}

HexagonGUI::~HexagonGUI()
{
    SDL_DestroyTexture(hexagonTexture);
}

void HexagonGUI::updateTextString(int nOfTanks, int nOfSoldiers)
{
    std::string newStringTanks = std::string("T: ") + std::to_string(nOfTanks);
    std::string newStringFootmen = std::string("P: ") + std::to_string(nOfSoldiers);
    if(stringTanks != newStringTanks || stringFootmen != newStringFootmen)
    {
        stringTanks = newStringTanks;
        stringFootmen = newStringFootmen;
        textElement->UpdateText(stringTanks + "\n" + stringFootmen);
    }
}

void HexagonGUI::draw()
{
    int stringWidth = 0;
    int stringHeight = 0;
    std::string longerLine = stringTanks > stringFootmen ? stringTanks : stringFootmen;
    TTF_SizeText(textElement->font, longerLine.c_str(), &stringWidth, &stringHeight);
    textXOffset = x + ((size  - stringWidth) / 2);
    // Multiply by two to account for the two strings being above each other
    textYOffset = y + ((size - (stringHeight * 2)) / 2);
    SDL_Rect rect = { x, y, size, size};

    if(hexagonTexture != nullptr)
    {
        SDL_RenderCopy(Window::renderer, hexagonTexture, nullptr, &rect);
        textElement->display(Window::renderer, textXOffset, textYOffset);
    }
}