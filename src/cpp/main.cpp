#include "raylib.h"
#include "Window.h"
#include "game.h"
#include "AVLTree.h"
#include "Global.h"
#include <thread>
#include <chrono>
int main()
{
    // Test AVL Tree
    AVLTree<global::LeaderboardEntry> testTree;
    testTree.insert(global::LeaderboardEntry("snake1", 10, Color{1, 0, 30, 255}));
    testTree.insert(global::LeaderboardEntry("snake2", 0, Color{1, 0, 30, 255}));
    testTree.insert(global::LeaderboardEntry("snake3", 5, Color{1, 0, 30, 255}));
    testTree.printTree();
    testTree.remove(global::LeaderboardEntry("snake3", 5, Color{1, 0, 30, 255}));
    testTree.printTree();
    testTree.remove(global::LeaderboardEntry("snake2", 0, Color{1, 0, 30, 255}));
    testTree.printTree();
    // Initialization
    Game game;
    game.init();
    bool game_ended = false;
    // Main game loop
    while (!game.shouldClose())
    {
        // Update and redrawl the game if it has not ended
        if (!game_ended)
        {
            game.update();
            game.draw();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(global::GAME::delay));
        // End game if all snakes are dead and we have not reached the maximum generations
        // then train the next generation if in training mode. If not in training mode end the game
        if (game.are_snakes_dead() && game.get_generation() <= global::TRAIN::GENERATIONS)
        {
            if (global::GAME::TRAINING_MODE)
            {
                game.train();
            }
            else
            {
                game_ended = true;
                game.end_game();
            }
        }
    }
    // De-Initialization
    return 0;
}