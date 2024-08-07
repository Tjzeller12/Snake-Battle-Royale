#ifndef PLAYER_SNAKE_H
#define PLAYER_SNAKE_H

#include "snake.h"

class Player_Snake : public Snake
{
public:
    Player_Snake(global::Map &i_map);
    Player_Snake(global::Map &i_map, global::SNAKE::Direction direction, global::Position initial_pos);
    virtual void update_direction() override;
};

#endif