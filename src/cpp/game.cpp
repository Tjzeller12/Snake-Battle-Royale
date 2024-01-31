#include "game.h"
#include "raylib.h"
#include "Window.h"
#include <iostream>
#include "snake.h"
#include "Global.h"
#include "Map_Functions.h"
#include "eigen3/Eigen/Dense"
#include <cstdio>
using std::unordered_map;
using std::vector;
// The game will have 10 snakes with 10 different colors and 10 different starting positions
// The snakes will be 4 blocks long
Game::Game() {
    window = Window(global::WINDOW::width, global::WINDOW::height, "Snake Battle Royale");
    // Draw the map and set walls
    map = new global::Map();
    // Set all of the walls
    map_functions::place_walls(*map);
    map_functions::place_food(*map);
    for(int i = 0; i < global::MAP::COLUMNS; i++) {
        (*map)[i][0].is_wall = true;
        (*map)[i][global::MAP::ROWS - 1].is_wall = true;
    }
    for(int i = 0; i < global::MAP::ROWS; i++) {
        (*map)[0][i].is_wall = true;
        (*map)[global::MAP::COLUMNS - 1][i].is_wall = true;
    }
}
Game::~Game() {
    // De-Initialization
    delete map;
    map = nullptr;
    this->close();
    for (Snake* snake_ptr : snakes) {
        delete snake_ptr;
        snake_ptr = nullptr;
    }
}
void Game::init() {
    // Initialization
    window.init();
    window.positionWindowOnSecondMonitor();
    window.setTargetFPS(60);
    this->make_snakes();
}
void Game::update() {
    // Update
    map_functions::update_map(*map, snakes);
    for (Snake* snake : snakes) {
        if(!snake->is_dead()) {
        snake->update_direction();
        snake->try_to_eat();
        snake->move(); 
        } else {
            snake->reset();
        }
    }
    // if(are_snakes_dead() && generation <= global::TRAIN::GENERATIONS) {
    //     if(global::GAME::TRAINING_MODE) {
    //         train();
    //     } else {
    //         end_game();
    //     }
    // }
}
void Game::draw() {
    window.beginDrawing();
    ClearBackground(Color{1, 0, 30, 255});
    for(int i = 0; i < global::MAP::COLUMNS; i++) {
        for(int j = 0; j < global::MAP::ROWS; j++) {
            if((*map)[i][j].is_wall) {
                DrawRectangle(i * global::MAP::CELL_SIZE, j * global::MAP::CELL_SIZE, global::MAP::CELL_SIZE, global::MAP::CELL_SIZE, Color{3, 0, 60, 255});
            } else if((*map)[i][j].is_food) {
                DrawRectangle(i * global::MAP::CELL_SIZE, j * global::MAP::CELL_SIZE, global::MAP::CELL_SIZE, global::MAP::CELL_SIZE, RED);
            }
        }
    }
    for (int i = 0; i < window.getScreenWidth(); i += 10) {
        DrawLine(i, 0, i, window.getScreenHeight(), Color{3, 0, 60, 255});
    }
    for (int i = 0; i < window.getScreenHeight(); i += 10) {
        DrawLine(0, i, window.getScreenWidth(), i, Color{3, 0, 60, 255});
    }
    //Add shadows on the boarders
    int shadow_width = 1;
    float shadow_intensity = 0.08;
    int max_distance = 260;
    for (int i = 0; i < max_distance; i++) {
        // make color rgb 0, 0, 0, 0.2
        float alpha = shadow_intensity * (1 - ((float)i / max_distance));
        DrawRectangle(0, i, window.getScreenWidth(), shadow_width, Fade(BLACK, alpha));
        DrawRectangle(i, 0, shadow_width, window.getScreenHeight(), Fade(BLACK, alpha));
        DrawRectangle(window.getScreenWidth() - i, 0, shadow_width, window.getScreenHeight(), Fade(BLACK, alpha));
        DrawRectangle(0, window.getScreenHeight() - i, window.getScreenWidth(), shadow_width, Fade(BLACK, alpha));   
    }
    //Snake player_snake = Snake(60, 6, 0);
    snakes[0]->draw(Color{0, 255, 0, 255});
    for (int i = 1; i <= global::SNAKE::START_SNAKE_AI_COUNT; i++) {
        // draw snake
        snakes[i]->draw(Color{0, static_cast<unsigned char>(i * (180/global::SNAKE::START_SNAKE_AI_COUNT) + 50), 0, 255});
        
    }
    window.endDrawing();
}
void Game::make_snakes() {
    global::Position player_position(60, 6);
    // auto player_snake = std::make_shared<Player_Snake>(*map, global::SNAKE::Up, player_position);
    Snake* player_snake = new Player_Snake(*map, global::SNAKE::Down, player_position);
    //(*map)[player_position.x][player_position.y].total_snake_bodies ++;
    snakes.push_back(player_snake);
    for (int i = 1; i <= global::SNAKE::START_SNAKE_AI_COUNT; i++) {
        global::Position current_pos = map_functions::get_random_empty_position(*map);
        // auto current_snake = std::make_shared<AI_Snake>(*map, global::SNAKE::Down, current_pos);
        Snake* current_snake = new AI_Snake(*map, global::SNAKE::Down, current_pos);
        //(*map)[current_pos.x][current_pos.y].total_snake_bodies ++;
        snakes.push_back(current_snake);
    }

}
short Game::get_generation() {
    return generation;
}
void Game::close() {
    // De-Initialization
    window.close();
}
bool Game::shouldClose() {
    return window.shouldClose();
}
void Game::train() {
    AI_Snake parent1 = get_max_wb();
    AI_Snake parent2 = get_max_wb();

    //print parent1 weights and biases
    std::cout << "Hidden Layer weights: " << parent1.get_weights1() << " Output Layer weights: " << parent1.get_weights2() << "\n";
    std::cout << "Hidden Layer bias: " << parent1.get_bias1() << " Outpute Layer bias: " << parent1.get_bias2() << "\n";
    // set new weights and biases for all snakes
    for(Snake* snake : snakes) {
        AI_Snake* ai_snake = dynamic_cast<AI_Snake*>(snake);
        if(ai_snake != nullptr) {
            ai_snake->set_child_weights_bias(parent1, parent2);
        }
    }
    // respond snakes
    spawn_AI();
    generation++;
}
void Game::end_game() {
    if(are_snakes_dead()) {
        this->draw_end_game();
    }
}
void Game::draw_end_game() {
    BeginDrawing();
    ClearBackground(Color{3, 0, 60, 255});
    const char* victory_text = "Victory";
    const char* defeat_text = "Defeat";
    short player_score = snakes[0]->get_score();
    short AI_score = get_max_score();
    int vict_text_width = MeasureText(victory_text, 60);
    int def_text_width = MeasureText(defeat_text, 60);
    char player_score_text[50];
    sprintf(player_score_text, "Player Score: %d", player_score);
    int ps_text_width = MeasureText(player_score_text, 40);
    char AI_score_text[50];
    sprintf(AI_score_text, "Top AI Score: %d", AI_score);
    int AIs_text_width = MeasureText(AI_score_text, 40);
    if (player_score >= AI_score) {
        DrawText(victory_text, global::WINDOW::width/2 - vict_text_width/2, global::WINDOW::height/4, 60, GREEN);
    } else {
        DrawText(defeat_text, global::WINDOW::width/2 - vict_text_width/2, global::WINDOW::height/4, 60, RED);
    }
    DrawText(player_score_text, global::WINDOW::width/2 - ps_text_width/2, global::WINDOW::height * 2/4, 40, GREEN);
    DrawText(AI_score_text, global::WINDOW::width/2 - AIs_text_width/2, global::WINDOW::height * 3/4, 40, RED);
    EndDrawing();
}
short Game::get_max_score() {
    short max = 0;
    for(int i = 1; i < snakes.size(); i++) {
        max = std::max(max, this->snakes[i]->get_score());
    }
    return max;
}
void Game::spawn_AI() {
     for (int i = 1; i <= global::SNAKE::START_SNAKE_AI_COUNT; i++) {
        global::Position current_pos = map_functions::get_random_empty_position(*map);
        snakes[i]->reset_timer();
        snakes[i]->get_body().clear();
        snakes[i]->get_body().push_back(current_pos);
     }
}

bool Game::are_snakes_dead() {
    for (int i = 0; i <= global::SNAKE::START_SNAKE_AI_COUNT; i++) {
        if (!(snakes[i]->is_dead())) return false;
    }
    return true;
}

AI_Snake Game::get_max_wb() {
    int max_score = 0;
    int max_index = 0;
    for(size_t i = 1; i < snakes.size(); i++) {
        if(dynamic_cast<AI_Snake*>(snakes[i])->get_score() > max_score) {
            max_index = i;
            max_score = dynamic_cast<AI_Snake*>(snakes[i])->get_score();
        }
    }
    AI_Snake max_snake = *(dynamic_cast<AI_Snake*>(snakes[max_index]));
    snakes[max_index]->set_score(0); // Make this no longer the max score to find the next parent
    return max_snake;
    
}