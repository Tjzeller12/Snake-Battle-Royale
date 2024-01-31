//game header
#include "raylib.h"
#include "Window.h"
#include "Global.h"
#include "snake.h"
#include <iostream>
#include <vector>
#ifndef GAME_H
#define GAME_H
class Game {
    private:
        Window window;
        //std::vector<std::shared_ptr<Snake>> snakes;
        std::vector<Snake*> snakes;
        global::Map* map;
        short generation;
    public:
    // creates game loop
        Game();
        ~Game();
        void init();
        void update();
        void draw();
        void close();
        bool shouldClose();
        void make_snakes();
        void train();
        void spawn_AI();
        bool are_snakes_dead();
        void end_game();
        void draw_end_game();
        short get_max_score();
        short get_generation();
        AI_Snake get_max_wb();
        
};
#endif