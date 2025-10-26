#include <string>
#include <iostream>
#include <memory>

#include <lib/Game.h>
#include <algorithm>

int main() {
    Game game;

    bool my_turn = false;
    bool are_ships_placed = false;
    bool loaded_from_file = false;

    uint64_t count_of_ships = 0;
    uint64_t count_of_killed_ships = 0;
    std::pair<uint64_t, uint64_t> last_hit = {0, 0};

    while (true) {
        std::string cmd;
        std::cin >> cmd;
        if (cmd == "exit") {
            std::cout << "ok" << std::endl;
            return 0;
        } else if (cmd == "ping") {
            std::cout << "pong" << std::endl;
        } else if (cmd == "create") {
            std::string role;
            std::cin >> role;
            if (role == "master") {
                game.CreateMaster();
                std::cout << "ok" << std::endl;
                my_turn = false;
            } else if (role == "slave") {
                game.CreateSlave();
                std::cout << "ok" << std::endl;
                my_turn = true;
            } else {
                std::cout << "failed" << std::endl;
            }
        } else if (cmd == "start") {
            if (!loaded_from_file) {
                game.СonfigureGame();
                are_ships_placed = true;
            }
            if (!are_ships_placed) {
                game.PlaceShips();
                are_ships_placed = true;
            }
            if (game.Start(count_of_ships)) {
                std::cout << "ok" << std::endl;
            } else {
                std::cout << "failed" << std::endl;
            }
        } else if (cmd == "stop") {
            game.Stop();
            std::cout << "ok" << std::endl;
        } else if (cmd == "set") {
            std::string parameter;
            std::cin >> parameter;
            if (parameter == "width") {
                uint64_t width;
                std::cin >> width;
                if (game.SetWidth(width)) {
                    std::cout << "ok" << std::endl;
                } else {
                    std::cout << "failed" << std::endl;
                }
            } else if (parameter == "height") {
                uint64_t height;
                std::cin >> height;
                if (game.SetHeight(height)) {
                    std::cout << "ok" << std::endl;
                } else {
                    std::cout << "failed" << std::endl;
                }
            } else if (parameter == "count") {
                int input_num;
                uint64_t count;
                std::cin >> input_num >> count;
                uint8_t type = static_cast<uint8_t>(input_num);
                if (game.SetShipCount(type, count)) {
                    std::cout << "ok" << std::endl;
                    count_of_ships += count;
                } else {
                    std::cout << "failed" << std::endl;
                }
            } else if (parameter == "strategy") {
                std::string strategy;
                std::cin >> strategy;
                game.SetStrategy(strategy);
                std::cout << "ok" << std::endl;
            }
        } else if (cmd == "get") {
            std::string parameter;
            std::cin >> parameter;
            if (parameter == "width") {
                std::cout << game.GetWidth() << std::endl;
            } else if (parameter == "height") {
                std::cout << game.GetHeight() << std::endl;
            } else if (parameter == "count") {
                uint8_t type;
                std::cin >> type;
                std::cout << game.GetCount(type) << std::endl;
            }
        } else if (cmd == "dump") {
            std::string path;
            std::cin >> path;
            if (!loaded_from_file) {
                game.СonfigureGame();
                are_ships_placed = true;
            }
            game.Dump(path);
            std::cout << "ok" << std::endl;
        } else if (cmd == "load") {
            std::string path;
            std::cin >> path;
            if (game.Load(path)) {
                std::cout << "ok" << std::endl;
                loaded_from_file = true;
                are_ships_placed = true;
            } else {
                std::cout << "failed" << std::endl;
            }
        } else if (cmd == "shot") {
            if (!my_turn) {
                uint64_t x, y;
                std::cin >> x >> y;
                std::string res_shot = game.Shoot(x, y);
                std::cout << res_shot << "\n";
                if (res_shot == "miss") {
                    my_turn = true;
                    continue;
                }
            } else {
                std::pair<uint64_t, uint64_t> shot = game.MakeShot(last_hit);
                std::cout << shot.first << ' ' << shot.second << std::endl;
                std::cin >> cmd;
                std::cin >> cmd;
                std::cin >> cmd;
                if (cmd == "miss") {
                    my_turn = false;
                } else if (cmd == "hit") {
                    last_hit = shot;
                } else {
                    count_of_killed_ships++;
                    last_hit = {0, 0};
                }
            }
        } else if (cmd == "finished") {
            if (game.IsFinished(count_of_killed_ships, count_of_ships)) {
                std::cout << "yes" << std::endl;
                count_of_ships = 0;
            } else {
                std::cout << "no" << std::endl;
            }
        } else if (cmd == "win") {
            std::cout << (game.IsWin(count_of_killed_ships, count_of_ships)
                              ? "yes"
                              : "no") << std::endl;
        } else if (cmd == "lose") {
            std::cout << (game.IsLose(count_of_killed_ships, count_of_ships)
                              ? "yes"
                              : "no") << std::endl;
        } else {
            std::cout << "wrong command" << std::endl;
        }
    }
    return 0;
}
