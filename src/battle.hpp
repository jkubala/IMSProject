#ifndef IMSPROJECT_BATTLE_HPP
#define IMSPROJECT_BATTLE_HPP
#include <cstdlib>
#include <iostream>
#include <ctime>
class battle_result
{
public:
    int attack_t, attack_p, defend_t, defend_p;
    bool attackers_won;
    battle_result(int attack_t, int attack_p, int defend_t, int defend_p, bool attackers_won);
};

class battle {
public:
    static battle_result result(int attack_t, int attack_p, int defend_t, int defend_p);
};


#endif //IMSPROJECT_BATTLE_HPP
