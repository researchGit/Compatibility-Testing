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


#ifndef FGO_FASTCOMPATIBILITYTEST_H
#define FGO_FASTCOMPATIBILITYTEST_H

#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include "Linked_List_Builder.h"
#include <queue>
#include <vector>
#include "HDT_Structure.h"
#include "HDT.h"
#include <list>


using namespace std;

#define INCOMPATIBLE 0;
#define FINISHED 1;

struct dis_Node{
    string label;
    vector<dis_Node *> children;
    dis_Node* parent;
    unordered_map<int, unordered_set<string> > tree_label;
    unordered_set<string> incomp_node;
    dis_Node(){};
    dis_Node(string str):label(str){parent = NULL;};
};

struct Y{
    unordered_map<string, linked_list_entry> component;
    dis_Node* treep;//points to the display graph
    unordered_map<int, unordered_set<string> >  map;
    unordered_map<string, int> semi;
    unordered_map<string, int> semi_potential;
    unordered_set<string> candidate;
    int sum_kl;
    Y(){};
};

class FastCompatibilityTest {
public:
    FastCompatibilityTest();
    FastCompatibilityTest(unordered_map<string, linked_list_entry> component, unordered_map<int, unordered_set<string> > map, unordered_map<string, int> semi, int kl,
                          vector<occ_Node> seq, unordered_map<string, array_Node>* _array, unordered_map<int, unordered_set<graph_Edge, hash_Edge> > tedge,
                          unordered_map<int, unordered_set<graph_Edge, hash_Edge> > ntedge, unordered_map<graph_Edge, int, hash_Edge> edges, et_tree_Node* root, unordered_map<int, unordered_set<graph_Edge, hash_Edge> > father_children,
                          unordered_map<int, unordered_map<string, unordered_set<graph_Edge, hash_Edge> > > query_table, bool edge_promotion);

    int build(bool internal_labeled);
    void deletion(string l, Y* y, dis_Node* node, bool internal_labeled, dis_Node* node_potential);
    string convert2newick(dis_Node *tree, string newick);
    bool father_child_relation(string original, int num,string compare);
    void delete_component_edge(string u, string v, unordered_map<string, linked_list_entry> *com);
    dis_Node* get_display_graph(){return dis_graph;};
    ~FastCompatibilityTest();

public:
    unordered_map<string, int> initial_semi;
    unordered_map<string, linked_list_entry> hpu;
    unordered_map<int, unordered_set<string> > valpos;
    dis_Node* dis_graph;
    int sum_kl;

    unordered_map<string, array_Node> array;
    unordered_map<int, unordered_set<graph_Edge, hash_Edge> > _tedge ;
    unordered_map<int, unordered_set<graph_Edge, hash_Edge> > _ntedge ;
    unordered_set<string> label_count;
    list<Y> Q;
    list<Y> Q_potential;
    HDT hdt;
    dis_Node tree_root;
    vector<Y> vec;
    unordered_map<string, unordered_set<int> > label_tree;
    unordered_map<string, int> semi_label_tree;
    unordered_map<string, unordered_set<int> > unsemi_label_tree;
    unordered_map<graph_Edge, int, hash_Edge> _edges;
    unordered_map<int, unordered_set<graph_Edge, hash_Edge> > fachild;
    unordered_map<string, unordered_set<int> > subpool;
    int adj_size;
    int get_result_label(){return result_label;};

private:
    void set_hdt(HDT hdt1){hdt = hdt1;};
    int result_label = 0;
};


#endif //FGO_FASTCOMPATIBILITYTEST_H
