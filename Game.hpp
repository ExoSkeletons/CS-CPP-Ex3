//
// Created by Aviad Levine on 14/05/2025.
//

#pragma once

#include <stdexcept>
#include <vector>

#include "Player.hpp"

namespace game {
    class illegal_action : public std::logic_error {
    public:
        explicit illegal_action(const std::string &__arg) : logic_error(__arg) {}

        explicit illegal_action(const char *string) : logic_error(string) {}
    };

    class action_unavailable final : public illegal_action {
    public:
        explicit action_unavailable(const char *reason = "", const bool personal = false)
            : illegal_action(
                std::string(personal
                                ? "You can't use use this action"
                                : "Action unavailable")
                + std::string(reason != "" ? ", " : "") + reason
            ) {}
    };

    typedef std::vector<player::Player *> PlayerList;

    class Game {
    public:
        // Forward definition
        struct Action;

    private:
        PlayerList players;
        int ci = -1;

        player::Player *target_player = nullptr;
        Action *current_action = nullptr;

    public:
        Game() = default;

        ~Game();

        void selectCurrentPlayer(int ci);

        auto getCurrentPlayer() const { return players.at(ci); }

        auto advanceCurrentPlayer();

        void setActionTarget(player::Player *target);

        void setAction(Action *action, bool keep_existing_args);

        auto getWinner() const;

        void playTurn();

        bool isWin() const { return players.size() == 1; }

        void addPlayer(player::Player *player) { players.push_back(player); }

        void removePlayer(const player::Player &player);

        auto getPlayers() const { return players; }
    };

    namespace ui::term {
        static bool confirmAction();

        void printTurn(const Game &game);

        void printWin(const player::Player &winner);

        static player::Player *chooseTarget(const PlayerList &players);

        static Game::Action *chooseAction(player::Player *, Game &game);

        player::Player *queryActionBlockers(
            const PlayerList &players, const player::Player *actor,
            const Game::Action *action
        );

        void printAction(const Game::Action *action);

        void printActionIllegal(const Game::Action *action, const illegal_action &why);

        void printActionBlocked(const Game::Action *action, const player::Player *blocker);

        void printCoupForced(const player::Player *actor);
    }
} // game
