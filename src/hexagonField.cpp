#include "hexagonField.hpp"
#include <iostream>

HexagonField::HexagonField(int q, int r, int s, int size, int horizontalOffset, int verticalOffset, std::string imagePath, std::string fontPath): q(q), r(r), s(s)
{
    assert (q + r + s == 0);
    this->gui = new HexagonGUI(size, horizontalOffset, verticalOffset, imagePath, fontPath);
}

HexagonField::HexagonField(int q, int r, int s): q(q), r(r), s(s)
{
    assert (q + r + s == 0);
}

HexagonField::~HexagonField()
{
    std::cout << "Sus" << std::endl;
}

HexagonField HexagonField::hexagonAdd(HexagonField a, HexagonField b)
{
    return HexagonField(a.q + b.q, a.r + b.r, a.s + b.s);
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
