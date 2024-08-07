#include "snake.h"
#include "raylib.h"
#include <eigen3/Eigen/Dense>
#include "Map_Functions.h"
#include <random>
#include <chrono>
#include "Model.h"
#include <cstring>
/*
Initialize map pointer, direction, place on map, and set food timer
*/
Snake::Snake(global::Map &i_map)
{
    map_ptr = &i_map;
    direction = global::SNAKE::Up;
    this->body.push_back(map_functions::get_random_empty_position(*map_ptr));
    food_timer = global::GAME::FOOD_TIMER_COUNT;
    score = 0;
}
/*
Overloaded constructor
*/
Snake::Snake(global::Map &i_map, global::SNAKE::Direction direction, global::Position initial_pos)
{
    map_ptr = &i_map;
    this->body.push_back(initial_pos);
    this->direction = direction;
    food_timer = global::GAME::FOOD_TIMER_COUNT;
    score = 0;
}
// Direction setter
void Snake::set_direction(global::SNAKE::Direction direction)
{
    this->direction = direction;
}

// Direction getter
global::SNAKE::Direction Snake::get_direction()
{
    return this->direction;
}
// Score getter
short Snake::get_score()
{
    return this->score;
}
// Score setter
void Snake::set_score(short scr)
{
    this->score = scr;
}
// Color getter
Color Snake::get_color()
{
    return this->color;
}
// Color setter
void Snake::set_color(Color color)
{
    this->color = color;
}
// Name setter
void Snake::set_name(char name[])
{
    std::strncpy(this->name, name, sizeof(this->name) - 1);
    this->name[sizeof(this->name) - 1] = '\0';
}
// Name getter
const char *Snake::get_name()
{
    return this->name;
}
// For every body part in the body vector, draw a square on map that is the color of the snake
void Snake::draw()
{
    // draw snake
    for (int i = 0; i < body.size(); i++)
    {
        DrawRectangle(body[i].x * global::MAP::CELL_SIZE, body[i].y * global::MAP::CELL_SIZE, global::MAP::CELL_SIZE, global::MAP::CELL_SIZE, this->get_color());
    }
}
/*
Starting at the snakes tail set every body part equal to the
one that preceded it in the vector. Update the head to be the adjacent cell in the
direction the snake is moving in.
*/
void Snake::move()
{
    for (int i = body.size() - 1; i > 0; i--)
    {
        body[i] = body[i - 1];
    }
    switch (direction)
    {
    case global::SNAKE::Up:
        body[0].y -= 1;
        break;
    case global::SNAKE::Down:
        body[0].y += 1;
        break;
    case global::SNAKE::Left:
        body[0].x -= 1;
        break;
    case global::SNAKE::Right:
        body[0].x += 1;
        break;
    }
}
// Iterate through each body part and increment the total snake bodies that are in that position on the map.
void Snake::update_map()
{
    if (!this->is_dead())
    {
        for (int i = 0; i < body.size(); i++)
        {
            (*map_ptr)[body[i].x][body[i].y].total_snake_bodies++;
        }
    }
}
/*
Add a body part in the oposite direction of the direction the snake is moving in.
*/
void Snake::grow()
{
    // grow snake based on direction
    switch (direction)
    {
    case global::SNAKE::Up:
        body.push_back(global::Position(body[body.size() - 1].x, body[body.size() - 1].y + 1));
        break;
    case global::SNAKE::Down:
        body.push_back(global::Position(body[body.size() - 1].x, body[body.size() - 1].y - 1));
        break;
    case global::SNAKE::Left:
        body.push_back(global::Position(body[body.size() - 1].x + 1, body[body.size() - 1].y));
        break;
    case global::SNAKE::Right:
        body.push_back(global::Position(body[body.size() - 1].x - 1, body[body.size() - 1].y));
        break;
    }
}
/*
If the food timer is 0 or the head of the snake hits a wall or another snake, then return true.
Otherwise return false.
*/
bool Snake::is_dead()
{
    // check if snake is dead
    if (this->body.size() <= 0)
        return true;
    if (!in_map(this->body[0]))
        return true;
    if ((*map_ptr)[this->body[0].x][this->body[0].y].is_wall || (*map_ptr)[this->body[0].x][this->body[0].y].total_snake_bodies > 1 || this->food_timer <= 0)
    {
        return true;
    }
    return false;
}
// Sets the timer back to its original value
void Snake::reset_timer()
{
    food_timer = global::GAME::FOOD_TIMER_COUNT;
}
/*
If the head of the snake is in a position that has food: increase score, reset the food timer, remvoe the food off the map
If there is no food then decrement the food timer.
*/
void Snake::try_to_eat()
{

    if ((*map_ptr)[this->body[0].x][this->body[0].y].is_food)
    {
        this->grow();
        (*map_ptr)[this->body[0].x][this->body[0].y].is_food = false;
        global::Position new_food_pos = map_functions::get_random_empty_position(*map_ptr);
        (*map_ptr)[new_food_pos.x][new_food_pos.y].is_food = true;
        this->reset_timer();
        this->score++;
    }
    else
    {
        this->food_timer--;
    }
}
/*
Reset kills the snake by removing every element from the body vector.
*/
void Snake::reset()
{
    // reset snake
    body.clear();
}
// Body getter
std::vector<global::Position> &Snake::get_body()
{
    return body;
}
// checks if snake is within the bounderies and returns the result
bool Snake::in_map(global::Position pos)
{
    return 0 <= pos.x && 0 <= pos.y && global::MAP::COLUMNS > pos.x && global::MAP::ROWS > pos.y;
}
// Compares the snakes scores. If they are tied then it compares their names
int Snake::compare_to(Snake *snake)
{
    if (this->get_score() != snake->get_score())
    {
        return this->get_score() - snake->get_score();
    }
    else
    {
        return strcmp(this->get_name(), snake->get_name());
    }
}
