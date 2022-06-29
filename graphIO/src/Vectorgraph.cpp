#include "Vectorgraph.h"
#include <iomanip>

using namespace std;
graph::graph(std::string filename)
{
    //filename = "/../" + filename;
    std::ifstream file(filename.c_str());
    int num_row, num_col, num_lines;
    int sum_adj = 0;
    bool is_symetric = 0;
    std::string title;
    std::getline(file, title);
    std::ofstream fout("data2.txt");
    if (title == "")
    {
        fout << "something went wrong";
    }
    if (title.find("symmetric") != std::string::npos)
    {
        is_symetric = 1;
    };
    //std::cout << title << std::endl;
    while (file.peek() == '%') file.ignore(2048, '\n');
    file >> num_row >> num_col >> num_lines;
    std::vector<std::vector<int> > temp_adj(num_row);
    std::vector<std::vector<float> > temp_data(num_row);
    nums.resize(num_row + 1);
    if (is_symetric)
    {
        sec_ver.resize(num_lines);
    }
    sec_ver.resize(2 * num_lines);
    for (int l = 0; l < num_lines; l++)
    {
        double data;
        int row, col;
        file >> row >> col >> data;
        temp_adj[row - 1].push_back(col - 1);
        temp_data[row - 1].push_back(data);
        if (is_symetric)
        {
            temp_adj[col - 1].push_back(row - 1);
            temp_data[col - 1].push_back(data);
        }
    }

    file.close();
    nums[0] = 0;
    int counter = 0;
    for (int i = 0; i < num_row; i++)
    {
        for (int j = 0; j < temp_adj[i].size(); j++)
        {
            adj.push_back(temp_adj[i][j]);
            data.push_back(temp_data[i][j]);
            sum_adj++;
            sec_ver[counter] = i;
            counter++;
        }
        nums[i + 1] = sum_adj;
    }
}
int find(std::vector<subtree>& t_tree, int i)
{
    int root = i;
    while (t_tree[root].parent != root)
    {
        root = t_tree[root].parent;
    }

    while (t_tree[i].parent != root)
    {
        int tmp = t_tree[i].parent;
        t_tree[i].parent = root;
        i = root;
    }

    return root;
}
