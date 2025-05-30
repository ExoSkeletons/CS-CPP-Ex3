#include <iostream>

#include "Game.hpp"
// TIP To <b>Run</b> code, press <shortcut actionId="Run"/> or
// click the <icon src="AllIcons.Actions.Execute"/> icon in the gutter.
int main() {
    game::Game game;


    /*/
    game.addPlayer((new Merchant())->setName("Whitney"));
    game.addPlayer((new Baron())->setName("Marcy"));
    game.addPlayer((new Spy())->setName("Frank"));
    game.addPlayer((new Judge())->setName("Bill"));
    game.addPlayer((new Baron())->setName("Melvin"));
    game.addPlayer((new Governor())->setName("Peter"));
    //*/


    int in;
    std::string name;

    do {
        // print players
        std::cout << "Players:" << std::endl;
        if (game.getPlayers().size() == 0) std::cout << "[none]";
        else
            for (int i = 0; i < game.getPlayers().size(); i++) {
                const auto p = game.getPlayers()[i];
                std::cout << i << ") " << p->getName() << " [" << p->getRoleName() << "]" << std::endl;
            }
        std::cout << std::endl;
        if (game.getPlayers().size() >= 2) std::cout << "0. Start Game\n";
        std::cout << "1. Add Player\n";
        if (game.getPlayers().size() > 0) std::cout << "2. Remove Player\n";
        // input
        std::cin >> in;

        switch (in) {
            case 1: {
                game::ui::term::addPlayer(game);
                break;
            }
            case 2: {
                game::ui::term::removePlayer(game);
                break;
            }
            case 0:
            default: ;
        }
    } while (!(in == 0 && game.getPlayers().size() > 1));

    std::cout << "\n\n---------- Game Start! ------------\n\n";

    while (!game.isWin()) {
        game.playTurn();
        game.advanceTurn();
    }
}

// TIP See CLion help at <a
// href="https://www.jetbrains.com/help/clion/">jetbrains.com/help/clion/</a>.
//  Also, you can try interactive lessons for CLion by selecting
//  'Help | Learn IDE Features' from the main menu.
