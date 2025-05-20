//
// Created by Aviad Levine on 14/05/2025.
//

#include "Player.hpp"

namespace player {
    void Player::act() { actions--; }
    void Player::startTurn() { actions = 1; }
    void Player::endTurn() { sanctioned = arrested = false; }

    void Merchant::startTurn() { if (getCoins() >= 3) incCoins(1); }
}
