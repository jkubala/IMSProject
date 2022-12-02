#include "hexagonField.hpp"

HexagonField::HexagonField(int q, int r, int s): q(q), r(r), s(s)
{
    assert (q + r + s == 0);
}

const std::vector<HexagonField> HexagonField::hexagonDirections = {
  HexagonField(1, 0, -1), HexagonField(1, -1, 0), HexagonField(0, -1, 1),
  HexagonField(-1, 0, 1), HexagonField(-1, 1, 0), HexagonField(0, 1, -1)
};

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

HexagonField HexagonField::hexagonDirection(int direction /* 0 to 5 */) {
    assert (0 <= direction && direction < 6);
    return hexagonDirections[direction];
}

HexagonField HexagonField::hexagonNeighbor(HexagonField hex, int direction) {
    return hexagonAdd(hex, hexagonDirection(direction));
}
