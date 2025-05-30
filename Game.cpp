//
// Created by Aviad Levine on 14/05/2025.
//

#include <iostream>

#include "GameActions.hpp"
#include "Game.hpp"

#include <ranges>

#include "typetools.hpp"

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

    auto Game::availableActionsFor(const PlayerRef player) {
        // map action to input key for selection
        std::vector<std::pair<Action *, char> > a;
        Game &game = *this;

        a.emplace_back(std::pair<Action *, char>(new Coup(player, nullptr, game), 'c'));

        a.emplace_back(std::pair<Action *, char>(new Gather(player, game), 'g'));
        a.emplace_back(std::pair<Action *, char>(new Tax(player, game), 't'));
        a.emplace_back(std::pair<Action *, char>(new Invest(player, game), 'i'));

        a.emplace_back(std::pair<Action *, char>(new Bribe(player, game), 'b'));

        a.emplace_back(std::pair<Action *, char>(new Sanction(player, nullptr, game), 's'));
        a.emplace_back(std::pair<Action *, char>(new Arrest(player, nullptr, game), 'a'));

        a.emplace_back(std::pair<Action *, char>(new Block(player, nullptr, game), 'l'));
        a.emplace_back(std::pair<Action *, char>(new Peek(player, nullptr, game), 'p'));

        // remove un-available actions
        for (auto it = a.begin(); it != a.end();) {
            try {
                it->first->assertActorValid();
                ++it; // move up
            } catch (illegal_action) {
                const auto tmp = it->first;
                it = a.erase(it); // erase and move to the next valid element
                delete tmp; // delete memory
            }
        }

        return a;
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

    void Game::removePlayer(const PlayerRef player) {
        if (removeValue<PlayerRef>(players, player))
            delete player;
        if (player == target_player) target_player = nullptr;
    }

    void Game::removePlayer(const int pi) {
        if (pi >= players.size() || pi < 0) return;
        if (players.at(pi) == target_player) target_player = nullptr;
        delete players.at(pi);
        players.erase(players.begin() + pi);
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


        void addPlayer(Game &game) {
            int c;
            std::string name;
            PlayerRef p = nullptr;

            std::cout << "Enter Player Name: ";
            std::cin >> name;

            std::cout << "Choose Role:\n0. Governor\t1. Spy\t2. Baron\t3. General\t4. Judge\t5. Merchant\n";
            std::cin >> c;
            switch (c) {
                case 0: {
                    p = new Governor();
                    break;
                }
                case 1: {
                    p = new Spy();
                    break;
                }
                case 2: {
                    p = new Baron();
                    break;
                }
                case 3: {
                    p = new General();
                    break;
                }
                case 4: {
                    p = new Judge();
                    break;
                }
                case 5: {
                    p = new Merchant();
                    break;
                }
                default: p = nullptr;
            }
            if (p != nullptr) {
                p->setName(name);
                game.addPlayer(p);
            }
        }

        void removePlayer(Game &game) {
            int in;
            std::cout << "Select Player # to remove: ";
            std::cin >> in;
            if (in >= 0 && in < game.getPlayers().size())
                game.removePlayer(in);
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
                    "Sc Ar Bl" << " " << "Coins" <<
                    endl;
            cout <<
                    "   " << string(cw_name, '-') << pads << string(cw_role, '-') <<
                    pads << "-  -  - " << " " << "-----" <<
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
                        (p->isArrested() ? "x " : "  ") << " " <<
                        (p->isBlocked() ? "x " : "  ") << " ";
                if (p->isHandShown() || isPC)
                    cout <<
                            p->getCoins();
                cout << endl;
            }
            cout << endl;
        }

        void printCoupForced(const PlayerRef actor) {
            std::cout << actor->getName() << " is forced to use coup." << std::endl;
        }

        void printWin(const PlayerRef winner) { cout << winner->getName() << " wins!" << endl; }


        PlayerRef chooseTarget(const PlayerList &players) {
            cout << "Pick Target: [0-" << players.size() - 1 << "]" << endl;
            int ti = 0;
            cin >> ti;
            return players.at(ti);
        }

        Action *chooseAction(const PlayerRef player, Game &game) {
            const auto actions = game.availableActionsFor(player);
            Action *action = nullptr;
            char act_in;

            cout <<
                    "Choose action: " <<
                    "x:END-TURN";
            for (auto [a, c]: actions) {
                cout << "    " << c << ":" << a->name;
                if (const auto cost = a->coinCost(); cost>0)
                    cout<< " " << a->coinCost();
            }
            cout << endl;

            // input selection
            cin >> act_in;

            // pick action matching selected input
            if (act_in == 'x') { player->endTurn(); } else
                for (auto [a, c]: actions)
                    if (act_in == c) {
                        action = a;
                        break;
                    }
            // free up map
            for (const auto a: actions | std::views::keys)
                if (a != action)
                    delete a;
            return action;
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
    }
} // game
