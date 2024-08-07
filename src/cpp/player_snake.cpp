#include "player_snake.h"
// Player snake constructors
Player_Snake::Player_Snake(global::Map &i_map) : Snake(i_map) {}
Player_Snake::Player_Snake(global::Map &i_map, global::SNAKE::Direction direction, global::Position initial_pos) : Snake(i_map, direction, initial_pos) {}
/*
Update direction based off the arrow keys (User input)
*/
void Player_Snake::update_direction()
{
    if (IsKeyPressed(KEY_UP) && this->direction != global::SNAKE::Down)
    {
        this->direction = global::SNAKE::Up;
    }
    else if (IsKeyPressed(KEY_DOWN) && this->direction != global::SNAKE::Up)
    {
        this->direction = global::SNAKE::Down;
    }
    else if (IsKeyPressed(KEY_LEFT) && this->direction != global::SNAKE::Right)
    {
        this->direction = global::SNAKE::Left;
    }
    else if (IsKeyPressed(KEY_RIGHT) && this->direction != global::SNAKE::Left)
    {
        this->direction = global::SNAKE::Right;
    }
}
