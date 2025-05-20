//
// Created by Aviad Levine on 14/05/2025.
//

#pragma once

#include <format>
#include <string>

namespace player {
    class Player {
        std::string name = "";
        int coins = 0;
        int actions = 1;
        bool arrested = false, sanctioned = false;

    public:
        Player() {}

        Player(Player const &copy) = default;

        Player &operator=(Player const &copy) = default;

        virtual ~Player() = default;

        std::string getName() const { return name; }

        int getCoins() const { return this->coins; }

        void incCoins(const int coins) { this->coins += coins; }

        int getActions() const { return actions; }

        bool hasActions() const { return actions > 0; }

        void incActions(const int actions) { this->actions += actions; }

        void act();

        virtual void startTurn();

        virtual void endTurn();

        void arrest() { arrested = true; }
        void sanction() { sanctioned = true; }

        bool isArrested() const { return arrested; }
        bool isSanctioned() const { return sanctioned; }
    };

    class Governor final : Player {};

    class Spy final : Player {};

    class Baron final : Player {};

    class General final : Player {};

    class Judge final : Player {};

    class Merchant final : Player {
        void startTurn() override;
    };
};
