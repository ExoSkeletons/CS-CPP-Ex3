//
// Created by Aviad Levine on 14/05/2025.
//

#pragma once

#include <stdexcept>
#include <vector>

#include "Player.hpp"
#include "typetools.hpp"

using namespace player;

namespace game {
    class illegal_action : public std::logic_error {
    public:
        explicit illegal_action(const std::string &__arg) : logic_error(__arg) {}

        explicit illegal_action(const char *string) : logic_error(string) {}
    };

    class action_unavailable final : public illegal_action {
    public:
        explicit action_unavailable(const std::string &reason = std::string(""), const bool personal = false)
            : illegal_action(
                std::string(personal
                                ? "You can't use this action"
                                : "Action unavailable")
                + std::string(!reason.empty() ? ", " : "") + reason
            ) {}
    };

    typedef std::vector<PlayerRef> PlayerList;

    class Game {
    public:
        // Forward definition
        struct Action;

    private:
        PlayerList players;
        size_t ci = 0;

        PlayerRef target_player = nullptr;
        Action *current_action = nullptr;

    public:
        Game() = default;

        ~Game();

        void selectCurrentPlayer(int ci);

        auto getCurrentPlayer() const { return players.at(ci); }

        void advanceCurrentPlayer();

        void setActionTarget(PlayerRef target);

        void setAction(Action *action, bool keep_existing_args);

        auto getWinner() const;

        void playTurn();

        bool isWin() const { return players.size() <= 1; }

        void addPlayer(const PlayerRef player) { players.push_back(player); }

        void removePlayer(const PlayerRef player) {
            if (removeValue<PlayerRef>(players, player))
                delete player;
            if (player == target_player) target_player = nullptr;
        }

        auto getPlayers() const { return players; }
    };

    namespace ui::term {
        bool confirmAction(const std::string &action, const std::string &desc);

        void printTurn(const Game &game);

        void printWin(PlayerRef winner);

        PlayerRef chooseTarget(const PlayerList &players);

        Game::Action *chooseAction(PlayerRef, Game &game);

        PlayerRef queryActionBlockers(
            const PlayerList &players, PlayerRef actor,
            const Game::Action *action,
            int &block_cost
        );

        void printAction(const Game::Action *action);

        void printActionIllegal(const Game::Action *action, const illegal_action &why);

        void printActionBlocked(const Game::Action *action, PlayerRef blocker);

        void printCoupForced(PlayerRef actor);
    }
} // game
