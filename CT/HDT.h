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
// HDT.h is the class for processing operations on treaps by using HDT algorithm
//

#ifndef FGO_HDT_H
#define FGO_HDT_H

#include "HDT_Structure.h"
#include "Linked_List_Builder.h"
#include <map>
#include <set>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include "Treap.h"
#include <stack>
#include <queue>
#include <ctime>
#include <random>

using namespace std;

class HDT {
public:
    HDT();
    HDT(vector<occ_Node> et_seq, unordered_map<string, array_Node> array, unordered_map<int, unordered_set<graph_Edge, hash_Edge> > t_edge,
        unordered_map<int, unordered_set<graph_Edge, hash_Edge> > nt_edge, unordered_map<graph_Edge, int, hash_Edge> edges,
        et_tree_Node* t, unordered_map<string, linked_list_entry> graph, int h, unordered_map<int, unordered_map<string, unordered_set<graph_Edge, hash_Edge> > > query_table,
    bool edge_promotion);

    void delete_Edge(string u, string v, bool fake);
    graph_Edge find_replace(int level, et_tree_Node* ct, string u, string v, et_tree_Node* ct2);
    void split_treap(string u, string v, int level_at);
    et_tree_Node* find_leftmost(et_tree_Node* tree);
    et_tree_Node* find_rightmost(et_tree_Node* tree);
    void cut_treap(string node1, string node2, et_tree_Node*& tree, int level_at);
    void rerooting(string u, et_tree_Node*& tree, int level_at);
    void increase_level_of_smaller_tree(int level, int e_level);
    void delete_hpu_edge(string u, string v);
    bool check_smaller(et_tree_Node* first_node, et_tree_Node* second_node);
    void cut_right(et_tree_Node*& tree, et_tree_Node*& ct1, et_tree_Node*& ct2, et_tree_Node*& position);
    void cut_left(et_tree_Node*& tree, et_tree_Node*& ct1, et_tree_Node*& ct2, et_tree_Node*& position);
    et_tree_Node* find_root(et_tree_Node* node);
    et_tree_Node* union_link(et_tree_Node* ct1, et_tree_Node* ct2, int level_at);

    bool connected(string u, string v, int level_at);
    bool fct_connected(string u, string v, int level_at);

    string smaller_tree_label(string l1, string l2, int level_at);
    int get_edge_level(string str1, string str2);

    ~HDT();

private:
    //euler tour sequence
    vector<occ_Node> et_sequence;
    //euler tour sequence list
    unordered_map<string, vector<occ_Node> > et_seq_list;
    //node list
    unordered_map<string, array_Node> node_array;
    //tree edge list
    unordered_map<int, unordered_set<graph_Edge, hash_Edge> > tree_edge;
    //non-tree edge list
    unordered_map<int, unordered_set<graph_Edge, hash_Edge> > non_tree_edge;
    //edge list
    unordered_map<graph_Edge, int, hash_Edge> edge_list;
    //treap
    et_tree_Node* treap;
    et_tree_Node* t1;
    et_tree_Node* t2;
    //treap list
    unordered_map<string, et_tree_Node*> treap_list;
    //treap list with level
    unordered_map<int, unordered_map<string, et_tree_Node*> > treap_list_with_level;
    //linked list of the tree
    unordered_map<string, linked_list_entry> hpu;
    //current 2 treaps
    unordered_map<int, vector<et_tree_Node*> > current_treaps;
    //height of this F
    int height;
    //priority
    int priority_g;

    unordered_set<et_tree_Node*> *backUpNodeSet = new unordered_set<et_tree_Node*>();

    unordered_map<int, unordered_map<string, unordered_set<graph_Edge, hash_Edge> > > non_tree_edge_query_table;

    unordered_set<graph_Edge, hash_Edge> moved_edge;

    vector<et_tree_Node*> compute_left_right_sequence(int dist, et_tree_Node* node);
    int compute_distance_to_root(et_tree_Node* node);


};


#endif //FGO_HDT_H
