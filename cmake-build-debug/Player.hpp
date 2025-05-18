//
// Created by Aviad Levine on 14/05/2025.
//

#ifndef PLAYER_H
#define PLAYER_H
#include <format>
#include <string>
#include <algorithm>
#include <deque>

#include "Game.hpp"
#include "typetools.hpp"

namespace player {
    class Judge;
    class Governor;
    // todo: all actions are future actions now. including gather.
    //  no other way to make sense of it.
    //  also all undo-s are nullification (coin loss)
    //  good luck.
    class Player {
        std::string name;
        int coins = 0;

        int actionsAvailable = 1;

    public:
        bool sanctioned = false;
        bool stolen_from = false;
        bool can_arrest = true;

        explicit Player(const std::string &name) { this->name = name; }

        Player(const Player &p): Player(p.name) {}

        Player &operator=(const Player &p) = delete;

        std::string getName() const { return this->name; }

        int getCoins() const { return this->coins; }
        void increaseCoins(const int coins) { this->coins += coins; }
        void decreaseCoins(const int coins) { increaseCoins(-coins); }

        virtual void giveCoins(const int coins) { increaseCoins(coins); }

        virtual int trySteal(const int coins) {
            decreaseCoins(coins); // take from me :(
            return coins; // stolen amount
        }

        virtual void startTurn() {
            actionsAvailable = 1;
            stolen_from = false;
        }

        virtual void endTurn() {
            sanctioned = false;
            can_arrest = true;
        }

        void takeAction() { actionsAvailable--; }
        bool hasActions() const { return actionsAvailable >= 1; }

        template<class PA, class PT, class PB>
        struct Action {
            const std::string name;
            const int coin_cost;

            Action(const std::string &name, const int coin_cost) : name(name), coin_cost(coin_cost) {}

            explicit Action(const std::string &name) : name(name), coin_cost(0) {}

            virtual ~Action() = default;

            bool canAct(const PA &actor, const PT &target, const game::Game &g) const {
                try { assertPossible(actor, target, g); } catch (game::illegal_action &e) {
                    printf("%s", e.what());
                    return false;
                }
                return true;
            }

            virtual void act(PA &actor, PT &target, game::Game &g) {
                actor.decreaseCoins(coin_cost);
                actor.takeAction();
            }

            virtual bool canBeUndoneBy(PB &blocker) { return false; }

            virtual void undo(PA &actor, PB &blocker) {}

            virtual void assertPossible(const PA &actor, const PT &target, game::Game g) const {
                if (!actor.hasActions())
                    throw game::illegal_action("No Actions left");
            }
        };

        typedef Action<Player, Player, Player> PAction;

        template<int coin_amount>
        struct GatherCoins : PAction {
            GatherCoins() : PAction("Gather", -coin_amount) {}
            explicit GatherCoins(const std::string &name) : PAction(name, -coin_amount) {}

            void assertPossible(const Player &actor, const Player &target, game::Game g) const override {
                PAction::assertPossible(actor, target, g);
                if (actor.sanctioned)
                    throw game::illegal_action(
                        std::format("Coin taking (%d) by %s blocked by Sanction.",
                                    actor.getName(), coin_amount
                        )
                    );
            }
        };

        typedef GatherCoins<1> Gather;

        template<int coin_amount>
        struct TaxCoins final : GatherCoins<coin_amount> {
            TaxCoins(): GatherCoins<coin_amount>("Tax") {}

            bool canBeUndoneBy(Player &blocker) override { return instanceof<Governor>(&blocker); }
        };

        typedef TaxCoins<2> Tax;

        struct Bribe final : PAction {
            Bribe() : PAction("Bribe", 4) {}

            void act(Player &actor, Player &target, game::Game &g) override { actor.actionsAvailable += 2; }

            bool canBeUndoneBy(Player &blocker) override { return instanceof<Judge>(&blocker); }
        };

        template<int steal_amount, bool add_to_self, bool allow_repeat>
        struct StealCoins final : PAction {
            StealCoins(): PAction("Arrest") {}

            void act(Player &actor, Player &target, game::Game &g) override {
                const int stolen_amount = target.trySteal(steal_amount);
                if (add_to_self) actor.giveCoins(stolen_amount);
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

        struct Sanction final : PAction {
            Sanction() : PAction("Sanction", 3) {}

            void act(Player &actor, Player &target, game::Game &g) override { target.sanctioned = true; }
        };

        struct Coup final : PAction {
            Coup() : PAction("Coup", 7) {}

            void act(Player &actor, Player &target, game::Game &g) override { g.removePlayer(target); }
        };

        std::vector<PAction *> availableActions;

        PAction
                *gather = new Gather(), *tax = new Tax(),
                *bribe = new Bribe(),
                *arrest = new Arrest(), *sanction = new Sanction(),
                *coup = new Coup();

    protected:
        virtual void initActions() {
            availableActions.push_back(gather);
            availableActions.push_back(tax);
            availableActions.push_back(bribe);
            availableActions.push_back(arrest);
            availableActions.push_back(sanction);
            availableActions.push_back(coup);
        }

        void replaceAction(PAction *action, PAction *replacement) {
            std::erase(availableActions, action);
            delete action;
            availableActions.push_back(replacement);
        }

    public:
        virtual ~Player() {
            delete gather;
            delete tax;
            delete bribe;
            delete arrest;
            delete sanction;
            delete coup;
        }
    };

    class Governor final : Player {
        explicit Governor(const std::string &name) : Player(name) { replaceAction(tax, new Tax<3>()); }
    };

    class Spy final : Player {
        int peeked_coins = -1;

        struct PeekCoins final : Action<Spy, Player, Player> {
            PeekCoins() : Action("Peek", 0) {}

            void act(Spy &actor, Player &target, game::Game &g) override { actor.peeked_coins = target.getCoins(); }
        };

        struct DisableArrest final : PAction {
            DisableArrest() : Action("Peek", 0) {}

            void act(Player &actor, Player &target, game::Game &g) override { actor.can_arrest = false; }
        };

    public:
        explicit Spy(const std::string &name): Player(name) {}
    };

    class Merchant final : Player {
        explicit Merchant(const std::string &name) : Player(name) {}

        int trySteal(const int coins) override {
            // steal from merchant? no no no!
            decreaseCoins(2 * coins);
            return 0;
        }
    };

    class Judge final : Player {
        explicit Judge(const std::string &name) : Player(name) {}
    };
};


#endif //PLAYER_H
