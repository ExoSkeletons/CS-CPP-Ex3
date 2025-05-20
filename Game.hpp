//
// Created by Aviad Levine on 14/05/2025.
//

#pragma once

#include <algorithm>
#include <stdexcept>
#include <vector>

#include "Player.hpp"

namespace game {
    class illegal_action final : public std::logic_error {
    public:
        explicit illegal_action(const std::string &__arg) : logic_error(__arg) {}

        explicit illegal_action(const char *string) : logic_error(string) {}
    };

    typedef std::vector<player::Player *> PlayerList;

    class Game {
    public:
        // Forward definition
        struct Action;

    private:
        PlayerList players;
        int ci = -1;
        player::Player &current_player = *players.at(0);
        player::Player &target_player = current_player;

        Action *current_action = nullptr;

    public:
        Game() = default;

        ~Game();

        void setCurrentPlayer(const player::Player &player) const;

        void advanceCurrentPlayer();

        void setActionTarget(const player::Player &target) const;

        void setAction(Action *action, bool keep_existing_args);

        void playTurn();

        bool isWin() const { return players.size() <= 1; }

        void addPlayer(player::Player *player) { players.push_back(player); }

        void removePlayer(const player::Player &player);
    };

    namespace ui::term {
        static bool confirmAction();

        static player::Player &pickTarget(const PlayerList &players);

        static Game::Action *pickAction(player::Player &, Game &game);
        player::Player *queryActionBlockers(
            const PlayerList &players, const player::Player *current_player,
            const Game::Action *current_action
        );
    }
} // game
