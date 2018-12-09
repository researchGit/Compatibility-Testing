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
// This class implements sparse table structure that is used for RMQ algorithm
// and defines a linked-list data structure that includes clusters defined in ... book
//

#ifndef FGO_SPARSETABLE_H
#define FGO_SPARSETABLE_H

#include <iostream>
#include <unordered_set>
#include <unordered_map>
#include <set>
#include <vector>
#include "TreeStructure.h"

using namespace std;

class SparseTable {

public:
    void dfs(dis_tree root, int depth);
    void rmq_preprocess(int n);
    int query(int l, int r);
    string compute_LCA(string u, string v);
    vector<string> get_euler_tour_seq();
    vector<int> get_depth_lca();
    unordered_map<string, int> get_node_array_lca();
    unordered_map<string, dis_tree> get_node_array_location();
    int** get_rmq_array();
    void free_rmq_array(int n);


private:
    vector<string> euler_tour_seq;
    vector<int> depth_lca;
    unordered_map<string, int> node_array_lca;
    unordered_map<string, dis_tree> node_array_location;
    int** rmq_array;

};


#endif //FGO_SPARSETABLE_H
