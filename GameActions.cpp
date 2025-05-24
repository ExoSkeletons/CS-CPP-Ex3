//
// Created by Aviad Levine on 20/05/2025.
//

#include "GameActions.hpp"
#include "typetools.hpp"

namespace game {
    void Action::assertLegal() const {
        if (!actor->hasActions()) throw illegal_action("No actions left");
        if (actor->getCoins() < coinCost()) throw illegal_action("Not enough coins for action");
    }

    void Action::pay() const {
        actor->incCoins(-coinCost());
        actor->act();
    }

    IncCoins::IncCoins(const std::string &name, Player *player, Game &game): Action(name, player, player, game) {}
    void IncCoins::action() const { actor->incCoins(coinAmount()); }
    int IncCoins::coinAmount() const { return 1; }

    void IncCoins::assertLegal() const {
        if (actor->isSanctioned()) throw action_unavailable("you're sanctioned", true);
        Action::assertLegal();
    }

    int Tax::coinAmount() const { return instanceof<Governor>(actor) ? 2 : 3; }

    int Bribe::coinCost() const { return 4; }
    void Bribe::action() const { actor->incActions(2); }
    bool Bribe::blockedBy(const PlayerRef blocker, int &block_cost) const { return instanceof<Judge>(blocker); }

    int StealCoins::stealAmount() const { return 1; }
    bool StealCoins::transfer() const { return true; }

    void StealCoins::assertLegal() const {
        if (target->isArrested())
            throw action_unavailable("target is already arrested", true);
        if (target->getCoins() < stealAmount())
            throw action_unavailable("target doesn't have enough to steal");
        Action::assertLegal();
    }

    void StealCoins::action() const {
        target->arrest();
        const int steal = stealAmount();
        target->incCoins(-steal);
        if (transfer()) actor->incActions(steal);
    }

    int Arrest::stealAmount() const { return StealCoins::stealAmount() * (instanceof<Merchant>(target) ? 2 : 1); }
    bool Arrest::transfer() const { return !instanceof<Merchant>(target); }

    void Sanction::action() const {
        target->sanction();
        if (instanceof<Baron>(target)) target->incCoins(1);
    }

    int Sanction::coinCost() const { return instanceof<Judge>(target) ? 4 : 3; }

    int Invest::coinCost() const { return 3; }
    int Invest::coinAmount() const { return 6; }
    void Invest::assertLegal() const { if (!instanceof<Baron>(actor)) throw action_unavailable("", true); }

    void Peek::assertLegal() const {
        if (!instanceof<Spy>(actor)) throw action_unavailable("", true);
        Action::assertLegal();
    }

    void Peek::action() const {
        target->reveal();
        actor->incActions(1);
    }

    void Coup::pay() const {
        Action::pay();
        actor->clearCoupReq();
    }

    void Coup::assertLegal() const { if (actor == target) throw action_unavailable("cannot coup self"); }
    void Coup::action() const { game.removePlayer(target); }
    int Coup::coinCost() const { return 7; }

    bool Coup::blockedBy(const PlayerRef blocker, int &block_cost) const {
        if (instanceof<General>(blocker)) {
            block_cost = 5;
            return true;
        }
        return false;
    }
} // game
