#ifndef HEXAGONGUI_H
#define HEXAGONGUI_H

#include "window.hpp"

class HexagonGUI
{
public:
    HexagonGUI(int w, int h, int x, int y, const std::string& imagePath);
    ~HexagonGUI();

    void draw() const;

private:
    int w, h, x, y;
    SDL_Texture* hexagonTexture = nullptr;
};
#endif