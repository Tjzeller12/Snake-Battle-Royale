#pragma once
#include <iostream>
#include <array>
#include <vector>
#include <eigen3/Eigen/Dense>
namespace global
{
    namespace GAME
    {
        constexpr unsigned short START_FOOD_COUNT = 60;
        constexpr unsigned short FOOD_TIMER_COUNT = 300;
        extern bool TRAINING_MODE;
        extern short delay;
    }
    namespace TRAIN
    {
        constexpr unsigned short GENERATIONS = 100;
        constexpr double MUTATION_RATE = 0.05;
        constexpr int HL_COUNT = 12;

    }

    namespace MAP
    {
        constexpr unsigned short ROWS = 70;
        constexpr unsigned short COLUMNS = 120;
        constexpr unsigned short CELL_SIZE = 10;
        constexpr unsigned short WALL_COUNT = 10;
        constexpr unsigned short WALL_SIZE = 5;
        // extern std::vector<std::vector<short>> map_layout;
    }

    namespace SNAKE
    {
        constexpr unsigned short START_LENGTH = 5;
        constexpr unsigned short START_SPEED = 5;
        constexpr unsigned short START_SNAKE_AI_COUNT = 60;
        enum Direction
        {
            Right,
            Up,
            Left,
            Down
        };
    }
    namespace WINDOW
    {
        constexpr unsigned short height = 700;
        constexpr unsigned short width = 1200;
        // constexpr unsigned short LBheight = 15 * SNAKE::START_SNAKE_AI_COUNT + 15;
        constexpr unsigned short LBwidth = 175;
    }
    struct Position
    {
        short x;
        short y;
        Position(const short i_x = 0, const short i_y = 0) : x(i_x), y(i_y) {}

        bool operator==(const Position &i_position)
        {
            return x == i_position.x && y == i_position.y;
        }
        bool operator!=(const Position &i_positon)
        {
            return x != i_positon.x || y != i_positon.y;
        }
    };

    struct Cell
    {
        bool is_food;
        bool is_wall;
        unsigned short total_snake_bodies;
        Cell()
        {
            is_food = false;
            is_wall = false;
            total_snake_bodies = 0;
        }
        Cell(bool i_is_wall, bool i_is_food, unsigned short i_total_snake_bodies) : is_wall(i_is_wall), is_food(i_is_food), total_snake_bodies(i_total_snake_bodies) {}
    };
    using Map = std::array<std::array<global::Cell, global::MAP::ROWS>, global::MAP::COLUMNS>;
}