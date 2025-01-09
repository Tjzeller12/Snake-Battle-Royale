#include "game.h"
#include "Window.h"
#include <iostream>
#include "Map_Functions.h"
#include "eigen3/Eigen/Dense"
#include <cstdio>
using std::unordered_map;
using std::vector;
namespace global
{
    namespace GAME
    {
        bool TRAINING_MODE = false;
        short delay = 125;
    }
}
/*
Create a new window. Make a mpa. Then place objects on the map
*/
Game::Game()
{
    generation = 0;
    window = Window(global::WINDOW::width + global::WINDOW::LBwidth, global::WINDOW::height, "Snake Battle Royale");
    // leaderBoardWindow = Window(global::WINDOW::LBwidth, global::WINDOW::LBheight, "Leader Board");
    //  Draw the map and set walls
    map = new global::Map();
    // Set all of the walls
    map_functions::place_walls(*map);
    map_functions::place_food(*map);
    // Make the boarders of the map
    for (int i = 0; i < global::MAP::COLUMNS; i++)
    {
        (*map)[i][0].is_wall = true;
        (*map)[i][global::MAP::ROWS - 1].is_wall = true;
    }
    for (int i = 0; i < global::MAP::ROWS; i++)
    {
        (*map)[0][i].is_wall = true;
        (*map)[global::MAP::COLUMNS - 1][i].is_wall = true;
    }
}
/*
Delete map pointer and every snake pointer.
*/
Game::~Game()
{
    // De-Initialization
    delete map;
    map = nullptr;
    this->close();
    for (Snake *snake_ptr : snakes)
    {
        delete snake_ptr;
        snake_ptr = nullptr;
    }
}
/*
Initializes the windows and the snakes.
*/
void Game::init()
{
    // Initialization
    window.init();
    window.positionWindowOnSecondMonitor();
    window.setTargetFPS(60);

    bool game_started = false;
    // draw game startscreen until user select mode
    while (!game_started)
    {
        game_started = draw_start();
    }
    this->make_snakes();
}
/*
Updates the map and the snakes.
*/
void Game::update()
{
    // Update
    map_functions::update_map(*map, snakes);
    for (Snake *snake : snakes)
    {
        if (!snake->is_dead())
        { // If the snake is not dead update its direction, make it eat, and move
            snake->update_direction();
            snake->try_to_eat(leaderBoardTree);
            snake->move();
        }
        else
        {
            snake->reset(); // Remove the snake if it is dead
        }
    }
}
/*
This fucntion draws the game. If first draws the food and walls. It then draws the grid lines. Then it draws
the shadows along the outside of the game. Then the snakes are assigned a color and drawled.
*/
void Game::draw()
{
    window.beginDrawing();
    ClearBackground(Color{1, 0, 30, 255});
    // Draw food and walls
    for (int i = 0; i < global::MAP::COLUMNS; i++)
    {
        for (int j = 0; j < global::MAP::ROWS; j++)
        {
            if ((*map)[i][j].is_wall)
            {
                DrawRectangle(i * global::MAP::CELL_SIZE, j * global::MAP::CELL_SIZE, global::MAP::CELL_SIZE, global::MAP::CELL_SIZE, Color{3, 0, 60, 255});
            }
            else if ((*map)[i][j].is_food)
            {
                DrawRectangle(i * global::MAP::CELL_SIZE, j * global::MAP::CELL_SIZE, global::MAP::CELL_SIZE, global::MAP::CELL_SIZE, RED);
            }
        }
    }
    // Draw grid lines
    for (int i = 0; i < global::WINDOW::width; i += 10)
    {
        DrawLine(i, 0, i, global::WINDOW::height, Color{3, 0, 60, 255});
    }
    for (int i = 0; i < global::WINDOW::height; i += 10)
    {
        DrawLine(0, i, global::WINDOW::width, i, Color{3, 0, 60, 255});
    }
    // Add shadows on the boarders
    int shadow_width = 1;
    float shadow_intensity = 0.08;
    int max_distance = 260;
    for (int i = 0; i < max_distance; i++)
    {
        // make color rgb 0, 0, 0, 0.2
        float alpha = shadow_intensity * (1 - ((float)i / max_distance));
        DrawRectangle(0, i, global::WINDOW::width, shadow_width, Fade(BLACK, alpha));
        DrawRectangle(i, 0, shadow_width, global::WINDOW::height, Fade(BLACK, alpha));
        DrawRectangle(global::WINDOW::width - i, 0, shadow_width, global::WINDOW::height, Fade(BLACK, alpha));
        DrawRectangle(0, global::WINDOW::height - i, global::WINDOW::width, shadow_width, Fade(BLACK, alpha));
    }

    // Draw and set AI snakes colors
    for (int i = 0; i < snakes.size(); i++)
    {
        snakes[i]->draw();
    }
    // Draw leaderboard
    std::vector<global::LeaderboardEntry> leaderboard = leaderBoardTree.toVector();
    for (int i = 0; i < leaderboard.size(); i++) {
        // Use shared_ptr directly instead of raw pointer
        global::LeaderboardEntry currSnake = leaderboard[i];
        Color rectangleColor;

        // Snake blocks will be on the right of the game, and they will take up an equal amount of space that depends on the amount of snakes we have
        Rectangle currentSnakeBlock = {
            global::WINDOW::width,
            static_cast<float>(global::WINDOW::height / leaderboard.size()) * i,
            global::WINDOW::LBwidth,
            static_cast<float>(global::WINDOW::height / leaderboard.size())
        };

        // Have every other row be a different color
        if (i % 2 == 0) {
            rectangleColor = Color{1, 0, 30, 255};
        } else {
            rectangleColor = Color{3, 0, 60, 255};
        }

        DrawRectangleRec(currentSnakeBlock, rectangleColor);

        // Draw the snake's color, name, and score in the snake block
        DrawText(currSnake.name, currentSnakeBlock.x + 12, currentSnakeBlock.y, 5, WHITE);

        char scoreText[50];
        snprintf(scoreText, sizeof(scoreText), "%d", currSnake.score);
        DrawText(scoreText, currentSnakeBlock.x + 150, currentSnakeBlock.y, 5, WHITE);

        // Draw snake color
        DrawRectangle(
            global::WINDOW::width + 1,
            ((global::WINDOW::height / leaderboard.size()) * i) + 1,
            (global::WINDOW::height / leaderboard.size()) - 2,
            (global::WINDOW::height / leaderboard.size()) - 2,
            currSnake.color
        );
    }
    window.endDrawing();

}


/*
Initializes every snake and appends them to the snakes vector.
*/
void Game::make_snakes()
{
    // Make player snake
    global::Position player_position(60, 6);
    Snake *player_snake = new Player_Snake(*map, global::SNAKE::Down, player_position);
    player_snake->set_name("Player Snake");
    player_snake->set_color(Color{0, 255, 0, 255});
    snakes.push_back(player_snake);
    leaderBoardTree.insert(global::LeaderboardEntry(player_snake->get_name(), player_snake->get_score(), player_snake->get_color()));
    char buffer[15];
    // Make AI snakes
    for (int i = 1; i <= global::SNAKE::START_SNAKE_AI_COUNT; i++)
    {
        global::Position current_pos = map_functions::get_random_empty_position(*map);
        Snake *current_snake = new AI_Snake(*map, global::SNAKE::Down, current_pos);
        snprintf(buffer, sizeof(buffer), "AI snake %d", i);
        current_snake->set_name(buffer);
        current_snake->set_color(Color{0, static_cast<unsigned char>(i * (180 / global::SNAKE::START_SNAKE_AI_COUNT) + 50), 0, 255});
        snakes.push_back(current_snake);
        leaderBoardTree.insert(global::LeaderboardEntry(current_snake->get_name(), current_snake->get_score(), current_snake->get_color()));
    }
}
// Getter for generation
short Game::get_generation()
{
    return generation;
}
// Closes the window
void Game::close()
{
    // De-Initialization
    window.close();
    // leaderBoardWindow.close();
}
// Checks to see if the window should close
bool Game::shouldClose()
{
    return window.shouldClose();
}
void Game::train()
{
    AI_Snake parent1 = get_max_wb();
    AI_Snake parent2 = get_max_wb();

    // print parent1 weights and biases
    std::cout << "Hidden Layer weights: " << parent1.get_weights1() << " Output Layer weights: " << parent1.get_weights2() << "\n";
    std::cout << "Hidden Layer bias: " << parent1.get_bias1() << " Outpute Layer bias: " << parent1.get_bias2() << "\n";
    // set new weights and biases for all snakes
    for (Snake *snake : snakes)
    {
        AI_Snake *ai_snake = dynamic_cast<AI_Snake *>(snake);
        if (ai_snake != nullptr)
        {
            ai_snake->set_child_weights_bias(parent1, parent2);
        }
    }
    // reset scores
    for (int i = 0; i < snakes.size(); i++)
    {
        leaderBoardTree.remove(global::LeaderboardEntry(snakes[i]->get_name(), snakes[i]->get_score(), snakes[i]->get_color()));
        snakes[i]->set_score(0);
        leaderBoardTree.insert(global::LeaderboardEntry(snakes[i]->get_name(), snakes[i]->get_score(), snakes[i]->get_color()));
    }
    // respond snakes
    spawn_AI();
    generation++;
}
/*
This draws the start screen. The start screen is made up of two buttons. There is a play game button that allows the
user to play against snake AI's that have predetermined weights and biases. There is also a training mode button that
allows the user to train the snakes that start with random weights. After each generation the top snakes weights and
biases are printed to the console so we can set the predetermined weights and biases for the game.
*/
bool Game::draw_start()
{
    Rectangle playButton = {global::WINDOW::width / 2 - 150, global::WINDOW::height / 2 - 50, 175, 50};
    Rectangle trainButton = {global::WINDOW::width / 2 + 50, global::WINDOW::height / 2 - 50, 175, 50};
    // Button colors
    Color playColor = {0, 255, 0, 255};
    Color trainColor = {0, 255, 0, 255};
    bool start = false;
    Vector2 mousePointer = GetMousePosition();
    // Play button mouse check
    if (CheckCollisionPointRec(mousePointer, playButton))
    {
        playColor = {0, 255, 0, 200};
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            start = true;
            global::GAME::TRAINING_MODE = false;
            global::GAME::delay = 120;
        }
    }
    // Training button mouse check
    if (CheckCollisionPointRec(mousePointer, trainButton))
    {
        trainColor = {0, 255, 0, 200};
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            start = true;
            global::GAME::TRAINING_MODE = true;
            global::GAME::delay = 15;
        }
    }
    // Button text
    const char *play_text = "Play Game";
    const char *train_text = "Training Mode";
    // Draw buttons
    BeginDrawing();
    ClearBackground(Color{3, 0, 60, 255});
    DrawRectangleRec(playButton, playColor);
    DrawRectangleRec(trainButton, trainColor);
    DrawText(play_text, playButton.x + 10, playButton.y + playButton.height / 4, 20, Color{3, 0, 60, 255});
    DrawText(train_text, trainButton.x + 10, trainButton.y + trainButton.height / 4, 20, Color{3, 0, 60, 255});
    // Draw two rectangles with text in them
    EndDrawing();
    return start;
}
/*
When all the snakes are dead draw the end game screen. Showing the top player score and AI score.
*/
void Game::end_game()
{
    if (are_snakes_dead())
    {
        this->draw_end_game();
    }
}
/*
This function displays the top player score and AI score. If the player score is higher it
displays victory and if it is lower it displays defeat.
*/
void Game::draw_end_game()
{
    BeginDrawing();
    ClearBackground(Color{3, 0, 60, 255}); // Get rid of game
    const char *victory_text = "Victory";
    const char *defeat_text = "Defeat";
    // get top scores
    short player_score = snakes[0]->get_score();
    short AI_score = get_max_score();
    // Format result text
    int vict_text_width = MeasureText(victory_text, 60);
    int def_text_width = MeasureText(defeat_text, 60);
    // Format and print player score and AI score
    char player_score_text[50];
    sprintf(player_score_text, "Player Score: %d", player_score);
    int ps_text_width = MeasureText(player_score_text, 40);
    char AI_score_text[50];
    sprintf(AI_score_text, "Top AI Score: %d", AI_score);
    int AIs_text_width = MeasureText(AI_score_text, 40);
    // Print the result
    if (player_score >= AI_score)
    {
        DrawText(victory_text, global::WINDOW::width / 2 - vict_text_width / 2, global::WINDOW::height / 4, 60, GREEN);
    }
    else
    {
        DrawText(defeat_text, global::WINDOW::width / 2 - vict_text_width / 2, global::WINDOW::height / 4, 60, RED);
    }
    DrawText(player_score_text, global::WINDOW::width / 2 - ps_text_width / 2, global::WINDOW::height * 2 / 4, 40, GREEN);
    DrawText(AI_score_text, global::WINDOW::width / 2 - AIs_text_width / 2, global::WINDOW::height * 3 / 4, 40, RED);
    EndDrawing();
}
// Finds the maximum score in the snakes vector
short Game::get_max_score()
{
    short max = 0;
    for (int i = 1; i < snakes.size(); i++)
    {
        max = std::max(max, this->snakes[i]->get_score());
    }
    return max;
}
// Goes through the snakes list and respawns each AI snake
void Game::spawn_AI()
{
    // Index starts at 1 because index 0 is reserved for the player snake
    for (int i = 1; i <= global::SNAKE::START_SNAKE_AI_COUNT; i++)
    {
        global::Position current_pos = map_functions::get_random_empty_position(*map);
        snakes[i]->reset_timer();
        snakes[i]->get_body().clear();
        snakes[i]->get_body().push_back(current_pos);
    }
}
/*
Iterates though each snake to check if it is dead. This function allows us to check
if the game is over (in play mode) or if we should spawn the next generation (train mode)
*/
bool Game::are_snakes_dead()
{
    for (int i = 0; i <= global::SNAKE::START_SNAKE_AI_COUNT; i++)
    {
        if (!(snakes[i]->is_dead()))
            return false;
    }
    return true;
}
/*
This function finds the snake with the highest score and returns it so we can
have its weights and baises. It also set the snakes score to zero so we can
find the next highest score.
*/
AI_Snake Game::get_max_wb()
{
    int max_score = 0;
    int max_index = 0;
    for (size_t i = 1; i < snakes.size(); i++)
    {
        if (dynamic_cast<AI_Snake *>(snakes[i])->get_score() > max_score)
        {
            max_index = i;
            max_score = dynamic_cast<AI_Snake *>(snakes[i])->get_score();
        }
    }
    AI_Snake max_snake = *(dynamic_cast<AI_Snake *>(snakes[max_index]));
    snakes[max_index]->set_score(0); // Make this no longer the max score to find the next parent
    return max_snake;
}