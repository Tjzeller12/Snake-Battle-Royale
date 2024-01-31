#include "raylib.h"
#include "Window.h"
#include "game.h"
#include <thread>
#include <chrono>
int main() {
    // Initialization
    Game game = Game();
    game.init();
    bool game_ended = false;
    // Main game loop
    while (!game.shouldClose()) {
        if (!game_ended) {
            game.update();
            game.draw();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        if(game.are_snakes_dead() && game.get_generation() <= global::TRAIN::GENERATIONS) {
            if(global::GAME::TRAINING_MODE) {
                game.train();
            } else {
                game_ended = true;
                game.end_game();
            }
        }
    }
    // De-Initialization
    return 0;
}