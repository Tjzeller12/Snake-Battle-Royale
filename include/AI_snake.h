#ifndef AI_SNAKE_H
#define AI_SNAKE_H

#include "snake.h"
#include <eigen3/Eigen/Dense>

class AI_Snake : public Snake
{
private:
    Eigen::MatrixXd weights1;
    Eigen::MatrixXd bias1;
    Eigen::MatrixXd weights2;
    Eigen::MatrixXd bias2;

public:
    AI_Snake(global::Map &i_map);
    AI_Snake(global::Map &i_map, global::SNAKE::Direction direction, global::Position initial_pos);
    virtual void update_direction() override;
    Eigen::MatrixXd get_inputs();
    void set_child_weights(Eigen::MatrixXd parent1, Eigen::MatrixXd parent2);
    void set_child_matrix(Eigen::MatrixXd parent1, Eigen::MatrixXd parent2, Eigen::MatrixXd &child);
    Eigen::MatrixXd get_weights1();
    Eigen::MatrixXd get_bias1();
    Eigen::MatrixXd get_weights2();
    Eigen::MatrixXd get_bias2();
    void set_weights1(Eigen::MatrixXd weights);
    void set_bias1(Eigen::MatrixXd bias);
    void set_weights2(Eigen::MatrixXd weights);
    void set_bias2(Eigen::MatrixXd bias);
    void set_child_weights_bias(AI_Snake parent1, AI_Snake parent2);
    void generate_random_matrix(double neuron_count, Eigen::MatrixXd &mat);
    void generate_initial_weights();
};

#endif