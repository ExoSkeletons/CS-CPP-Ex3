//
// Created by Aviad Levine on 14/05/2025.
//

#pragma once

#include <format>
#include <string>

namespace game {
    class Game;
}

namespace player {
    class Player {
        std::string name = "Unnamed Player";

        int coins = 10, actions = 0;

        bool
                arrested = false,
                sanctioned = false,
                hand_shown = false,
                protected_ = false,
                must_coup = false;

    protected:
        Player() {}

    public:
        Player(Player const &copy) = default;

        Player &operator=(Player const &copy) = default;

        virtual ~Player() = default;

        Player *setName(std::string const &name);

        std::string getName() const { return name; }

        virtual std::string getRoleName() = 0;

        int getCoins() const { return this->coins; }

        void incCoins(const int coins) { this->coins += coins; }

        int getActions() const { return actions; }

        bool hasActions() const { return getActions() > 0; }

        void incActions(const int actions) { this->actions += actions; }

        void act() { actions--; }

        void endTurn() { actions = 0; }

        virtual void onTurnStart();

        virtual void onTurnEnd();

        virtual void onAnyTurnStart();

        virtual void onAnyTurnEnd();

        void arrest() { arrested = true; }
        void sanction() { sanctioned = true; }
        void reveal() { hand_shown = true; }
        void protect() { protected_ = true; }
        void clearCoupReq() { must_coup = false; }

        bool isArrested() const { return arrested; }
        bool isSanctioned() const { return sanctioned; }
        bool isHandShown() const { return hand_shown; }
        bool isProtected() const { return protected_; }
        bool mustCoup() const { return must_coup; }
    };

    class Governor final : public Player {
    public:
        std::string getRoleName() override { return "Governor"; }
    };

    class Spy final : public Player {
    public:
        std::string getRoleName() override { return "Spy"; }
    };

    class Baron final : public Player {
    public:
        std::string getRoleName() override { return "Baron"; }
    };

    class General final : public Player {
    public:
        std::string getRoleName() override { return "General"; }
    };

    class Judge final : public Player {
    public:
        std::string getRoleName() override { return "Judge"; }
    };

    class Merchant final : public Player {
    public:
        std::string getRoleName() override { return "Merchant"; }

        void onTurnStart() override;
    };
};
