#pragma once
#include "window.hpp"

class HexagonGUI : public Window
{
public:
    HexagonGUI(const Window& window, int w, int h, int x, int y, int r, int g, int b, int a);

    void draw() const;

private:
    int w, h, x, y, r, g, b, a;
};