#ifndef SNAKE_H
#define SNAKE_H
#include "raylib.h"
#include "Window.h"
#include "Global.h"
#include <iostream>
// Snake class with coordinates and direction
class Snake
{
protected:
    std::vector<global::Position> body;
    global::SNAKE::Direction direction;
    global::Map *map_ptr;
    short score = 0;
    short food_timer;
    Color color;
    char name[15];

public:
    Snake(global::Map &i_map);
    Snake(global::Map &i_map, global::SNAKE::Direction direction, global::Position initial_pos);

    // Ensures correct destructor is called for dirived classes
    virtual ~Snake() = default;

    void set_direction(global::SNAKE::Direction direction);
    void set_color(Color color);
    Color get_color();
    void set_name(char name[]);
    const char *get_name();
    void move();
    void reset();
    virtual void update_direction() = 0;
    void update_status();
    void update_map();
    void grow();
    std::vector<global::Position> &get_body();
    short get_score();
    void set_score(short scr);
    bool is_dead();
    void try_to_eat();
    global::SNAKE::Direction get_direction();
    void draw();
    bool in_map(global::Position pos);
    void reset_timer();
    int compare_to(Snake *snake);
};
#endif