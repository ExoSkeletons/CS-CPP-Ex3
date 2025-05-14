//
// Created by Aviad Levine on 14/05/2025.
//

#ifndef PLAYER_H
#define PLAYER_H
#include <format>
#include <string>

#include "Game.hpp"

namespace player {
    class Player {
        std::string name;
        int coins = 0;

        int actionsAvailable = 1;

        bool sanctioned = false;
        bool stolen_from = false;

    public:
        explicit Player(const std::string &name) { this->name = name; }

        Player(const Player &p): Player(p.name) {}

        virtual ~Player() {
            for (auto &action: this->actions)
                delete action;
        }

        Player &operator=(const Player &p) = delete;

        std::string getName() const { return this->name; }

        int getCoins() const { return this->coins; }
        void giveCoins(const int coins) { this->coins = coins; }
        void takeCoins(const int coins) { giveCoins(-coins); }

        virtual void startTurn() {
            actionsAvailable = 1;
            sanctioned = false;
            stolen_from = false;
        }

        void takeAction() { actionsAvailable--; }
        bool hasActions() const { return actionsAvailable >= 1; }

        struct Action {
            const std::string name;
            const int coin_cost;

            Action(const std::string &name, const int coin_cost) : name(name), coin_cost(coin_cost) {}

            explicit Action(const std::string &name) : name(name), coin_cost(0) {}

            virtual ~Action() = default;

            bool isPossible(const Player &actor, const Player &target, const game::Game &g) const {
                try { assertPossible(actor, target, g); } catch (game::illegal_action &e) {
                    printf("%s", e.what());
                    return false;
                }
                return true;
            }

            virtual void act(Player &actor, Player &target, game::Game &g) {
                actor.takeCoins(coin_cost);
                actor.takeAction();
            }

            virtual void assertPossible(const Player &actor, const Player &target, game::Game g) const {
                if (!actor.hasActions())
                    throw game::illegal_action("No Actions left");
            }
        };

        template<int coin_amount>
        struct GatherCoins : Action {
            GatherCoins() : Action("Gather", -coin_amount) {}
            explicit GatherCoins(const std::string &name) : Action(name, -coin_amount) {}

            void assertPossible(const Player &actor, const Player &target, game::Game g) const override {
                Action::assertPossible(actor, target, g);
                if (actor.sanctioned)
                    throw game::illegal_action(
                        std::format("Coin taking (%d) by %s blocked by Sanction.",
                                    actor.getName(), coin_amount
                        )
                    );
            }
        };

        typedef GatherCoins<1> Gather;

        struct Tax final : GatherCoins<2> {
            Tax(): GatherCoins("Tax") {}

            void assertPossible(const Player &actor, const Player &target, game::Game g) const override {
                GatherCoins::assertPossible(actor, target, g);
                // ...
            }
        };

        struct Bribe final : Action {
            Bribe() : Action("Bribe", 4) {}

            void act(Player &actor, Player &target, game::Game &g) override { actor.actionsAvailable += 2; }
        };

        template<int steal_amount, bool add_to_self, bool allow_repeat>
        struct StealCoins final : Action {
            StealCoins(): Action("Steal") {}

            void act(Player &actor, Player &target, game::Game &g) override {
                target.takeCoins(steal_amount);
                if (add_to_self) actor.giveCoins(steal_amount);
                target.stolen_from = true;
            }

            void assertPossible(const Player &actor, const Player &target, game::Game g) const override {
                if (target.getCoins() < steal_amount)
                    throw game::illegal_action("Not enough coins to steal");
                if (!allow_repeat && target.stolen_from)
                    throw game::illegal_action("Cannot steal from same player twice");
            }
        };

        typedef StealCoins<1, true, false> Arrest;

        struct Sanction final : Action {
            Sanction() : Action("Sanction", 3) {}

            void act(Player &actor, Player &target, game::Game &g) override { target.sanctioned = true; }
        };

        struct Coup final : Action {
            Coup() : Action("Coup", 7) {}

            void act(Player &actor, Player &target, game::Game &g) override { g.removePlayer(target); }
        };

        std::pmr::vector<const Action *> actions = {
            new Gather(), new Tax(),
            new Bribe(),
            new Arrest(), new Sanction(),
            new Coup()
        };
    };
};

#endif //PLAYER_H
