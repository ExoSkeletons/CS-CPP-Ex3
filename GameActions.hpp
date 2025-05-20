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
        player::Player &actor, &target;
        Game &game;

        explicit Action(
            const std::string &_name,
            player::Player &actor, player::Player &target,
            Game &game
        ) : name(_name), actor(actor), target(target), game(game) {}

        virtual ~Action() = default; // assumes

        virtual int coinCost() const { return 0; }
        virtual bool blockedBy(player::Player &blocker) const { return false; }

        virtual bool canAct() const;

        void act() const {
            pay();
            action();
        }

        void waste() const { pay(); }

    protected:
        void pay() const;

        virtual void action() const {}
    };

    typedef Game::Action Action;

    struct Gather final : Action {
        Gather(player::Player &actor, Game &game);

        void action() const override;

        bool canAct() const override;
    };

    struct Tax final : Action {
        Tax(player::Player &actor, Game &game);

        bool canAct() const override;

        void action() const override;
    };

    struct Bribe final : Action {
        Bribe(player::Player &actor, Game &game);

        int coinCost() const override;

        void action() const override;

        bool blockedBy(player::Player &blocker) const override;
    };

    struct StealCoins : Action {
        explicit StealCoins(const std::string &_name, player::Player &thief, player::Player &victim, Game &game);

        virtual int stealAmount() const;

        virtual bool transfer() const;

        bool canAct() const override;

        void action() const override;
    };

    struct Arrest final : StealCoins {
        Arrest(player::Player &thief, player::Player &victim, Game &game);

        int stealAmount() const override;

        bool transfer() const override;
    };

    struct Sanction final : Action {
        Sanction(player::Player &actor, player::Player &target, Game &game);

        void action() const override;
    };

    struct Coup final : Action {
        Coup(player::Player &actor, player::Player &target, Game &game);

        void action() const override;
    };
} // game
