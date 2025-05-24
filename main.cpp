#include "Game.hpp"
// TIP To <b>Run</b> code, press <shortcut actionId="Run"/> or
// click the <icon src="AllIcons.Actions.Execute"/> icon in the gutter.
int main() {
    game::Game game;

    game.addPlayer((new Merchant())->setName("Whitney"));
    game.addPlayer((new Baron())->setName("Marcy"));
    game.addPlayer((new Spy())->setName("Frank"));
    game.addPlayer((new Judge())->setName("Bill"));
    game.addPlayer((new Baron())->setName("Melvin"));
    game.addPlayer((new Governor())->setName("Peter"));

    while (!game.isWin()) {
        game.playTurn();
        game.advanceTurn();
    }

    return 0;
}

// TIP See CLion help at <a
// href="https://www.jetbrains.com/help/clion/">jetbrains.com/help/clion/</a>.
//  Also, you can try interactive lessons for CLion by selecting
//  'Help | Learn IDE Features' from the main menu.
