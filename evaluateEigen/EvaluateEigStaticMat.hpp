// EvaluateEigStaticMat.h
#pragma once
#include <Eigen/Dense>
#include <iostream>

template<int N>
class EvaluateEigStaticMat {
public:
    // Constructor: initialize with zeros
    EvaluateEigStaticMat() {
        A.setRandom();
        B.setRandom();
        C.setZero();
    }


    void doSomething( int loopSize, bool noalias ) {
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

    // Example: print matrix
    void print() const {
        std::cout << "A:\n" << A << "\n";
        std::cout << "B:\n" << B << "\n";
        std::cout << "C:\n" << C << "\n";
    }

public:
    // Public 10x10 double-precision matrix
    Eigen::Matrix<double, N, N> A;
    Eigen::Matrix<double, N, N> B;
    Eigen::Matrix<double, N, N> C;
};



