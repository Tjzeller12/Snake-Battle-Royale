#ifndef SNAKE_H
#define SNAKE_H
#include "raylib.h"
#include "Window.h"
#include "Global.h"
#include <iostream>
#include <eigen3/Eigen/Dense>
// Snake class with coordinates and direction
class Snake {
    protected:
        std::vector<global::Position> body;
        global::SNAKE::Direction direction;
        global::Map* map_ptr;
        short score;
        short food_timer;
    public:
        Snake(global::Map& i_map);
        Snake(global::Map& i_map, global::SNAKE::Direction direction, global::Position initial_pos);
        void set_position();
        void get_positon();
        void set_direction(global::SNAKE::Direction direction);
        void move();
        void reset();
        virtual void update_direction();
        void update_status();
        void update_map();
        void grow();
        std::vector<global::Position>& get_body();
        short get_score();
        void set_score(short scr);
        bool is_dead();
        void try_to_eat();
        global::SNAKE::Direction get_direction();
        void draw(Color color);
        bool in_map(global::Position pos);
        void reset_timer();
        
};
class Player_Snake : public Snake{
    public:
        Player_Snake(global::Map& i_map);
        Player_Snake(global::Map& i_map, global::SNAKE::Direction direction, global::Position initial_pos);
        virtual void update_direction() override;
};
class AI_Snake : public Snake{
    private:
        Eigen::MatrixXd weights1;
        Eigen::MatrixXd bias1;
        Eigen::MatrixXd weights2;
        Eigen::MatrixXd bias2;
    public:
        AI_Snake(global::Map& i_map);
        AI_Snake(global::Map& i_map, global::SNAKE::Direction direction, global::Position initial_pos);
        virtual void update_direction() override;
        Eigen::MatrixXd get_inputs();
        void set_child_weights(Eigen::MatrixXd parent1, Eigen::MatrixXd parent2);
        void set_child_matrix(Eigen::MatrixXd parent1, Eigen::MatrixXd parent2, Eigen::MatrixXd& child);
        Eigen::MatrixXd get_weights1();
        Eigen::MatrixXd get_bias1();
        Eigen::MatrixXd get_weights2();
        Eigen::MatrixXd get_bias2();
        void set_weights1(Eigen::MatrixXd weights);
        void set_bias1(Eigen::MatrixXd bias);
        void set_weights2(Eigen::MatrixXd weights);
        void set_bias2(Eigen::MatrixXd bias);
        void set_child_weights_bias(AI_Snake parent1, AI_Snake parent2);
        void generate_random_matrix(double neuron_count, Eigen::MatrixXd& mat);
        void generate_initial_weights();

};
#endif