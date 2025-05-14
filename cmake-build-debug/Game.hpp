//
// Created by Aviad Levine on 14/05/2025.
//

#ifndef GAME_H
#define GAME_H

#include <stdexcept>
#include <vector>


namespace player {
    class Player;
}

namespace game {
    class illegal_action final : public std::logic_error {
    public:
        explicit illegal_action(const std::string &__arg) : logic_error(__arg) {}

        explicit illegal_action(const char *string) : logic_error(string) {}
    };

    class Game {
        std::vector<player::Player> players;

    public:
        Game();

        ~Game();

        void playTurn();

        bool isWin() const;

        void addPlayer(player::Player &player);

        void removePlayer(player::Player &player);
    };
} // game

#endif //GAME_H
