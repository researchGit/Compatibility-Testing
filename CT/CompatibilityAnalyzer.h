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
// CompatibilityAnalyzer.h is designed for reading Newick format strings from the input files and then
// parsing them into destinating data structures. At the mean time, some other structures that are used by
// HDT algorithm will be initialized
//

#ifndef FGO_COMPATIBILITYANALYZER_H
#define FGO_COMPATIBILITYANALYZER_H

#include "Parser.h"
#include "unordered_map"
#include "Linked_List_Builder.h"
#include "HDT_Structure.h"
#include <stack>

using namespace std;

class CompatibilityAnalyzer {

public:
    void initializer(string file_path);
    void print_tree(tree_node* root);
    unordered_map<string, linked_list_entry> get_HPU(){return HPU;};
    int get_sum_kl(){return sum_kl;};
    unordered_map<int, unordered_set<string> > get_valid_position(){return valid_pos;};
    unordered_map<string, int> get_semi_universal_label(){return semi_universal_label;};
    unordered_map<int, unordered_set<graph_Edge, hash_Edge> > get_father_children(){return parent_child_set;};
    ~CompatibilityAnalyzer();

private:
    unordered_map<string, linked_list_entry> convert2list(tree_node *root, int tree_index);
    multimap<string, unordered_map<string, linked_list_entry> > input_trees;
    unordered_map<string, linked_list_entry> HPU;
    string convert2newick(tree_node *tree, string newick);

private:
    unordered_map<string, linked_list_entry> hpu;
    int sum_kl;
    unordered_map<int, unordered_set<string> > valid_pos;
    unordered_map<string, int> semi_universal_label;
    //parent-child relation is designed as one to one mapping relation in each tree
    unordered_map<int, unordered_set<graph_Edge, hash_Edge> > parent_child_set;
    //child-parent relation is designed as one to many mapping relation for the entire tree structure
    //because in the HPU structure, or the tree after merging all input trees together, one child node may
    //have several different parent nodes. These nodes are original from different input trees
    unordered_map<string, unordered_set<string> > child_parent_set;
    unordered_set<graph_Edge2, hash_Edge2> edge;
    unordered_set<linked_list_entry *> *backUpEntrySet = new unordered_set<linked_list_entry*>();
};


#endif //FGO_COMPATIBILITYANALYZER_H
