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
        //Update and redrawl the game if it has not ended
        if (!game_ended) {
            game.update();
            game.draw();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(global::GAME::delay));
        //End game if all snakes are dead and we have not reached the maximum generations
        //then train the next generation if in training mode. If not in training mode end the game
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