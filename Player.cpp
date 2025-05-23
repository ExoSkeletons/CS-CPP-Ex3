//
// Created by Aviad Levine on 14/05/2025.
//

#include "Player.hpp"

namespace player {
    Player *Player::setName(std::string const &name) {
        if (!name.empty()) this->name = name;
        return this;
    }

    void Player::act() { actions--; }

    void Player::onTurnStart() {
        actions = 1;
        if (getCoins() >= 10)
            must_coup = true;
    }

    void Player::onTurnEnd() { sanctioned = arrested = false; }
    void Player::onAnyTurnEnd() { hand_shown = false; }

    void Merchant::onTurnStart() { if (getCoins() >= 3) incCoins(1); }
}
