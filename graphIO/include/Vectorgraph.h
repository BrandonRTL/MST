#ifndef __TGRAPH_H__
#define __TGRAPH_H__
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <omp.h>
#include <fstream>
class graph
{
public:
    std::vector<int> adj;
    std::vector<int> nums;
    std::vector<int> sec_ver;
    std::vector<float> data;
    graph(std::string filename);
};
class subtree
{
public:
    int parent;
    int rank;
    subtree(int pr = 0, int r = 0)
    {
        parent = pr;
        rank = r;
    }
};

#endif