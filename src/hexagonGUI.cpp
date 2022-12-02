#include "hexagonGUI.hpp"
#include <iostream>
#include <sstream>

HexagonGUI::HexagonGUI(int size, int x, int y, const std::string& imagePath, const std::string& fontPath) : size(size), x(x), y(y)
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

    int fontSize = size / getLongerLine(stringWithText).length();
    textElement = new Text(Window::renderer, fontPath, fontSize, stringWithText);
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

void HexagonGUI::updateTextString(int nOfTanks, int nOfSoldiers)
{
    std::string newString = std::string("T: ") + std::to_string(nOfTanks) + std::string("\nP: ") + std::to_string(nOfSoldiers);
    if(stringWithText != newString)
    {
        stringWithText = newString;
        std::cout << "BRUUUH" << newString;
        textElement->UpdateText(stringWithText);
    }
}

void HexagonGUI::draw()
{
    int stringWidth = 0;
    int stringHeight = 0;
    TTF_SizeText(textElement->font, getLongerLine(stringWithText).c_str(), &stringWidth, &stringHeight);
    textXOffset = x + ((size  - stringWidth) / 2);
    // Multiply by two to account for the two strings being above each other
    textYOffset = y + ((size - (stringHeight * 2)) / 2);
    SDL_Rect rect = { x, y, size, size};

    if(hexagonTexture)
    {
        SDL_RenderCopy(Window::renderer, hexagonTexture, nullptr, &rect);
        textElement->display(Window::renderer, textXOffset, textYOffset);
    }
}