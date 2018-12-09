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
// Linked_list_builder.h is the class for constructing a big linked-list structure by merging all the input trees
//

#ifndef FGO_LIST_BUILDER_H
#define FGO_LIST_BUILDER_H

#include <iostream>
#include <unordered_set>
#include <unordered_map>
#include <map>

using namespace std;

//define status for DFS algorithm
#define NONCOLORED 0
#define WHITE 1
#define GRAY 2
#define BLACK 3

struct linked_list_entry{
    string value;
    int color;
    unordered_set<int> belongs;
    unordered_map<string, linked_list_entry> ajcn_list;
    string to;
    string from;
    int reinsert;
    int first_occur_number;
    int count;
    int kl;
    double weight;
    string input_weight;
    linked_list_entry(){};
    linked_list_entry(string str, int a, int b):value(str),color(a){belongs.insert(b);
        to = ""; from = ""; reinsert = 0; count = 0; kl = 1; weight = 0; input_weight = "";};
};


class Linked_List_Builder {

public:
    void merge_input_trees(multimap<string, unordered_map<string, linked_list_entry> > input_trees, unordered_map<string, linked_list_entry> global_hpu);
    int Get_Nodes_Number();
    unordered_map<int, unordered_set<string> > get_valid_position(){return initial_valid_pos;};
    unordered_map<string, int> get_semi_universal_nodes(){return initial_semi_universal_nodes;};
    unordered_map<string, linked_list_entry> get_graph_hpu(){return graph_hpu;};
    int get_sum_kl(){return sum_kl;};

private:
    unordered_map<string, unordered_map<string, linked_list_entry> > tree_collection;
    unordered_map<string, linked_list_entry> graph_hpu;
    unordered_map<int, unordered_set<string> > initial_valid_pos;
    unordered_map<string, int> initial_semi_universal_nodes;
    //number of the nodes in the tree
    int NODES_NUMBER;
    int sum_kl = 0;
    unordered_map<int, unordered_map<string, linked_list_entry> > trees;

private:
    void set_valid_position(unordered_map<int, unordered_set<string> > valpos){initial_valid_pos = valpos;};
    void set_graph_hpu(unordered_map<string, linked_list_entry> graph){graph_hpu = graph;};
    void set_nodes_number(int number){NODES_NUMBER = number;};
    void set_semi_universal_nodes(unordered_map<string, int> semi){initial_semi_universal_nodes = semi;};

};


#endif //FGO_LIST_BUILDER_H
