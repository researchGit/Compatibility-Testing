//MIT License
//
//Copyright (c) 2018 Lei Liu
//
//Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
//documentation files (the "Software"), to deal in the Software without restriction, including without limitation
//the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
//permit persons to whom the Software is furnished to do so, subject to the following conditions:
//
//The above copyright notice and this permission notice shall be included in all copies or substantial portions
//of the Software.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
//TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

//
// Detailed implementation of sparse table
//

#include "SparseTable.h"

using namespace std;

//compute the euler tour sequence for the input trees, sequence is stored in euler_tour_seq
//at the same time, initialize the depth array depth_lca, and record the index and copy of the first
//occurrence of each label in the euler tour sequence in node_array_lca and node_array_location
//depth is the level that a node lies on
void SparseTable::dfs(dis_tree root, int depth) {
    string _label = root.label;
    int _depth = depth;

    euler_tour_seq.push_back(_label);
    depth_lca.push_back(_depth);

    unordered_map<string, int>::iterator it1 = node_array_lca.find(_label);
    if(it1 == node_array_lca.end()){
        //not existing
        int index = (int)euler_tour_seq.size() - 1;
        node_array_lca.insert(unordered_map<string, int>::value_type(_label, index));
        node_array_location.insert(unordered_map<string, dis_tree>::value_type(_label, root));
    }

    for(unordered_set<dis_tree *>::iterator it2 = root.chd.begin(); it2 != root.chd.end(); it2++){
        dfs(*(*it2), ++depth);
        euler_tour_seq.push_back(_label);
        depth_lca.push_back(--depth);
    }
}

//compute the value of each entry of sparse table that is used by rmq algorithm
//n is the size of depth_array
void SparseTable::rmq_preprocess(int n) {
    //allocation
    rmq_array = (int **)malloc(sizeof(int*)*n);
    for(int i = 0; i < n; i++){
        *(rmq_array+i) = (int *)malloc(sizeof(int)*(int)(ceil(log10(n)/log10(2))));
    }

    //initialization the first element of each row as its row index
    for(int i = 0; i < n; i++){
        rmq_array[i][0] = i;
    }

    //complete the rest of the rmq array
    for(int j = 1; 1 << j <= n; j++){
        for(int i = 0; i + (1 << j) - 1 < n; i++){
            int a = rmq_array[i][j-1];
            int b = rmq_array[i + (1<<(j-1))][j-1];
            if(depth_lca[a] <= depth_lca[b]){
                rmq_array[i][j] = a;
            }
            else{
                rmq_array[i][j] = b;
            }
        }
    }
}

//query the sparse table, return the index of smaller value
//l and r are the index of two labels in the node array
int SparseTable::query(int l, int r) {
    int k = 0;
    while((1<<(k+1)) <= r - l + 1){
        k++;
    }
    int a = rmq_array[l][k];
    int b = rmq_array[r - (1<<k) + 1][k];
    if(depth_lca[a] <= depth_lca[b])
        return a;
    else
        return b;
}

//this is the main access to computing the LCA for two nodes with label u and v
//u and v are two labels
string SparseTable::compute_LCA(string u, string v) {
    int x = -1;
    int y = -1;
    unordered_map<string, int>::iterator iterator1 = node_array_lca.find(u);
    if(iterator1 != node_array_lca.end()){
        x = iterator1->second;
    }
    unordered_map<string, int>::iterator iterator2 = node_array_lca.find(v);
    if(iterator2 != node_array_lca.end()){
        y = iterator2->second;
    }
    if(x >= 0 && y >= 0){
        if(x > y){
            return euler_tour_seq[query(y,x)];
        }
        else{
            return euler_tour_seq[query(x,y)];
        }
    }
    else{
        cout << "can not find the corresponding node" << endl;
        return "NON-EXISTING-NODE";
    }
}

//destory the rmq array
void SparseTable::free_rmq_array(int n) {
    for(int i = 0; i < n;i++)
        free(*(rmq_array + i));
    free(rmq_array);
}

vector<string> SparseTable::get_euler_tour_seq() {
    return euler_tour_seq;
}

vector<int> SparseTable::get_depth_lca() {
    return depth_lca;
}

unordered_map<string, int> SparseTable::get_node_array_lca() {
    return node_array_lca;
}

int** SparseTable::get_rmq_array() {
    return rmq_array;
}

unordered_map<string, dis_tree> SparseTable::get_node_array_location() {
    return node_array_location;
}