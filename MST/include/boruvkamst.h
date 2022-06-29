#ifndef __MST_H__
#define __MST_H__
#include "Vectorgraph.h"
#include <time.h>
#include <climits>
#include <iostream>

int find(std::vector<subtree>& subsets, int i);
void treeUnion(std::vector<subtree>& subsets, int a, int b);
double boruvkasMST(graph& gr);
double boruvkaMSTOpenMP(graph& gr);
int findMinPerVertex(std::vector<int>& adj, std::vector<int>& nums, std::vector<int>& sec_ver, std::vector<float>& data, int id);
int removeDuplicates(std::vector<int>& adj, std::vector<int>& nums, std::vector<int>& sec_ver, std::vector<float>& data, std::vector<float>& minEdge, int id);
void printGraph(graph gr, std::string graphName);
void printVec(std::vector<int> new_adjs_temp, int size);
void printRemovedSmallestEdge(std::vector<int> smallestEdge, int size);

#endif