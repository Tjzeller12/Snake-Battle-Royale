#include "snake.h"
#include "raylib.h"
#include "Window.h"
#include <eigen3/Eigen/Dense>
#include "Map_Functions.h"
#include <random>
#include <chrono>
#include "Model.h"
Snake::Snake(global::Map& i_map) {
    map_ptr = &i_map;
    direction = global::SNAKE::Up;
    this->body.push_back(map_functions::get_random_empty_position(*map_ptr));
    food_timer = global::GAME::FOOD_TIMER_COUNT;
}
Snake::Snake(global::Map& i_map, global::SNAKE::Direction direction, global::Position initial_pos) {
    map_ptr = &i_map;
    this->body.push_back(initial_pos);
    this->direction = direction;
    food_timer = global::GAME::FOOD_TIMER_COUNT;
}
void Snake::set_direction(global::SNAKE::Direction direction) {
    this->direction = direction;
}

global::SNAKE::Direction Snake::get_direction() {
    return this->direction;
}
short Snake::get_score(){
    return this->score;
}
void Snake::set_score(short scr) {
    this->score = scr;
}
void Snake::draw(Color color) {
    // draw snake
    for (int i = 0; i < body.size(); i++) {
        DrawRectangle(body[i].x * global::MAP::CELL_SIZE, body[i].y * global::MAP::CELL_SIZE, global::MAP::CELL_SIZE, global::MAP::CELL_SIZE, color);
    }
}
void Snake::move(){
    // move snake
        //(*map_ptr)[body[body.size() - 1].x][body[body.size() - 1].y].total_snake_bodies --;
    for (int i = body.size() - 1; i > 0; i--) {
        body[i] = body[i - 1];
    }
    switch (direction) {
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
        //(*map_ptr)[body[0].x][body[0].y].total_snake_bodies ++; 
    
}
void Snake::update_map() {
    if(!this->is_dead()) {
        for (int i = 0; i < body.size(); i++) {
        (*map_ptr)[body[i].x][body[i].y].total_snake_bodies ++;
        }
    }
}
void Snake::grow() {
    // grow snake based on direction
    switch (direction) {
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
    //(*map_ptr)[body[body.size() - 1].x][body[body.size() - 1].y].total_snake_bodies ++;
    //body.push_back(body[body.size() - 1]);
}
bool Snake::is_dead() {
    // check if snake is dead
    if (this->body.size() <= 0) return true;
    if(!in_map(this->body[0])) return true;
    if ((*map_ptr)[this->body[0].x][this->body[0].y].is_wall || (*map_ptr)[this->body[0].x][this->body[0].y].total_snake_bodies > 1 || this->food_timer <= 0) {
        return true;
    }
    return false;
    
}
void Snake::reset_timer(){
    food_timer = global::GAME::FOOD_TIMER_COUNT;
}
void Snake::try_to_eat() {

    if ((*map_ptr)[this->body[0].x][this->body[0].y].is_food) {
        this->grow();
        (*map_ptr)[this->body[0].x][this->body[0].y].is_food = false;
        global::Position new_food_pos = map_functions::get_random_empty_position(*map_ptr);
        (*map_ptr)[new_food_pos.x][new_food_pos.y].is_food = true;
        this->score++;
        this->food_timer = global::GAME::FOOD_TIMER_COUNT;
    } else {
        this->food_timer--;
    }
}
void Snake::reset() {
    // reset snake
    body.clear();
}

std::vector<global::Position>& Snake::get_body() {
    return body;
}
void Snake::update_direction() {
    //move in random direction
    int random_direction = GetRandomValue(0, 3);
    switch (random_direction) {
        case 0:
            if (this->direction != global::SNAKE::Down) {
                this->direction = global::SNAKE::Up;
            }
            break;
        case 1:
            if (this->direction != global::SNAKE::Up) {
                this->direction = global::SNAKE::Down;
            }
            break;
        case 2:
            if (this->direction != global::SNAKE::Right) {
                this->direction = global::SNAKE::Left;
            }
            break;
        case 3:
            if (this->direction != global::SNAKE::Left) {
                this->direction = global::SNAKE::Right;
            }
            break;
    }
}
void Player_Snake::update_direction() {
    // based on user input, update direction
    if (IsKeyPressed(KEY_UP) && this->direction != global::SNAKE::Down) {
        this->direction = global::SNAKE::Up;
    }
    else if (IsKeyPressed(KEY_DOWN) && this->direction != global::SNAKE::Up) {
        this->direction = global::SNAKE::Down;
    }
    else if (IsKeyPressed(KEY_LEFT) && this->direction != global::SNAKE::Right) {
        this->direction = global::SNAKE::Left;
    }
    else if (IsKeyPressed(KEY_RIGHT) && this->direction != global::SNAKE::Left) {
        this->direction = global::SNAKE::Right;
    }
}
void AI_Snake::update_direction() {
        //move in random direction
    Eigen::MatrixXd inputs = this->get_inputs();
    Eigen::ArrayXd outputs = MODEL::Sequential(inputs, this->weights1,this->bias1, this-> weights2, this->bias2);
    Eigen::ArrayXd::Index max_index;
    double max_element = outputs.maxCoeff(&max_index);
    int new_direction = (3 + max_index + this->direction) % 4;

    switch(new_direction) {
        case 0: {
            if (global::SNAKE::Direction::Left != this->direction) {
                this->direction = global::SNAKE::Direction::Right;
            }
            break;
        }
        case 1: {
            if (global::SNAKE::Direction::Down != this->direction) {
                this->direction = global::SNAKE::Direction::Up;
            }
            break;
        }
        case 2: {
            if (global::SNAKE::Direction::Right != this->direction) {
                this->direction = global::SNAKE::Direction::Left;
            }
            break;
        }
        case 3: {
            if (global::SNAKE::Direction::Up != this->direction) {
                this->direction = global::SNAKE::Direction::Down;
            }
            break;
        }
    }
}

Eigen::MatrixXd AI_Snake::get_inputs() {

    std::array<char, 8> step_x = {1, 1, 0, -1, -1, -1, 0, 1};
    std::array<char, 8> step_y = {0, -1, -1, -1, 0, 1, 1, 1};
    Eigen::MatrixXd inputs(1, 8);

    for(unsigned char a = 2 * this->direction; ; a = (1 + a) % 8) {
        unsigned char input_index = (a + 2 * (4 - direction)) % 8;
        unsigned short distance = 0;
        global::Position curr_pos = body[0];

        while (1) {
            distance++;
            curr_pos.x += step_x[a];
            curr_pos.y += step_y[a];

            if (in_map(curr_pos)) {
                if (map_ptr->at(curr_pos.x).at(curr_pos.y).is_food) {
                    inputs(0, input_index) = static_cast<double>(std::max(global::MAP::COLUMNS, global::MAP::ROWS) - distance);
                    break;
                }
                if (0 < map_ptr->at(curr_pos.x).at(curr_pos.y).total_snake_bodies || map_ptr->at(curr_pos.x).at(curr_pos.y).is_wall) {
                    inputs(0, input_index) = static_cast<double>(distance - std::max(global::MAP::COLUMNS, global::MAP::ROWS));
                    break;
                }
            } else {
                inputs(0, input_index) = static_cast<double>(distance - std::max(global::MAP::COLUMNS, global::MAP::ROWS));
                break;
            }
        }
        if (a == (7 + 2 * this->direction) % 8) break;
    }
    return inputs;
}

bool Snake::in_map(global::Position pos) {
    return 0 <= pos.x && 0 <= pos.y && global::MAP::COLUMNS > pos.x && global::MAP::ROWS > pos.y;
}

void AI_Snake::set_child_weights_bias(AI_Snake parent1, AI_Snake parent2) {
    this->set_child_matrix(parent1.get_weights1(), parent2.get_weights1(), this->weights1);
    this->set_child_matrix(parent1.get_bias1(), parent2.get_bias1(), this->bias1);
    this->set_child_matrix(parent1.get_weights2(), parent2.get_weights2(), this->weights2);
    this->set_child_matrix(parent1.get_bias2(), parent2.get_bias2(), this->bias2);
}
//helper for setting weights and biases
void AI_Snake::set_child_matrix(Eigen::MatrixXd parent1, Eigen::MatrixXd parent2, Eigen::MatrixXd& child) {
    Eigen::MatrixXd mask = Eigen::MatrixXd::Random(parent1.rows(), parent1.cols()).unaryExpr([](double elem) {
        return elem > 0 ? 1.0 : 0.0;
    });
    std::cout << "Parent 1: Size " << parent1.size() << " Parent 2: Size " << parent2.size() << " mask size: " << mask.size() << "\n";
    child = mask.cwiseProduct(parent1) + (Eigen::MatrixXd::Ones(parent1.rows(), parent1.cols()) - mask).cwiseProduct(parent2);

    Eigen::MatrixXd mutation = Eigen::MatrixXd::Random(child.rows(), child.cols());
    for(int i = 0; i < child.rows(); ++i) {
        for(int j = 0; j < child.cols(); j++) {
            if ((std::rand() / static_cast<double>(RAND_MAX)) < global::TRAIN::MUTATION_RATE) {
                child(i, j) += mutation(i, j);
            }
        }
    }
}

void AI_Snake::generate_random_matrix(double neuron_count, Eigen::MatrixXd& mat) {
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator(seed);
    std::normal_distribution<double> distribution(0.0, std::sqrt(2.0 / neuron_count));

    for(int i = 0; i < mat.rows(); i++){
        for(int j = 0; j < mat.cols(); j++) {
            mat(i, j) = distribution(generator);
        }
    }
}

void AI_Snake::generate_initial_weights() {
    if (global::GAME::TRAINING_MODE) {
        generate_random_matrix(global::TRAIN::HL_COUNT, this->weights1);
        generate_random_matrix(3, this->weights2);
        std::cout << " Weights 1 " << this->weights1 << " Weights 2 " << this->weights2 << " Bias 1 " << this->bias1 << " bias2 " << this->bias2 << "\n";
    } else {
        this->weights1 << -1.23635, -0.000558379,     0.564241,     0.492897,    -0.437467,     0.842471,      1.52343,    -0.726721,    -0.219098,    -0.363192,     -0.15883,     0.950549,
                         -0.402162,     -0.27471,    -0.181202,    -0.179921,    0.0759702,    -0.927299,    -0.347762,     0.222699,      0.28318,  -0.00073691,    -0.869338,     0.207043,
                         -0.286209,   -0.0157219,   -0.0981461,  -0.00700724,   -0.0481736,     0.453225,   -0.0486449,     0.271944,      1.10376,     0.431583,    -0.301661,      2.01778,
                         0.0695969,     0.672533,    -0.356365,    -0.121696,     0.282046,    -0.765652,    -0.119407,     0.271685,    -0.187202,      0.23707,     0.524608,     0.114411,
                          0.791291,    -0.632402,     0.164563,    -0.154402,     0.116114,     0.114152,     0.270046,     0.139089,     0.634333,     -0.60044,    -0.546449,     0.595394,
                         0.0316093,     0.235313,    -0.689272,    -0.586176,  -0.00287243,     0.148312,    -0.555939,    -0.146282,    -0.923979,    -0.424753,    -0.220048,     0.455413,
                         -0.131422,   0.00984269,    -0.630248,    -0.408574,     -1.52406,    -0.361411,     -1.67424,      0.32651,    0.0504218,  -0.00660784,     0.464991,     -1.37307,
                          0.425992,     0.368936,     0.340764,     0.674047,   -0.0360605,   -0.0666092,     0.465248,     0.801711,   -0.0186431,     0.250737,    -0.219218,     0.340787;
        this->weights2 <<  0.954437,   -1.32615,   0.474636,
                           0.740443,   0.229653,   0.394445,
                           -2.08097,   0.097262,  -0.274876,
                           0.583363,    1.13416,   0.496286,
                          -0.174407,  0.0162866,   0.137943,
                          -0.218137,  -0.458077,  -0.210614,
                           -2.24174,    1.09213,   0.225718,
                          -0.185425, -0.0306822,  0.0300111,
                          -0.507604,    1.06066,   0.459518,
                          -0.401764,  -0.671811,    1.09876,
                           0.595937,   0.404279,  -0.868189,
                            -1.0965,  -0.944678,   0.429156;
        this->bias1 << -0.851615, 0.0, 0.0, 0.0, 0.0641998, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0;
        this->bias2 << 0.0, -0.224962, 0.0;
    }
}

Eigen::MatrixXd AI_Snake::get_weights1() {
    return this->weights1;
}
Eigen::MatrixXd AI_Snake::get_weights2() {
    return this->weights2;
}
Eigen::MatrixXd AI_Snake::get_bias1() {
    return this->bias1;
}
Eigen::MatrixXd AI_Snake::get_bias2() {
    return this->bias2;
}

Player_Snake::Player_Snake(global::Map& i_map) : Snake(i_map) {}
Player_Snake::Player_Snake(global::Map& i_map, global::SNAKE::Direction direction, global::Position initial_pos) :Snake(i_map, direction, initial_pos) {}

AI_Snake::AI_Snake(global::Map& i_map) :
    Snake(i_map),
    weights1(8, global::TRAIN::HL_COUNT),
    weights2(global::TRAIN::HL_COUNT, 3),
    bias1(Eigen::MatrixXd::Zero(1, global::TRAIN::HL_COUNT)),
    bias2(Eigen::MatrixXd::Zero(1, 3))
{ this->generate_initial_weights();}

AI_Snake::AI_Snake(global::Map& i_map, global::SNAKE::Direction direction, global::Position initial_pos) :
    Snake(i_map, direction, initial_pos),
    weights1(8, global::TRAIN::HL_COUNT),
    weights2(global::TRAIN::HL_COUNT, 3),
    bias1(Eigen::MatrixXd::Zero(1, global::TRAIN::HL_COUNT)),
    bias2(Eigen::MatrixXd::Zero(1, 3))
{this->generate_initial_weights();}