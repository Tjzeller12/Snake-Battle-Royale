#include "AI_snake.h"
//Overloaded constructor
AI_Snake::AI_Snake(global::Map& i_map) :
//Set weights and baises and map pointer
    Snake(i_map),
    weights1(8, global::TRAIN::HL_COUNT), //Make 8X(hidden layer count) matrix
    weights2(global::TRAIN::HL_COUNT, 3), //Make (hidden layer count)X3 matrix
    bias1(Eigen::MatrixXd::Zero(1, global::TRAIN::HL_COUNT)), //Make a 1X(hidden layer count) matrix
    bias2(Eigen::MatrixXd::Zero(1, 3)) //Make a 1X3 matrix
{ this->generate_initial_weights();}
//Overloaded constructor
AI_Snake::AI_Snake(global::Map& i_map, global::SNAKE::Direction direction, global::Position initial_pos) :
//Set weights and baises and map pointer
    Snake(i_map, direction, initial_pos),
    weights1(8, global::TRAIN::HL_COUNT),
    weights2(global::TRAIN::HL_COUNT, 3),
    bias1(Eigen::MatrixXd::Zero(1, global::TRAIN::HL_COUNT)),
    bias2(Eigen::MatrixXd::Zero(1, 3))
{this->generate_initial_weights();}
/*
Update direction based off the max element from the output of the neural net. The AI has 3 options:
1. Turn left
2. Go straight
3. Turn right
This algorith determines the the new direction based on the current direction: new direction = (3 + + max index + current direction) mod 4
*/
void AI_Snake::update_direction() {
    //Get inputs
    Eigen::MatrixXd inputs = this->get_inputs();
    //Get output by passing in inputs,weights, and biases into sequential model
    Eigen::ArrayXd outputs = MODEL::Sequential(inputs, this->weights1,this->bias1, this-> weights2, this->bias2);
    Eigen::ArrayXd::Index max_index;
    //Get max output
    double max_element = outputs.maxCoeff(&max_index);
    //Get the direction based off the max element index
    int new_direction = (3 + max_index + this->direction) % 4;
    //Update direction
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
/*
Gets distance from food/snakes/walls in all 8 directions of snake. Then gets the difference
between these distances and the maximum distance. The input will be possitive fro food and negative
for snakes, walls, or anything else.
*/
Eigen::MatrixXd AI_Snake::get_inputs() {
    //Direction variables
    std::array<char, 8> step_x = {1, 1, 0, -1, -1, -1, 0, 1};
    std::array<char, 8> step_y = {0, -1, -1, -1, 0, 1, 1, 1};
    //Initialize inputs matrix
    Eigen::MatrixXd inputs(1, 8);
    //Iterate through each direction arround snakes head. The loop goes through each direction until 
    //it completes a cirlce
    for(unsigned char a = 2 * this->direction; ; a = (1 + a) % 8) {
        //Calculate the input index relative to the current direction of the snake
        unsigned char input_index = (a + 2 * (4 - direction)) % 8;
        //Distance for how far the snake can move in the current direction
        unsigned short distance = 0;
        //Position of snake head
        global::Position curr_pos = body[0];

        //Move in current direction until food, wall, or snake is found
        while (1) {
            //increment distance
            distance++;
            //update psotion based off direction
            curr_pos.x += step_x[a];
            curr_pos.y += step_y[a];
            //check if the current position is in the map
            if (in_map(curr_pos)) {
                if (map_ptr->at(curr_pos.x).at(curr_pos.y).is_food) { //input possitive for food
                    //The input value will be the max possible distance minus the current distance
                    inputs(0, input_index) = static_cast<double>(std::max(global::MAP::COLUMNS, global::MAP::ROWS) - distance);
                    break;
                }
                if (0 < map_ptr->at(curr_pos.x).at(curr_pos.y).total_snake_bodies || map_ptr->at(curr_pos.x).at(curr_pos.y).is_wall) { //input negative for snake or wall
                    //The input value will current distance - max distance
                    inputs(0, input_index) = static_cast<double>(distance - std::max(global::MAP::COLUMNS, global::MAP::ROWS));
                    break;
                }
            } else {
                //The input value will current distance - max distance
                inputs(0, input_index) = static_cast<double>(distance - std::max(global::MAP::COLUMNS, global::MAP::ROWS));
                break;
            }
        }
        //Break the loop if we completed the full circle around the snake's head
        if (a == (7 + 2 * this->direction) % 8) break;
    }
    return inputs;
}
//Uses the set child_matrix method to set the weights and baises based off the parents weights and baises. The parents will be the top 2 scorers.
void AI_Snake::set_child_weights_bias(AI_Snake parent1, AI_Snake parent2) {
    this->set_child_matrix(parent1.get_weights1(), parent2.get_weights1(), this->weights1);
    this->set_child_matrix(parent1.get_bias1(), parent2.get_bias1(), this->bias1);
    this->set_child_matrix(parent1.get_weights2(), parent2.get_weights2(), this->weights2);
    this->set_child_matrix(parent1.get_bias2(), parent2.get_bias2(), this->bias2);
}
/*
helper for setting weights and biases. Takes two parent parameters and one child. The parents are the 2 highest scores of the previous genration.
We will create a matrix that takes random weights from both of the parents (some of the weights will be from parent 1 and the others will be from parent 2).
We will then add some mutation so there is a little bit of variation from the parents genes. The mutation will less then the mutation rate defined in global and
will be added on to the weight.
*/

void AI_Snake::set_child_matrix(Eigen::MatrixXd parent1, Eigen::MatrixXd parent2, Eigen::MatrixXd& child) {
    //Mask matrix is a matrix of 1s and 0s where 1s represent parent 1 and zero represents parent 2.
    Eigen::MatrixXd mask = Eigen::MatrixXd::Random(parent1.rows(), parent1.cols()).unaryExpr([](double elem) {
        return elem > 0 ? 1.0 : 0.0;
    });
    //Print the weights and baises in case we want to update our predefined weights and baises
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
/*
Generate a random number for each value in the matrix and normalize it with the normal distribution.
*/
void AI_Snake::generate_random_matrix(double neuron_count, Eigen::MatrixXd& mat) {
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator(seed);
    //Create a normal distribution with a seed of zero and a mean of sqrt(2/neuron count)
    std::normal_distribution<double> distribution(0.0, std::sqrt(2.0 / neuron_count));
    //This normal distrubiton function allows the weights to be small and centered around zero.
    //This prevents vanishing and exploding gradients
    for(int i = 0; i < mat.rows(); i++){
        for(int j = 0; j < mat.cols(); j++) {
            mat(i, j) = distribution(generator);
        }
    }
}
/*
If we are in training mode call generate randome matrix twice to get weights 1 and 2. This is because we initialize the snakes with random weights and zero vectors for the baises.
Then print the weights and baises. If we are in game mode then set the weights and baises to a value that is predetermined.
*/
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
//Weights 1 getter
Eigen::MatrixXd AI_Snake::get_weights1() {
    return this->weights1;
}
//Weight 2 getter
Eigen::MatrixXd AI_Snake::get_weights2() {
    return this->weights2;
}
//Bais 1 getter
Eigen::MatrixXd AI_Snake::get_bias1() {
    return this->bias1;
}
//Bais 2 getter
Eigen::MatrixXd AI_Snake::get_bias2() {
    return this->bias2;
}