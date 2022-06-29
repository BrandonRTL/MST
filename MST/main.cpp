#include "boruvkamst.h"
#include <iomanip> 
#include <iostream>
#include <omp.h>
#include <atomic>
using namespace std;
int main() {
    double y, y_2;
    graph A("E:/gr/size900k.mtx"); 
    std::ofstream fout("data.txt");
    std::ofstream fout2("time.txt");
    fout2.clear();
    y_2 = boruvkasMST(A);
    fout << std::setprecision(20) << "seq1: " << y_2  << std::endl;
    std::cout << "PARALLEL\n";
    omp_set_num_threads(4);
    y = boruvkaMSTOpenMP(A);
    fout <<  y << " " << y_2 << std::endl;
	if(y != y_2) {
	    fout << "something went wrong: " <<   y << " " << y_2 << std::endl;
    }
    return 0;
}
