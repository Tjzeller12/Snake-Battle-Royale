#ifndef MAP_FUNCTIONS_H
#define MAP_FUNCTIONS_H
#include "Global.h"
#include <iostream>
#include <vector>
#include <raylib.h>
#include <random>

namespace map_functions
{
    static std::mt19937 rng(std::random_device{}());

    // Finds a random position on the map that does not have a wall, food, or snake and returns it
    inline global::Position get_random_empty_position(global::Map &map)
    {
        std::uniform_int_distribution<int> dist_x(0, global::MAP::COLUMNS - 1);
        std::uniform_int_distribution<int> dist_y(0, global::MAP::ROWS - 1);
        global::Position pos;
        do
        {
            pos.x = dist_x(rng);
            pos.y = dist_y(rng);
        } while (map.at(pos.x).at(pos.y).total_snake_bodies != 0 || map.at(pos.x).at(pos.y).is_food || map.at(pos.x).at(pos.y).is_wall);
        return pos;
    }
    // Finds an empty position and places food in it
    inline void place_food(global::Map &map)
    {
        for (int i = 0; i < global::GAME::START_FOOD_COUNT; i++)
        {
            global::Position pos = get_random_empty_position(map);
            map[pos.x][pos.y].is_food = true;
        }
    }
    // Randomly generates walls around the map
    inline void place_walls(global::Map &map)
    {
        std::uniform_int_distribution<int> direction_dist(0, 3);
        for (int i = 0; i < global::MAP::WALL_COUNT; i++)
        {
            int direction = direction_dist(rng);
            int j = 0;
            global::Position start_pos = get_random_empty_position(map);
            // Builds a wall in a random direction until it is the size of the "WALL_SIZE" variable
            while (j < global::MAP::WALL_SIZE)
            {
                // If outside of map then break
                if (start_pos.x < 1 || start_pos.x >= global::MAP::COLUMNS - 1 || start_pos.y < 1 || start_pos.y >= global::MAP::ROWS - 1)
                {
                    break;
                }
                // Set is wall to true at current positon
                map[start_pos.x][start_pos.y].is_wall = true;
                switch (direction)
                { // check to see if the next position is in the map and increment in that direction
                case 1:
                    if (start_pos.y - 1 > 0)
                        start_pos.y++;
                    break;
                case 2:
                    if (start_pos.y + 1 < global::MAP::ROWS - 1)
                        start_pos.y--;
                    break;
                case 3:
                    if (start_pos.x - 1 > 0)
                        start_pos.x--;
                    break;
                case 4:
                    if (start_pos.x - 1 < global::MAP::COLUMNS)
                        start_pos.x--;
                    break;
                }
                j++;
            }
        }
    }
    /*
        Resets the entire map by setting total snake bodies equal to zero for each cell. Then updates the map
        by calling update_map on each snake.
    */
    inline void update_map(global::Map &map, std::vector<Snake *> &snakes)
    {
        // reset map
        for (int i = 0; i < global::MAP::COLUMNS; i++)
        {
            for (int j = 0; j < global::MAP::ROWS; j++)
            {
                map[i][j].total_snake_bodies = 0;
            }
        }
        // update map
        for (int i = 0; i < snakes.size(); i++)
        {
            snakes[i]->update_map();
        }
    }
};

#endif