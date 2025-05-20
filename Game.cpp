//
// Created by Aviad Levine on 14/05/2025.
//

#include <iostream>

#include "GameActions.hpp"
#include "Game.hpp"

namespace game {
    Game::~Game() {
        for (const auto &player: players) delete player;
        delete current_action;
    }

    void Game::setCurrentPlayer(const player::Player &player) const {
        current_player = player;
        if (current_action)
            current_action->actor = current_player;
    }

    void Game::advanceCurrentPlayer() {
        ci++;
        if (ci == players.size()) ci = 0;
        setCurrentPlayer(*players.at(ci));
    }

    void Game::setActionTarget(const player::Player &target) const {
        target_player = target;
        if (current_action)
            current_action->target = target_player;
    }

    void Game::setAction(Action *action, const bool keep_existing_args = false) {
        delete current_action;
        current_action = action;
        if (keep_existing_args) {
            setCurrentPlayer(current_player);
            setActionTarget(target_player);
        }
    }

    void Game::playTurn() {
        advanceCurrentPlayer();

        current_player.startTurn();
        while (current_player.hasActions() && !isWin()) {
            setAction(ui::term::pickAction(current_player, *this));
            setActionTarget(ui::term::pickTarget(players));

            if (const auto action = current_action) {
                if (!action->canAct())
                    continue;

                if (ui::term::queryActionBlockers(players, &current_player, current_action)) {
                    action->waste();
                    // TODO notify blocked by blocker
                } else action->act();
            }
        }
        current_player.endTurn();

        if (isWin()) {
            // TODO: show win
        }
    }

    void Game::removePlayer(const player::Player &player) {
        players.erase(players.begin(), std::ranges::find(players, &player));
        delete &player;
    }

    namespace ui::term {
        bool confirmAction(const std::string &action, const std::string &desc = "") {
            char confirm;
            if (desc != "") std::cout << desc << std::endl;
            std::cout << "Would you like to " << action << "? (Y/n)" << std::endl;
            do {
                std::cin >> confirm;
                if (confirm == 'y' || confirm == 'Y') return true;
            } while (confirm != 'y' && confirm != 'Y' && confirm != 'n' && confirm != 'N');
            return false;
        }

        player::Player &pickTarget(const PlayerList &players) {
            int ti = 0;
            std::cin >> ti;
            return *players.at(ti);
        }

        Action *pickAction(player::Player &player, Game &game) {
            int act_i = 0;
            std::cin >> act_i;
            // ReSharper disable CppDFAMemoryLeak
            switch (act_i) {
                case 0: return new Coup(player, player, game);

                case 1: return new Gather(player, game);
                case 2: return new Tax(player, game);
                case 3: return new Bribe(player, game);

                case 4: return new Sanction(player, player, game);
                case 5: return new Arrest(player, player, game);

                default: return nullptr;
            }
        }

        player::Player *queryActionBlockers(
            const PlayerList &players, const player::Player *current_player, const Action *current_action
        ) {
            if (current_action)
                for (int pi = 0; pi < players.size(); pi++) {
                    if (players.at(pi) == current_player) continue;
                    if (const auto blocker = players.at(pi); current_action->blockedBy(*blocker))
                        if (confirmAction(
                            std::format("Player {} can block this action.", blocker->getName()),
                            std::format("Block {}", current_player->getName())
                        ))
                            return blocker;
                }
            return nullptr;
        }
    }
} // game
