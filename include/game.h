// game header
#include "raylib.h"
#include "Window.h"
#include "Global.h"
#include "snake.h"
#include "AI_snake.h"
#include "player_snake.h"
#include "AVLTree.h"
#include <iostream>
#include <vector>
#ifndef GAME_H
#define GAME_H
class Game
{
private:
    Window window;
    Window leaderBoardWindow;
    std::vector<Snake *> snakes;
    AVLTree<global::LeaderboardEntry> leaderBoardTree;
    global::Map *map;
    short generation;

public:
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
    bool draw_start();
    void draw_end_game();
    void draw_leader_board();
    short get_max_score();
    short get_generation();
    AI_Snake get_max_wb();

};
#endif