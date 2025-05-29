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
        PlayerRef actor, target;
        Game &game;

        explicit Action(
            const std::string &_name,
            const PlayerRef actor, const PlayerRef target,
            Game &game
        ) : name(_name), actor(actor), target(target), game(game) {}

        virtual ~Action() = default; // assumes

        virtual int coinCost() const { return 0; }

        virtual bool blockedBy(const PlayerRef blocker, int &block_cost) const {
            block_cost = 0;
            return false;
        }

        virtual void assertLegal() const;

        void act() const {
            pay();
            action();
        }

        void waste() const { pay(); }

    protected:
        virtual void pay() const;

        virtual void action() const {}
    };

    typedef Game::Action Action;


    struct Coup final : Action {
        Coup(const PlayerRef actor, const PlayerRef target, Game &game): Action(
            "Coup", actor, target, game) {}

        void assertLegal() const override;

        void pay() const override;

        void action() const override;

        int coinCost() const override;

        bool blockedBy(PlayerRef blocker, int &block_cost) const override;
    };


    struct IncCoins : Action {
        IncCoins(const std::string &name, PlayerRef player, Game &game);

        void action() const override;

        void assertLegal() const override;

        virtual int coinAmount() const;
    };

    struct StealCoins : Action {
        StealCoins(const std::string &name, const PlayerRef thief, const PlayerRef victim, Game &game)
            : Action(name, thief, victim, game) {}

        virtual int stealAmount() const;

        virtual bool transfer() const;

        void assertLegal() const override;

        void action() const override;
    };


    struct Gather final : IncCoins {
        Gather(const PlayerRef actor, Game &game): IncCoins("Gather", actor, game) {}
    };

    struct Tax final : IncCoins {
        Tax(const PlayerRef player, Game &game): IncCoins("Tax", player, game) {}

        int coinAmount() const override;
    };

    struct Invest final : IncCoins {
        Invest(const PlayerRef actor, Game &game) : IncCoins("Invest", actor, game) {}

        int coinCost() const override;

        int coinAmount() const override;

        void assertLegal() const override;
    };


    struct Bribe final : Action {
        Bribe(const PlayerRef actor, Game &game): Action("Bribe", actor, actor, game) {}

        int coinCost() const override;

        void action() const override;

        bool blockedBy(PlayerRef blocker, int &block_cost) const override;
    };

    struct Arrest final : StealCoins {
        Arrest(const PlayerRef thief, const PlayerRef victim, Game &game) : StealCoins(
            "Arrest", thief, victim, game) {}

        int stealAmount() const override;

        bool transfer() const override;
    };

    struct Sanction final : Action {
        Sanction(const PlayerRef actor, const PlayerRef target, Game &game) : Action(
            "Sanction", actor, target, game) {}

        void action() const override;

        int coinCost() const override;
    };


    struct Peek final : Action {
        Peek(const PlayerRef actor, const PlayerRef target, Game &game) : Action(
            "Peek", actor, target, game) {}

        void assertLegal() const override;

        void action() const override;
    };

    struct Block final : Action {
        Block(const PlayerRef actor, const PlayerRef target, Game &game): Action("Block", actor, target, game) {}

        void assertLegal() const override;

        void action() const override;
    };
} // game
