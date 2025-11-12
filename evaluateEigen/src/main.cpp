//build
//g++ -std=c++17 main.cpp -I ~/pkg/eigen/install/include/eigen3 -o test
//g++ -std=c++17 -O3 main.cpp -I ~/pkg/eigen/install/include/eigen3 -o test
//g++ -std=c++17 -O3 -march=native -ffast-math main.cpp -I ~/pkg/eigen/install/include/eigen3 -o test
//g++ -std=c++17 -O3 -march=native -funroll-loops main.cpp -I ~/pkg/eigen/install/include/eigen3 -o test

#include "EvaluateEigStaticMat.hpp"
#include "EvaluateEigDynamicMat.hpp"
#include <iostream>
#include <memory>
#include <vector>
#include <chrono>   // for timing

double test_static(int nObjects, int dim, int loopSize, bool noaliasFlag);
double test_dynamic(int nObjects, int dim, int loopSize, bool noaliasFlag); 
int main(int argc, char**argv) {
    // Scale up
    if( argc !=4 ){
        std::cout<< "test <nObjects> <loopSize> <noaliasFlag>" << std::endl; 
        std::cout<< "example:" << std::endl; 
        std::cout<< "test 1 10000 0" << std::endl; 
        std::cout<< "test 100 10000 1" << std::endl; 
        return -1;
    }
    int n = atoi(argv[1]);
    int loopSize =  atoi(argv[2]);   
    bool noaliasFlag = atoi(argv[3]);    
    std::cout << "n=" << n << std::endl;


    printf("Averaged Time:\tstatic(sec) \tdynamic(sec) \tR(dynamic/static)\n");
    std::vector<int> vDim={1,2,3,4,5,6,7,8,10,20};
    for(int dim : vDim){
        double dt_static  = test_static(n,dim, loopSize, noaliasFlag);
        double dt_dynamic = test_dynamic(n,dim, loopSize, noaliasFlag); 
        printf("dim=%d        \t%f\t %f \t %f\n", dim, dt_static, dt_dynamic, dt_dynamic/dt_static);
    }
    return 0;
}


#define TEST_STATIC_BLOCK(DIM) do { \
    std::vector<std::shared_ptr<EvaluateEigStaticMat<DIM>>> vStatic; \
    vStatic.reserve(nObjects); \
    for (int i = 0; i < nObjects; ++i) { \
        auto obj = std::make_shared<EvaluateEigStaticMat<DIM>>(); \
        vStatic.push_back(obj); \
    } \
    start_time = std::chrono::high_resolution_clock::now(); \
    for (auto &el : vStatic) { \
        el->doSomething(loopSize, noaliasFlag); \
    } \
    end_time = std::chrono::high_resolution_clock::now(); \
} while(0)

double test_static(int nObjects, int dim, int loopSize, bool noaliasFlag) 
{
    double dt;
    std::chrono::time_point<std::chrono::high_resolution_clock> start_time;
    std::chrono::time_point<std::chrono::high_resolution_clock> end_time;
    std::chrono::duration<double> elapsed;
    if(dim==1){
        TEST_STATIC_BLOCK(1);
    }
    else if(dim==2){
        TEST_STATIC_BLOCK(2);
    }
    else if(dim==3){
        TEST_STATIC_BLOCK(3);
    }
    else if(dim==4){
        TEST_STATIC_BLOCK(4);
    }
    else if(dim==5){
        TEST_STATIC_BLOCK(5);
    }
    else if(dim==6){
        TEST_STATIC_BLOCK(6);
    }
    else if(dim==7){
        TEST_STATIC_BLOCK(7);
    }
    else if(dim==8){
        TEST_STATIC_BLOCK(8);
    }
    else if(dim==10){
        TEST_STATIC_BLOCK(10);
    }
    else if(dim==20){
        TEST_STATIC_BLOCK(20);
    }
    else{
        printf("error: not impl dim=%d\n", dim);
    }
    elapsed = end_time - start_time;   
    dt = elapsed.count();


    //std::cout << "total Time used for vStatic loop: " << dt << " seconds\n";

#if 0
    if (!vStatic.empty()) {
        std::cout << "\nFirst vStatic object's matrix:\n";
        vStatic[0]->print();
    }
#endif
    return dt;
}


double test_dynamic(int nObjects, int dim, int loopSize, bool noaliasFlag) 
{
  
    // Create n shared_ptr: vDynamic
    std::vector<std::shared_ptr<EvaluateEigDynamicMat>> vDynamic;
    vDynamic.reserve(nObjects);
    for (int i = 0; i < nObjects; ++i) {
        auto obj = std::make_shared<EvaluateEigDynamicMat>(dim);
        vDynamic.push_back(obj);
    }

    // Measure time for vDynamic loop
    auto start_dynamic = std::chrono::high_resolution_clock::now();
    for (auto &el : vDynamic) {
        el->doSomething(loopSize,noaliasFlag);
    }
    auto end_dynamic = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> elapsed_dynamic = end_dynamic - start_dynamic;
    double dt = elapsed_dynamic.count();

    //std::cout << "total Time used for vDynamic loop: " << dt << " seconds\n";

    // Example: print the first object
#if 0    
    if (!vDynamic.empty()) {
        std::cout << "\nFirst vDynamic object's matrix:\n";
        vDynamic[0]->print();
    }
#endif
    return dt;
}

