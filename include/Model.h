#ifndef MODEL_H
#define MODEL_H
#include <eigen3/Eigen/Dense>
namespace MODEL {

    inline Eigen::MatrixXd softmax(Eigen::MatrixXd input) {
        Eigen::Map<Eigen::ArrayXd> input_arr(input.data(), input.size());
        double maxCoeff = input_arr.maxCoeff();
        Eigen::ArrayXd ez = (input_arr - maxCoeff).exp();
        Eigen::ArrayXd softmax_output = ez / ez.sum();
        return softmax_output;
    }
    
    inline Eigen::MatrixXd ReLU(Eigen::MatrixXd z) {
        return z.cwiseMax(0.0);
    }

    inline Eigen::MatrixXd Layer(Eigen::MatrixXd A_in, Eigen::MatrixXd W, Eigen::MatrixXd b) {
        Eigen::MatrixXd Z = A_in * W + b;
        return ReLU(Z);
    }

    inline Eigen::ArrayXd Sequential(Eigen::MatrixXd x_inputs, Eigen::MatrixXd W1, Eigen::MatrixXd b1,  Eigen::MatrixXd W2, Eigen::MatrixXd b2) {
        Eigen::MatrixXd A1 = Layer(x_inputs, W1, b1);
        Eigen::MatrixXd output = Layer(A1, W2, b2);
        return softmax(output);
    }

}



#endif