//
// Created by Aviad Levine on 20/05/2025.
//

#include "GameActions.hpp"
#include "typetools.hpp"

namespace game {
    bool Action::canAct() const { return actor.hasActions() && actor.getCoins() >= coinCost(); }
    void Action::pay() const { actor.incCoins(-coinCost()); }

    Gather::Gather(player::Player &actor, Game &game): Action("Gather", actor, actor, game) {}
    void Gather::action() const { actor.incCoins(1); }
    bool Gather::canAct() const { return Action::canAct() && !actor.isSanctioned(); }

    Bribe::Bribe(player::Player &actor, Game &game): Action("Bribe", actor, actor, game) {}
    int Bribe::coinCost() const { return 4; }
    void Bribe::action() const { actor.incActions(2); }
    bool Bribe::blockedBy(player::Player &blocker) const { return instanceof<player::Judge>(&blocker); }

    StealCoins::StealCoins(const std::string &_name, player::Player &thief, player::Player &victim, Game &game)
        : Action(_name, thief, victim, game) {}

    int StealCoins::stealAmount() const { return 1; }
    bool StealCoins::transfer() const { return true; }

    bool StealCoins::canAct() const {
        return Action::canAct() && !target.isArrested() && !target.getCoins() > stealAmount();
    }

    void StealCoins::action() const {
        target.arrest();
        const int steal = stealAmount();
        target.incCoins(-steal);
        if (transfer()) actor.incActions(steal);
    }

    Arrest::Arrest(player::Player &thief, player::Player &victim, Game &game)
        : StealCoins("Arrest", thief, victim, game) {}

    int Arrest::stealAmount() const {
        return StealCoins::stealAmount() * (instanceof<player::Merchant>(&target) ? 2 : 1);
    }

    bool Arrest::transfer() const { return !instanceof<player::Merchant>(&target); }

    Sanction::Sanction(player::Player &actor, player::Player &target, Game &game)
        : Action("Sanction", actor, target, game) {}

    void Sanction::action() const { target.sanction(); }

    Coup::Coup(player::Player &actor, player::Player &target, Game &game): Action("Coup", actor, target, game) {}
    void Coup::action() const { game.removePlayer(target); }

    Tax::Tax(player::Player &actor, Game &game): Action("Tax", actor, actor, game) {}
    bool Tax::canAct() const { return Action::canAct() && !actor.isSanctioned(); }
    void Tax::action() const { actor.incCoins(instanceof<player::Governor>(&actor) ? 2 : 3); }
} // game
