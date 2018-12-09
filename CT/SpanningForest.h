
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
// SpanningForest.h is designed for constructing euler tour sequence and node array by using DFS algorithm
//

#ifndef FGO_SPANNINGFOREST_H
#define FGO_SPANNINGFOREST_H

#include "Linked_List_Builder.h"
#include "HDT_Structure.h"
#include <iostream>
#include <set>
#include <map>
#include <vector>
#include <unordered_map>
#include <stack>

using namespace std;

class SpanningForest {
public:
    void generate_spanning_forest(unordered_map<string, linked_list_entry> _hpu);
    void generate_spanning_forest_withNoColor(unordered_map<string, linked_list_entry> _hpu);
    vector<occ_Node> get_euler_tour_sequence(){return et_tour_sequence;};
    unordered_map<string, array_Node> get_Node_Array(){return node_array;};
    unordered_map<int, unordered_set<graph_Edge, hash_Edge> > get_tree_edge(){return tree_edge;};
    unordered_map<int, unordered_set<graph_Edge, hash_Edge> > get_non_tree_edge(){return non_tree_edge;};
    unordered_map<graph_Edge, int, hash_Edge> get_edges(){return all_edges;};
    unordered_map<int, unordered_map<string, unordered_set<graph_Edge, hash_Edge> > > get_query_table(){return non_tree_edge_query_table;};

private:
    void non_recursive_DFS_visit(unordered_map<string, linked_list_entry> *graph_entry, string node_value, string first_value);

private:
    int OCC_NUMBER_COUNTER = 0;
    vector<occ_Node> et_tour_sequence; //euler tour sequence
    unordered_map<string, array_Node> node_array; // node list
    unordered_map<int, unordered_set<graph_Edge, hash_Edge> > tree_edge;
    unordered_map<int, unordered_set<graph_Edge, hash_Edge> > non_tree_edge;
    unordered_map<int, unordered_map<string, unordered_set<graph_Edge, hash_Edge> > > non_tree_edge_query_table;
    unordered_map<graph_Edge, int, hash_Edge> all_edges;

};


#endif //FGO_SPANNINGFOREST_H
