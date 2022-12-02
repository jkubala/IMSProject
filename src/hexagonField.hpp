#ifndef HEXAGON_H
#define HEXAGON_H
#include <assert.h>
#include <vector>
#include "hexagonGUI.hpp"

class HexagonField
{
public:
    const int q, r, s;
    HexagonField(int q, int r, int s);
    HexagonField hexagonAdd(HexagonField a, HexagonField b);
    HexagonField hexagonSubtract(HexagonField a, HexagonField b);
    HexagonField hexagonMultiply(HexagonField a, int k);
    int hexagonLength(HexagonField hex);
    int hexagonDistance(HexagonField a, HexagonField b);
    HexagonField hexagonDirection(int direction /* 0 to 5 */);
    HexagonField hexagonNeighbor(HexagonField hex, int direction);

    static const std::vector<HexagonField> hexagonDirections;

    bool operator == (const HexagonField comparedHex) const
    {
        return this->q == comparedHex.q && this->r == comparedHex.r && this->s == comparedHex.s;
    }
    bool operator != (const HexagonField comparedHex) const
    {
        return !(*this == comparedHex);
    }

    HexagonGUI* gui = nullptr;
};

namespace std
{
  template<>
    struct hash<HexagonField>
    {
      size_t operator()(const HexagonField & obj) const
      {
        return hash<int>()(obj.q);
      }
    };
}
#endif