#include "battle.hpp"

battle_result::battle_result(int attack_t, int attack_p, int defend_t, int defend_p, bool attackers_won)
        : attack_t(attack_t), attack_p(attack_p), defend_t(defend_t), defend_p(defend_p),
          attackers_won(attackers_won) {}

battle_result battle::result(int attack_t, int attack_p, int defend_t, int defend_p) {
    std::srand(std::time(nullptr)); // use current time as seed for random generator
    int survivor_t = 0, survivor_p = 0;
    int coward_t = 0, coward_p = 0;
    while (true) {
        // tanks attack
        for (int i = 0; i < attack_t; ++i) {
            //each tank attack 3 times
            for (int j = 0; j < 3; ++j) {
                int x = 1 + std::rand() / ((RAND_MAX + 1u) / 6);
                int y = 1 + std::rand() / ((RAND_MAX + 1u) / 6);
                if (defend_t > 0) {
                    if (x >= 6) {
                        defend_t--;
                        if (y == 1) {
                            survivor_t++;
                        }

                    }
                } else if (defend_p > 0) {
                    if (x > 1) {
                        defend_p--;
                        if (y >= 3) {
                            survivor_p++;
                        }
                    }
                } else {
                    return battle_result(attack_t + coward_t, attack_p + coward_p, survivor_t, survivor_p, true);
                }
            }
            break;

        }

        // tank defends
        for (int i = 0; i < defend_t; ++i) {
            //each tank attack 3 times
            for (int j = 0; j < 3; ++j) {
                int x = 1 + std::rand() / ((RAND_MAX + 1u) / 6);
                int y = 1 + std::rand() / ((RAND_MAX + 1u) / 6);
                if (attack_t > 0) {
                    if (x >= 2) {
                        attack_t--;
                        if (y == 1) {
                            coward_t++;
                        }
                    }
                } else if (attack_p > 0) {
                    if (x > 4) {
                        attack_p--;
                        if (y > 4) {
                            coward_p++;
                        }
                    }
                } else {
                    return battle_result(coward_t, coward_p, defend_t + survivor_t, defend_p + survivor_p, false);
                }
            }
            break;
        }

        // infantry attacks
        for (int i = 0; i < attack_p; ++i) {
            //each soldier attack once
            int x = 1 + std::rand() / ((RAND_MAX + 1u) / 6);
            int y = 1 + std::rand() / ((RAND_MAX + 1u) / 6);
            if (defend_t > 0) {
                if (x * y == 36) {
                    defend_t--;
                }
            } else if (defend_p > 0) {
                if (x > 4) {
                    defend_p--;
                    if (y >= 6) {
                        survivor_p++;
                    }
                }
            } else {
                return battle_result(attack_t + coward_t, attack_p + coward_p, survivor_t, survivor_p, true);
            }
            break;
        }

        // infantry defends
        for (int i = 0; i < defend_p; ++i) {
            //each soldier attack once
            int x = 1 + std::rand() / ((RAND_MAX + 1u) / 6);
            int y = 1 + std::rand() / ((RAND_MAX + 1u) / 6);
            if (attack_t > 0) {
                if (x * y >= 30) {
                    defend_t--;
                }
            } else if (attack_p > 0) {
                if (x > 2) {
                    defend_p--;
                    if (y >= 6) {
                        survivor_p++;
                    }
                }
            } else {
                return battle_result(attack_t + coward_t, attack_p + coward_p, survivor_t, survivor_p, false);
            }
            break;
        }
    }
}