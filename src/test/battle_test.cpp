//
// Created by honza on 4.12.22.
//
#include "../battle.hpp"
int main(int argc, char *argv[])
{
    battle_result result = battle::result(20,50,10,20);
    std::cout << result.attack_t << " " << result.attack_p <<" " << result.defend_t <<" " << result.defend_p<<std::endl;
    if (result.attackers_won)
        std::cout << "attacker" << std::endl;
    return 0;
}