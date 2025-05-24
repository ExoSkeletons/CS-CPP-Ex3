//
// Created by Aviad Levine on 20/05/2025.
//

#pragma once
#include <string>

#include "Game.hpp"
#include "Player.hpp"

namespace game {
    struct Game::Action {
        const std::string name;
        player::Player *actor, *target;
        Game &game;

        explicit Action(
            const std::string &_name,
            player::Player *actor, player::Player *target,
            Game &game
        ) : name(_name), actor(actor), target(target), game(game) {}

        virtual ~Action() = default; // assumes

        virtual int coinCost() const { return 0; }
        virtual bool blockedBy(player::Player &blocker) const { return false; }

        virtual void assertLegal() const;

        void act() const {
            pay();
            actor->act();
            action();
        }

        void waste() const { pay(); }

    protected:
        virtual void pay() const;

        virtual void action() const {}
    };

    typedef Game::Action Action;


    struct Coup final : Action {
        Coup(player::Player *actor, player::Player *target, Game &game): Action("Coup", actor, target, game) {}

        void pay() const override;

        void action() const override;

        int coinCost() const override;

        bool blockedBy(player::Player &blocker) const override;
    };


    struct IncCoins : Action {
        IncCoins(const std::string &name, player::Player *player, Game &game);

        void action() const override;

        void assertLegal() const override;

        virtual int coinAmount() const;
    };

    struct StealCoins : Action {
        StealCoins(const std::string &name, player::Player *thief, player::Player *victim, Game &game)
            : Action(name, thief, victim, game) {}

        virtual int stealAmount() const;

        virtual bool transfer() const;

        void assertLegal() const override;

        void action() const override;
    };


    struct Gather final : IncCoins {
        Gather(player::Player *actor, Game &game): IncCoins("Gather", actor, game) {}
    };

    struct Tax final : IncCoins {
        Tax(player::Player *player, Game &game): IncCoins("Tax", player, game) {}

        int coinAmount() const override;
    };


    struct Bribe final : Action {
        Bribe(player::Player *actor, Game &game): Action("Bribe", actor, actor, game) {}

        int coinCost() const override;

        void action() const override;

        bool blockedBy(player::Player &blocker) const override;
    };

    struct Arrest final : StealCoins {
        Arrest(player::Player *thief, player::Player *victim, Game &game) : StealCoins("Arrest", thief, victim, game) {}

        int stealAmount() const override;

        bool transfer() const override;

        bool blockedBy(player::Player &blocker) const override { return target == &blocker && target->isProtected(); }
    };

    struct Sanction final : Action {
        Sanction(player::Player *actor, player::Player *target, Game &game) : Action("Sanction", actor, target, game) {}

        void action() const override;

        int coinCost() const override;
    };


    struct Peek final : Action {
        Peek(player::Player *actor, player::Player *target, Game &game) : Action("Peek", actor, target, game) {}

        void assertLegal() const override;

        void action() const override;
    };

    struct Protect final : Action {
        Protect(player::Player *actor, player::Player *target, Game &game): Action("Protect", actor, target, game) {}

        int coinCost() const override { return 5; }

        void assertLegal() const override;

        void action() const override;
    };
} // game
