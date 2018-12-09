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
// InputGenerator.h is the class designed for generating compatible input samples
//

#ifndef FGO_INPUTGENERATOR_H
#define FGO_INPUTGENERATOR_H
#include <iostream>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include "Linked_List_Builder.h"
#include "HDT.h"
#include <inttypes.h>
#include <cinttypes>
#include <stdio.h>
#include "sstream"
#include <random>
#include <stack>
#include <queue>

using namespace std;

#define UNI32_MAX 4294967295

struct in_tree{
    string label;
    int sequence_value;
    bool selected_child;
    in_tree* l_child;
    in_tree* r_child;
    in_tree* parent;
    in_tree* substract_parent;
    in_tree* substract_l_child;
    in_tree* substract_r_child;
    in_tree(){l_child = NULL; r_child = NULL; parent = NULL; substract_parent = NULL; substract_l_child = NULL; substract_r_child = NULL;};
};

struct in_tree_non2{
    string label;
    vector<in_tree_non2*> children;
    in_tree_non2* parent;
    in_tree_non2* substract_parent;
    vector<in_tree_non2*> substract_children;
    in_tree_non2(){parent = NULL; substract_parent = NULL;};
};

struct simulate_stack{
    int _stack_top;
    vector<in_tree*> *_vector_stack;
};

struct simulate_stack_non2{
    int _stack_top;
    vector<in_tree_non2*> *_vector_stack;
};

class InputGenerator {

private:
    int label_count = 1;
    int final_label = 1;
    unordered_map<int, unordered_set<graph_Edge, hash_Edge> > father_child;
    unordered_map<int, unordered_set<string> > subsetsByindex;
    in_tree* tree_root;
    in_tree* sub_tree_root;
    in_tree_non2* sub_tree_root_non2;
    simulate_stack _vec_stack;
    simulate_stack_non2 _vec_stack_non2;
    vector<string> *label_list;
    vector<in_tree*> *node_list;
    vector<in_tree_non2*> *node_list_non2;
    stack<in_tree*> *preorder_stack;
    stack<in_tree_non2*> *preorder_stack_non2;
    unordered_map<string, linked_list_entry> global_hpu;
    unordered_map<string, unordered_set<string>* > *left_descendant;
    unordered_map<string, unordered_set<string>* > *right_descendant;
    unordered_map<string, in_tree*> *label_parent_map;
    unordered_set<string> *in_use_triplet;
    vector<string> *internal_nodes_vec;
    unordered_map<string, vector<unordered_set<string> *> *> *potential_tripletbylabel = new unordered_map<string, vector<unordered_set<string> *> *>();
    bool internal_label = false;
    unordered_set<in_tree*> *backUpTreeNode = new unordered_set<in_tree*>();
    unordered_set<linked_list_entry*> *backUpEntrySet = new unordered_set<linked_list_entry*>();
    unordered_set<unordered_set<string>* > *backUpPtrSet = new unordered_set<unordered_set<string>* >();
    unordered_set<in_tree_non2*> *backUpNonTreeNode = new unordered_set<in_tree_non2*>();

public:
    unordered_map<string, linked_list_entry> get_global_hpu(){return global_hpu;};
    void build_up_preorder_stack(in_tree* root);
    void build_up_preorder_stack_non2(in_tree_non2* root);
    void insert_to_subset(int subset_number, string _node_value);
    void triple_subset();
    void binary_tree_builder_with_internal_label(int N, int subset_number);
    void binary_tree_builder_leaf_label_only(int N, int subset_number);
    void triple_builder_leaf_label_only(int N);
    void initial_tree4triplet_test(int N, double portion, double lper);
    in_tree* initial_tree_construction(double lper, int lindex, int hindex);
    void basic_triplets();
    void potential_triplets();
    void additional_triplets(int N, double portion);
    in_tree_non2* non_binary_tree_builder_leaf_label_only(int N, int subset_number, int degree);
    void subtract_tree(unordered_set<string> subset);
    void subtract_tree_non2(unordered_set<string> subset, in_tree_non2* _tree_root, int degree);
    void clear_substract_structure(in_tree_non2* _tree_root);
    void convert_subtree_to_linked_list(unordered_map<string, linked_list_entry> &l_list, int tree_num);
    void convert_non2_subtree_to_linked_list(unordered_map<string, linked_list_entry> &l_list, int tree_num, in_tree_non2* subtree_root);
    void check_children_count(in_tree* root);
    bool get_internal_label(){return internal_label;};
    unordered_map<int, unordered_set<graph_Edge, hash_Edge> > get_father_children(){return father_child;};
    multimap<string, unordered_map<string, linked_list_entry> > build_hpu(in_tree *& tree);
    multimap<string, unordered_map<string, linked_list_entry> > build_triple_hpu(in_tree *& tree);
    multimap<string, unordered_map<string, linked_list_entry> > build_hpu_non2(in_tree_non2 *& tree, int degree);
    multimap<string, unordered_map<string, linked_list_entry> > construct_tree_with_internal_label(int number_of_nodes, int number_of_subsets);
    multimap<string, unordered_map<string, linked_list_entry> > construct_tree_leaf_label_only(int number_of_nodes, int number_of_subsets);
    multimap<string, unordered_map<string, linked_list_entry> > construct_triples(int number_of_nodes);
    multimap<string, unordered_map<string, linked_list_entry> > construct_specifiedTriples(int number_of_labels, double portion, double lper);
    multimap<string, unordered_map<string, linked_list_entry> > construct_non_binary_degree_tree(int number_of_nodes, int number_of_subsets, int degree);
    ~InputGenerator();
};


#endif //FGO_INPUTGENERATOR_H
