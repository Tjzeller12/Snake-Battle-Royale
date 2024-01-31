#ifndef MAP_FUNCTIONS_H
#define MAP_FUNCTIONS_H
#include "Global.h"
#include <iostream>
#include <vector>
#include <raylib.h>

namespace map_functions {
    inline global::Position get_random_empty_position(global::Map& map) {
        global::Position pos;
        do {
            pos.x = rand() % global::MAP::COLUMNS;
            pos.y = rand() % global::MAP::ROWS;
        } while (map.at(pos.x).at(pos.y).total_snake_bodies != 0 || map.at(pos.x).at(pos.y).is_food || map.at(pos.x).at(pos.y).is_wall);
        return pos;
    }
    inline void place_food(global::Map& map) {
        for (int i = 0; i < global::GAME::START_FOOD_COUNT; i++) {
            global::Position pos = get_random_empty_position(map);
            map[pos.x][pos.y].is_food = true;
        }
    }
    inline void place_walls(global::Map& map) {
        for(int i = 0; i < global::MAP::WALL_COUNT; i++) {
            int direction = rand() % 4;
            int j = 0;
            global::Position start_pos = get_random_empty_position(map);
            while (j < global::MAP::WALL_SIZE) {
                if (start_pos.x < 1 || start_pos.x >= global::MAP::COLUMNS - 1 || start_pos.y < 1 || start_pos.y >= global::MAP::ROWS - 1) {
                    break;
                }
                map[start_pos.x][start_pos.y].is_wall = true;
                switch(direction) {
                    case 1 : 
                        if(start_pos.y - 1 > 0) start_pos.y++;
                        break;
                    case 2 :
                        if(start_pos.y + 1 < global::MAP::ROWS - 1) start_pos.y--;
                        break;
                    case 3 :
                        if(start_pos.x - 1 > 0) start_pos.x--;
                        break;
                    case 4 :
                        if(start_pos.x - 1 < global::MAP::COLUMNS) start_pos.x--;
                        break;
                }
                j++;
            }
        }
    }
    inline void update_map(global::Map& map, std::vector<Snake*>& snakes) {
        // reset map
        for (int i = 0; i < global::MAP::COLUMNS; i++) {
            for (int j = 0; j < global::MAP::ROWS; j++) {
                map[i][j].total_snake_bodies = 0;
            }
        }
        // update map
        for (int i = 0; i < snakes.size(); i++) {
            snakes[i]->update_map();
        }
        
    }
};

#endif