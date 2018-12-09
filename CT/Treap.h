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
// Treap.h implements the basic operations of treap
//

#ifndef FGO_TREAP_H
#define FGO_TREAP_H

#include "HDT_Structure.h"
#include <map>
#include <set>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <unordered_set>

using namespace std;

class Treap {

public:
    void build_Treap(vector<occ_Node> _euler_sequence, unordered_map<string, array_Node>* _node_array);
    void Treap_Print();
    void Treap_Print_PreOrder(et_tree_Node* root);
    void Treap_Rotate_Left(et_tree_Node *& node);
    void Treap_Rotate_Right(et_tree_Node *& node);
    ~Treap();

public:
    et_tree_Node* tree_root;
    unordered_map<string, array_Node>* node_array;
    et_tree_Node* last_visited_tree_node;

private:
    void create_random_integer_sequence(int number_sequence[], int seq_size);
    int Treap_Insert(et_tree_Node *& cursor, double number, string value, int priority,
                     bool active, et_tree_Node* _parent_pointer);
    void update_active_child_left_to_right(et_tree_Node *& node, et_tree_Node *& temp );
    void update_active_child_right_to_left(et_tree_Node *& temp, et_tree_Node *& node );
    void update_treap_size(et_tree_Node *& node);


private:
    int treap_size = 0;
    int th = 0;
    unordered_set<et_tree_Node*> *backUpTreapNode = new unordered_set<et_tree_Node*>();

};


#endif //FGO_TREAP_H
