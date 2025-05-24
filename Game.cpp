//
// Created by Aviad Levine on 14/05/2025.
//

#include <iostream>

#include "GameActions.hpp"
#include "Game.hpp"

using std::string, std::cout, std::cin, std::endl;

namespace game {
    Game::~Game() {
        for (const auto &player: players) delete player;
        delete current_action;
    }

    void Game::selectCurrentPlayer(const int ci) {
        this->ci = ci;
        if (current_action)
            current_action->actor = getCurrentPlayer();
    }

    void Game::advanceTurn() {
        ci++;
        if (ci >= players.size()) ci = 0;
        selectCurrentPlayer(ci);
    }

    void Game::setActionTarget(const PlayerRef target) {
        target_player = target;
        if (current_action)
            current_action->target = target;
    }

    void Game::setAction(Action *action, const bool keep_existing_args = false) {
        delete current_action;
        current_action = action;
        if (keep_existing_args) {
            selectCurrentPlayer(ci);
            setActionTarget(target_player);
        }
    }

    auto Game::getWinner() const {
        if (players.size() > 1) { throw illegal_action("No winner yet"); }
        return players.at(0);
    }

    void Game::playTurn() {
        const auto current_player = getCurrentPlayer();

        for (const auto &p: players)
            p->onAnyTurnStart();
        current_player->onTurnStart();

        while (current_player->hasActions() && !isWin()) {
            ui::term::printTurn(*this);

            // choose action
            if (current_player->mustCoup()) {
                ui::term::printCoupForced(current_player);
                setAction(new Coup(current_player, nullptr, *this));
            } else
                setAction(ui::term::chooseAction(current_player, *this));

            // select target
            if (current_action != nullptr && current_action->target == nullptr)
                setActionTarget(ui::term::chooseTarget(players));

            if (const auto action = current_action) {
                try {
                    // check if action is legal
                    action->assertLegal();

                    // check if anyone can (and wants to) block this action
                    // [fake real-time]
                    int blocked_cost = 0;
                    if (const auto blocking =
                            ui::term::queryActionBlockers(players, current_player, current_action, blocked_cost)
                    ) {
                        ui::term::printActionBlocked(action, blocking);
                        // block action (waste coins)
                        blocking->incCoins(-blocked_cost);
                        action->waste();
                    } else {
                        ui::term::printAction(action);
                        // perform action
                        action->act();
                    }
                } catch (illegal_action &why) { ui::term::printActionIllegal(action, why); }
            }
        }

        current_player->onTurnEnd();
        for (const auto &p: players)
            p->onAnyTurnEnd();

        if (isWin()) ui::term::printWin(getWinner());
    }

    namespace ui::term {
        bool confirmAction(const string &action, const string &desc = "") {
            char confirm;
            if (desc != "") cout << desc << endl;
            cout << "Would you like to " << action << "? (Y/n)" << endl;
            do {
                cin >> confirm;
                if (confirm == 'y' || confirm == 'Y') return true;
            } while (confirm != 'y' && confirm != 'Y' && confirm != 'n' && confirm != 'N');
            return false;
        }

        void printTurn(const Game &game) {
            cout << endl;

            constexpr int pad = 3;
            const auto pads = string(pad, ' ');
            // calc col width (max strlen per col)
            size_t cw_name = 10, cw_role = 7;
            for (const auto p: game.getPlayers()) {
                if (cw_name < p->getName().length()) cw_name = p->getName().length();
                if (cw_role < p->getRoleName().length()) cw_role = p->getRoleName().length();
            }
            cw_name += 2;
            cw_role += 2;
            cout <<
                    "   " << "Name" << string(cw_name - 4, ' ') << pads <<
                    "Role" << string(cw_role - 4, ' ') << pads <<
                    "Sc Ar" << " " << "Coins" <<
                    endl;
            cout <<
                    "   " << string(cw_name, '-') << pads << string(cw_role, '-') <<
                    pads << "-  - " << " " << "-----" <<
                    endl;

            const auto current_player = game.getCurrentPlayer();
            for (const auto p: game.getPlayers()) {
                const auto name = p->getName(), role = p->getRoleName();
                const bool isPC = p == current_player;
                isPC
                    ? cout << p->getActions() << ">"
                    : cout << "  ";
                cout <<
                        " " <<
                        name << string(cw_name - name.length(), ' ') << pads <<
                        role << string(cw_role - role.length(), ' ') << pads <<
                        (p->isSanctioned() ? "x " : "  ") << " " <<
                        (p->isArrested() ? "x " : "  ") << " ";
                if (p->isHandShown() || isPC)
                    cout <<
                            p->getCoins();
                cout << endl;
            }
            cout << endl;
        }

        void printWin(const PlayerRef winner) { cout << winner->getName() << " wins!" << endl; }

        PlayerRef chooseTarget(const PlayerList &players) {
            cout << "Pick Target: [0-" << players.size() - 1 << "]" << endl;
            int ti = 0;
            cin >> ti;
            return players.at(ti);
        }

        Action *chooseAction(const PlayerRef player, Game &game) {
            char act_i;
            cout <<
                    "Choose action: [x:END   c:Coup   g:Gather t:Tax i:Invest    b:Bribe   s:Sanction a:Arrest   p:Peek]"
                    << endl;
            cin >> act_i;
            switch (act_i) {
                case 'x': {
                    player->endTurn();
                    return nullptr;
                }

                case 'c': return new Coup(player, nullptr, game);

                case 'g': return new Gather(player, game);
                case 't': return new Tax(player, game);
                case 'i': return new Invest(player, game);

                case 'b': return new Bribe(player, game);

                // nullptr target signals waiting to be filled later
                case 's': return new Sanction(player, nullptr, game);
                case 'a': return new Arrest(player, nullptr, game);

                case 'p': return new Peek(player, nullptr, game);

                default: return nullptr;
            }
        }

        PlayerRef queryActionBlockers(
            const PlayerList &players, const PlayerRef actor, const Action *action, int &block_cost) {
            if (action)
                for (size_t pi = 0; pi < players.size(); pi++) {
                    if (players.at(pi) == action->actor) continue;
                    if (const auto blocker = players.at(pi); action->blockedBy(blocker, block_cost)) {
                        if (players.at(pi) == action->target)
                            return action->target;

                        if (confirmAction(
                            std::format("Block {}", actor->getName()),
                            std::format(
                                "Player {} can block {} for {} coins.",
                                blocker->getName(), action->name, block_cost
                            )
                        ))
                            return blocker;
                    }
                }
            return nullptr;
        }

        void printAction(const Game::Action *action) {
            cout << action->actor->getName() << " used " << action->name;
            if (action->target != action->actor)
                cout << " on " << action->target->getName();
            cout << endl;
        }

        void printActionIllegal(const Game::Action *action, const illegal_action &why) { cout << why.what() << endl; }

        void printActionBlocked(const Game::Action *action, const PlayerRef blocker) {
            cout <<
                    action->name <<
                    " by " << action->actor->getName() <<
                    " on " << action->target->getName() <<
                    " ";
            blocker == action->target
                ? cout << "Failed"
                : cout << "was blocked by" << blocker->getName();
            cout << "!";
            cout << endl;
        }

        void printCoupForced(const PlayerRef actor) {
            std::cout << actor->getName() << " is forced to use coup." << std::endl;
        }
    }
} // game
