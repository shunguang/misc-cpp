// EvaluateEigDynamicMat.h
#pragma once
#include <Eigen/Dense>
#include <iostream>

class EvaluateEigDynamicMat {
public:
    // Constructor takes runtime dimensions
    EvaluateEigDynamicMat(int n) 
        : A(n, n), B(n, n), C(n, n) 
    {
        A.setRandom();
        B.setRandom();
        C.setZero();
    }

    void doSomething(int loopSize, bool noalias) {
        if( noalias ){
            for(int i=0; i<loopSize; i++){
                C.noalias() = A * B;
            }
        }else{
            for(int i=0; i<loopSize; i++){
                C = A * B;
            }
        }
    }

    // Print matrix
    void print() const {
        std::cout << "A:\n" << A << "\n";
        std::cout << "B:\n" << B << "\n";
        std::cout << "C:\n" << C << "\n";
    }
public:
    // Public 10x10 double-precision matrix
    Eigen::MatrixXd A;
    Eigen::MatrixXd B;
    Eigen::MatrixXd C;
};

