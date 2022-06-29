#include "boruvkamst.h"
#include <atomic>
//#define PRINT_CONTROL
using namespace std;
int findMinPerVertex(std::vector<int>& adj, std::vector<int>& nums, std::vector<int>& sec_ver, std::vector<float>& data, int id)
{
    int minEdge = -1;
    int minWeight = INT_MAX;
    int minDst = nums.size();
    for (int j = nums[id]; j < nums[id + 1]; j++)
    {
        int weight = data[j];
        int dest = adj[j];
        if (weight < minWeight || (weight == minWeight && dest < minDst))
        {
            minEdge = j;
            minWeight = weight;
            minDst = dest;
        }
    }
   
    return minEdge;

}
void countNewEdges(std::vector<int>& newNumsSumm, std::vector<int>& excPreSum, std::vector<int>& adj, std::vector<int>& nums, std::vector<int>& colors, int i)
{
    int newEdges = 0;
    int color2 = colors[i];
    for (int j = nums[i]; j < nums[i + 1]; j++)
    {
        int color1 = colors[adj[j]];
        if (color1 != color2)
        {
            newEdges++;
        }
    }
#pragma omp atomic 
    newNumsSumm[excPreSum[color2]] += newEdges;
    //return 1;
}

void removeDuplicates(std::vector<int>& adj, std::vector<int>& nums, std::vector<int>& sec_ver, std::vector<float>& data, std::vector<int>& vertMinEdge, int id)
{
    int edge = vertMinEdge[id];
    if (edge == -1)
        return;
    int dst = adj[edge];
    int otherEdge = vertMinEdge[dst];
    if (otherEdge == -1) 
        return;
    int otherDst = adj[otherEdge];

    if (id == otherDst && id < dst) 
    {
        vertMinEdge[id] = -1;
    }

}
void treeUnion(std::vector<subtree>& t_tree, int a, int b)
{
    int rootA = find(t_tree, a);
    int rootB = find(t_tree, b);
    if (t_tree[rootA].rank < t_tree[rootB].rank)
        t_tree[rootA].parent = rootB;
    else if (t_tree[rootA].rank > t_tree[rootB].rank)
        t_tree[rootB].parent = rootA;
    else
    {
        t_tree[rootB].parent = rootA;
        t_tree[rootA].rank = t_tree[rootA].rank + 1;
    }
}
double boruvkasMST(graph& gr)
{
    subtree t_tree;
    int V = gr.nums.size() - 1;
    int E = gr.data.size();
    std::vector<subtree> trees(V);
    int treeCounter = V;
    int treeControl = 0;
    double treeWeight = 0;
    std::cout << "MST1" << std::endl;

    for (int i = 0; i < V; i++)
    {
        t_tree = subtree(i, 0);
        trees[i] = t_tree;
    }
    double start = omp_get_wtime();
    while (treeCounter > 1)
    {
        std::vector<int> smallestEdge(V, -1);
        int treeControl = treeCounter;
        for (int i = 0; i < V; i++)
        {
            int setB = find(trees, i);
            for (int j = gr.nums[i]; j < gr.nums[i + 1]; j++)
            {
                int setA = find(trees, gr.adj[j]);
                if (setA != setB)
                {
                    if (smallestEdge[setA] == -1 || gr.data[smallestEdge[setA]] > gr.data[j])
                        smallestEdge[setA] = j;
                    if (smallestEdge[setB] == -1 || gr.data[smallestEdge[setB]] > gr.data[j])
                        smallestEdge[setB] = j;
                }
            }
        }
        for (int i = 0; i<V; i++)
        {
            if (smallestEdge[i] != -1)
            {
                int setA = find(trees, gr.adj[smallestEdge[i]]);
                int setB = find(trees, gr.sec_ver[smallestEdge[i]]);

                if (setA != setB)
                {
                    treeWeight += gr.data[smallestEdge[i]];
                    treeUnion(trees, setA, setB);
                    treeControl--;
                }
            }
        }
        if (treeControl == treeCounter)
            break;
        treeCounter = treeControl;
    }
    double end = omp_get_wtime();
    cout << (end - start) << std::endl;
    return treeWeight;
   
}
double boruvkaMSTOpenMP(graph& gr)
{
    std::ofstream fout("time.txt", std::ios::app);
    int iter = 0;
    bool changed;
    int E = gr.data.size();
    int size = gr.nums.size() - 1;;
    int treeCounter = size;
    int treeControl = 0;
    double treeWeight = 0;
    vector<int> newAdj;
    vector<int> newSecVer;
    vector<float> newData;
    vector<int> newNums;
    newAdj = gr.adj;
    newSecVer = gr.sec_ver;
    newData = gr.data;
    newNums = gr.nums;
    std::vector<int> smallestEdge(size, -1);
    std::vector<int> colors(size, -1);
    int i;
    vector<double> times(7, 0);
    double end = 0;
    double start = omp_get_wtime();
#ifdef PRINT_CONTROL
    printGraph(gr, "starting graph");
    cout << endl;
#endif
    while (treeCounter > 1)
    {
        int treeControl = 0;
       // a) Find minimum edge per vertex new:
#ifdef PRINT_CONTROL
        cout << "Find minimum edge per vertex: " << endl;
#endif
        double time = omp_get_wtime();
#pragma omp parallel private(i)
        {
#pragma omp for schedule(guided)
            for (i = 0; i < size; i++)
            {
                smallestEdge[i] = findMinPerVertex(newAdj, newNums, newSecVer, newData, i);
            }
        }
        end = omp_get_wtime();
        times[0] += end - time;
        //fout << "finding min vert: " << omp_get_wtime() - time << std::endl;
#ifdef PRINT_CONTROL
        for (int i = 0; i < size; i++)
        {
            cout << i << "'s smallest edge: " << smallestEdge[i] << " (" << newAdj[smallestEdge[i]] << "," << newSecVer[smallestEdge[i]] << ") weight:" << newData[smallestEdge[i]] << endl;
        }
#endif
        // b) Remove mirrored edges new
        time = omp_get_wtime();
#pragma omp parallel for schedule(guided)
        for (i = 0; i < size; i++)
        {
            removeDuplicates(newAdj, newNums, newSecVer, newData, smallestEdge, i);
        }
        end = omp_get_wtime();
        times[1] += end - time;
        //fout << "removeDuplicates: " << omp_get_wtime() - time << std::endl;
#ifdef PRINT_CONTROL
        printRemovedSmallestEdge(smallestEdge, size);
#endif
        // c) Initialize and propagate colors new:
        time = omp_get_wtime();
#pragma omp parallel for schedule(guided)
        for (i = 0; i < size; i++)
        {
            int edge = smallestEdge[i];
            if (edge == -1)
            {
                colors[i] = i;
            }
            else colors[i] = newAdj[edge];
        }     
        do {
            changed = false;

#pragma omp parallel private(i)
            {
                bool myChanged = false;

#pragma omp for
                for (i = 0; i < size; i++)
                {
                    int my_color = colors[i];
                    int other_color = colors[my_color];

                    if (my_color != other_color)
                    {
                        colors[i] = other_color;
                        myChanged = true;
                    }
                }

                if (myChanged) changed = true;
            }

        } while (changed);
        end = omp_get_wtime();
        times[2] += end - time;
        //fout << "Initialize and propagate colors: " << omp_get_wtime() - time << std::endl;

        time = omp_get_wtime();
#pragma omp parallel for reduction (+:treeWeight)
        for (i = 0; i < size; i++)
        {
            int edge = smallestEdge[i];
            if (edge != -1)
                treeWeight += newData[edge];
        }
        end = omp_get_wtime();
        times[3] += end - time;
        //fout << "Add tree weight: " << omp_get_wtime() - time << std::endl;

        // d) Create new vertex ids
        time = omp_get_wtime();
        std::vector<int> flags(size, 0);
        std::vector<int> excPreSum(size);
        int sum = 0;
        for (int i = 0; i < size; i++)
        {
            excPreSum[i] = sum;
            if(colors[i] == i)
            {
                flags[i] = 1;
                sum += 1;
            }
        }
        end = omp_get_wtime();
        times[4] += end - time;
        //fout << "Create new vertex ids: " << omp_get_wtime() - time << std::endl;
        treeControl = sum;

        if (treeControl == treeCounter)
        {
            break;
        }
#ifdef PRINT_CONTROL
        cout << "Flag:  ";
        printVec(flags, size - 1);
        cout << "Summ:  ";
        printVec(excPreSum, size - 1);
        cout << endl;
#endif    
        // e) Count, assign, and insert new edges
        // e.1) Count 
        time = omp_get_wtime();
        int nvm = sum;
        vector<int> newNumsSumm(nvm);
        vector<int> new_adjs(nvm + 1, 0);
#pragma omp parallel for schedule(guided)
        for (int i = 0; i < size; i++)
        {
            countNewEdges(newNumsSumm, excPreSum, newAdj, newNums, colors, i);
        }
        
        end = omp_get_wtime();
        times[5] += end - time;
        //fout << "Count newNumsSumm: " << omp_get_wtime() - time << std::endl;

        time = omp_get_wtime();        
        int pref_sum = 0;
        for (int i = 0; i < nvm; i++) {
            pref_sum += newNumsSumm[i];
            new_adjs[i + 1] = pref_sum;
        }
        //vector<int> new_adjs_temp = new_adjs;
        //out << "Count new adjs: " << omp_get_wtime() - time << std::endl;

        vector<atomic_int> new_adjs_temp(nvm + 1);
        for (int i = 0; i < new_adjs_temp.size(); i++)
        {
            new_adjs_temp[i] = new_adjs[i];
        }
        int new_edge_size = new_adjs[nvm];
        
        vector<int> newFirstEdge(new_edge_size);
        vector<int> new_sec_edge(new_edge_size);
        vector<float> new_data_1(new_edge_size);
        time = omp_get_wtime();
#pragma omp parallel for schedule(guided) 
        for (int i = 0; i < size; i++)
        {
            int color2 = colors[i];
            int sVertID = excPreSum[color2];
            for (int j = newNums[i]; j < newNums[i + 1]; j++)
            {
                int color1 = colors[newAdj[j]];
                if (color1 != color2)
                {
                    int id = 0;
                    id = atomic_fetch_add(&new_adjs_temp[sVertID], 1);
                    newFirstEdge[id] = excPreSum[color1];
                    new_sec_edge[id] = excPreSum[color2];
                    new_data_1[id] = newData[j];                   
                }
            }
        }

        end = omp_get_wtime();
        times[6] += end - time;
        //fout << "Insert new edges: " << omp_get_wtime() - time << std::endl;

        // new graph
       time = omp_get_wtime();
       newAdj = newFirstEdge;
       newSecVer = new_sec_edge;
       newData = new_data_1;
       newNums = new_adjs;
#ifdef PRINT_CONTROL
        printGraph(gr, "new graph");
        cout << endl;
#endif
        treeCounter = treeControl;
        size = newNums.size() - 1;
        //fout << "New graph: " << omp_get_wtime() - time << std::endl;
#ifdef PRINT_CONTROL
        cout << "ITERATION END" << endl;
        std::cout << "Total weight at the end of the iteration: " << treeWeight << std::endl;
#endif
        //cout <<  iter << " ITERATION END" << endl;
        //iter++;

    }
#ifdef PRINT_CONTROL
    cout << (clock() - start) / CLOCKS_PER_SEC << endl;
#endif
    end = omp_get_wtime();
    cout << (end - start) << std::endl;
    fout << "Min edge: " << times[0] << endl;
    fout << "Mirrored: " << times[1] << endl;
    fout << "Colors: " << times[2] << endl;
    fout << "Tree weight: " << times[3] << endl;
    fout << "New vert ids: " << times[4] << endl;
    fout << "Count new edges: " << times[5] << endl;
    fout << "Insert new edges: " << times[6] << endl;
    fout << std::endl;
    fout.close();
    return treeWeight;
}

void printGraph(graph gr, string st)
{
    cout << st << endl;
    for (int i = 0; i < gr.data.size(); i++)
    {
        cout << "(" << gr.adj[i] << "," << gr.sec_ver[i] << ") weight: " << gr.data[i] << endl;
    }
    for (int i = 0; i < gr.nums.size(); i++)
    {
        cout << gr.nums[i] << " ";
    }
}
void printVec(vector<int> new_adjs_temp, int size)
{
    for (int i = 0; i < size + 1; i++)
    {
        cout << new_adjs_temp[i] << " ";
    }
}
void printRemovedSmallestEdge(vector<int> smallestEdge, int size)
{
    cout << "Remove mirrored edges: " << endl;
    for (int i = 0; i < size; i++)
    {
        cout << i << "'s smallest edge: " << smallestEdge[i] << endl;
    }
}