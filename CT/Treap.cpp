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
// Detailed implementation of Treap.h
//

#include "Treap.h"
#include <random>

using namespace std;

//Print
void Treap::Treap_Print() {
    Treap_Print_PreOrder(tree_root);
}

string convert2newick(et_tree_Node *tree, string newick) {
    if(tree != NULL){
        if(tree->l_child != nullptr || tree->r_child != nullptr){
            //this tree has the children
            newick += "(";
            newick = convert2newick(tree->l_child, newick);
            newick += ',';
            newick = convert2newick(tree->r_child, newick);
            if(newick[newick.length()-1] == ','){
                newick = newick.substr(0, newick.length()-1);
            }
            newick += ")";
        }

        string value = tree->value;
        //in all the data we used, there was no weight associated with any label, so we simply leave it empty here
        string weight = "";

        if(weight.length() > 0){
            value += ":";
            value += weight;
        }

        newick += value;
    }
    return newick;
}

//Pre order
void Treap::Treap_Print_PreOrder(et_tree_Node *root) {
    if(root != NULL) {
        Treap_Print_PreOrder(root->l_child);

        cout << root << " " << "number:" << root->number << " value:" << root->value << " priority:" << root->priority << " active:" <<root->active;
        if(root->parent != NULL) {
            cout << " parent:" << root->parent->number;
        }
        cout << " active child: " << root->act_child << " subtree size: " << root->node_weight;
        if(root->parent){
            cout << " parent: " << root->parent;
        }
        cout << endl;
        Treap_Print_PreOrder(root->r_child);
    }
}

//construct a random sequence of a certain number of integer number
//these numbers are used as the priority of the treap node
void Treap::create_random_integer_sequence(int *number_sequence, int seq_size) {
    random_device seq_index_rd;
    mt19937_64 seq_index_mt(seq_index_rd());
    uniform_int_distribution<int> seq_index_dist(1, INT32_MAX);

    for(int i = 0; i < seq_size; i++) {
        number_sequence[i] = seq_index_dist(seq_index_mt);
    }
}

//construct the treap
//priority is generated with a method posted on url: blog.ruofeidu.com/treao-in-45-lines-of-c/
void Treap::build_Treap(vector<occ_Node> _euler_sequence, unordered_map<string, array_Node> *_node_array) {
    node_array = _node_array;
    tree_root = NULL;
    last_visited_tree_node = NULL;

    //compute random priority
    int size = (int)_euler_sequence.size();
    int arr[size];
    memset(arr,0,sizeof(arr));
    for(int i = 0; i < size ; i++){
        arr[i] = ((rand() << 15) + rand()) % 200000000; //use new method to generate the random number
    }

//    create_random_integer_sequence(arr, size);

    //build the treap
    for(int i = 0; i < size; i++){
        et_tree_Node* temp = NULL;
        Treap_Insert(tree_root, _euler_sequence[i].number, _euler_sequence[i].value, arr[i],
                     _euler_sequence[i].active, temp);
    }

//    cout << "treap is " << endl;
//    Treap_Print_PreOrder(tree_root);
//    string newick = "";
//    newick = convert2newick(tree_root, newick);
//    cout << newick << endl;
}

//insert an elemetn in to the treap
int Treap::Treap_Insert(et_tree_Node *&cursor, double number, string value, int priority, bool active, et_tree_Node *_parent_pointer) {
    //if the cursor finds the place that is empty
    if(cursor == nullptr) {
        cursor = new et_tree_Node(number, value, priority, active);
        backUpTreapNode->insert(cursor);
        cursor->parent = _parent_pointer;
        unordered_map<string, array_Node>::iterator _array_entry_finder = (*node_array).find(value);
        if(last_visited_tree_node == nullptr){
            last_visited_tree_node = cursor;
        }
        else{
            if(_array_entry_finder != (*node_array).end()){
                unordered_map<int, unordered_map<string, unordered_set<et_tree_Node*> > >::iterator level_entry = _array_entry_finder->second.node_occr.find(0);
                if(level_entry != _array_entry_finder->second.node_occr.end()){
                    unordered_map<string, unordered_set<et_tree_Node*> >::iterator _occur_finder = level_entry->second.find(last_visited_tree_node->value);
                    if(_occur_finder != level_entry->second.end()){
                        //exists, then insert
                        _occur_finder->second.insert(last_visited_tree_node);
                    }
                    else{
                        unordered_set<et_tree_Node*> tempset;
                        tempset.insert(last_visited_tree_node);
                        level_entry->second.insert(unordered_map<string, unordered_set<et_tree_Node*> >::value_type(last_visited_tree_node->value, tempset));
                    }
                }
                else{
                    unordered_set<et_tree_Node*> temp_set;
                    temp_set.insert(last_visited_tree_node);
                    unordered_map<string, unordered_set<et_tree_Node*> > temp_label_occur_map;
                    temp_label_occur_map.insert(unordered_map<string, unordered_set<et_tree_Node*> >::value_type(last_visited_tree_node->value, temp_set));
                    _array_entry_finder->second.node_occr.insert(unordered_map<int, unordered_map<string, unordered_set<et_tree_Node*> > >::value_type(0, temp_label_occur_map));
                }
            }
            unordered_map<string, array_Node>::iterator _array_entry_finder2 = (*node_array).find(last_visited_tree_node->value);
            if(_array_entry_finder2 != (*node_array).end()){
                unordered_map<int, unordered_map<string, unordered_set<et_tree_Node*> > >::iterator level_entry = _array_entry_finder2->second.node_occr.find(0);
                if(level_entry != _array_entry_finder2->second.node_occr.end()){
                    unordered_map<string, unordered_set<et_tree_Node*> >::iterator _occur_finder2 = level_entry->second.find(value);
                    if(_occur_finder2 != level_entry->second.end()){
                        //exist, then insert
                        _occur_finder2->second.insert(cursor);
                    }
                    else{
                        unordered_set<et_tree_Node*> tempset;
                        tempset.insert(cursor);
                        level_entry->second.insert(unordered_map<string, unordered_set<et_tree_Node*> >::value_type(value, tempset));
                    }
                }
                else{
                    unordered_set<et_tree_Node*> temp_set;
                    temp_set.insert(cursor);
                    unordered_map<string, unordered_set<et_tree_Node*> > temp_label_occur_map;
                    temp_label_occur_map.insert(unordered_map<string, unordered_set<et_tree_Node*> >::value_type(value, temp_set));
                    _array_entry_finder2->second.node_occr.insert(unordered_map<int, unordered_map<string, unordered_set<et_tree_Node*> > >::value_type(0, temp_label_occur_map));
                }
            }
            last_visited_tree_node = cursor;
        }
        treap_size++;

        //act_node1 could be replaced
        if(active){
            unordered_map<int, et_tree_Node*>::iterator level_occur_entry = (*node_array)[value].act_Node1.find(0);
            if(level_occur_entry != (*node_array)[value].act_Node1.end()){
                (*node_array)[value].act_Node1.erase(level_occur_entry);
            }
            (*node_array)[value].act_Node1.insert(unordered_map<int, et_tree_Node*>::value_type(0, cursor));
        }
    }
    else if(cursor->number > number){
        //if the current node has the larger value than key
        cursor->node_weight++;
        Treap_Insert(cursor->l_child, number, value, priority, active, cursor);
        if(cursor->priority > cursor->l_child->priority) {
            //fix the treap
            Treap_Rotate_Right(cursor);
        }
        if(cursor->l_child != nullptr){
            if(cursor->l_child->active || cursor->l_child->act_child){
                cursor->act_child = true;
            }
        }
        if(cursor->r_child != nullptr){
            if(cursor->r_child->active || cursor->r_child->act_child){
                cursor->act_child = true;
            }
        }
    }
    else if(cursor->number < number){
        //if the current node has the smaller value than key
        cursor->node_weight++;
        Treap_Insert(cursor->r_child, number, value, priority, active, cursor);
        if(cursor->priority > cursor->r_child->priority) {
            //fix the treap
            Treap_Rotate_Left(cursor);
        }
        if(cursor->l_child != nullptr){
            if(cursor->l_child->active || cursor->l_child->act_child){
                cursor->act_child = true;
            }
        }
        if(cursor->r_child != nullptr){
            if(cursor->r_child->active || cursor->r_child->act_child){
                cursor->act_child = true;
            }
        }
    }
    else{
        //if the current node has the same value as the key
        //we do nothing here
        return  0;
    }
    return 1;
}

//active child is used in the treap to mark if the desendents of a node have one or more non-tree edge
void Treap::update_active_child_left_to_right(et_tree_Node *&node, et_tree_Node *&temp) {
    if(node->l_child != NULL){
        node->l_child->parent = node;
        temp->node_weight -= node->l_child->node_weight;
        if(node->l_child->active || node->l_child->act_child){
            node->act_child = true;
        }
        else{
            if(node->r_child != NULL){
                if(!(node->r_child->active || node->r_child->act_child)){
                    node->act_child = false;
                }
                else{
                    node->act_child = true;
                }
            }
            else{
                node->act_child = false;
            }
        }
    }
    else{
        if(node->r_child != NULL){
            if(!(node->r_child->active || node->r_child->act_child)){
                node->act_child = false;
            }
            else{
                node->act_child = true;
            }
        }
        else{
            node->act_child = false;
        }
    }
}

//active child is used in the treap to mark if the desendents of a node have one or more non-tree edge
void Treap::update_active_child_right_to_left(et_tree_Node *&temp, et_tree_Node *&node) {
    if(temp->r_child != NULL){
        temp->r_child->parent = temp;
        if(temp->r_child->active || temp->r_child->act_child){
            temp->act_child = true;
        }
        else{
            if(temp->l_child != NULL){
                if(!(temp->l_child->active || temp->l_child->act_child)){
                    temp->act_child = false;
                }
                else{
                    temp->act_child = true;
                }
            }
            else{
                temp->act_child = false;
            }
        }
    }
    else{
        if(temp->l_child != NULL){
            if(!(temp->l_child->active || temp->l_child->act_child)){
                temp->act_child = false;
            }
            else{
                temp->act_child = true;
            }
        }
        else{
            temp->act_child = false;
        }
    }
}

//when rotate the treap, the size of each subtree should be updated correctly
void Treap::update_treap_size(et_tree_Node *&node) {
    if(node->l_child != NULL && node->r_child != NULL){
        node->node_weight = node->l_child->node_weight + node->r_child->node_weight + 1;
    }
    else if(node->l_child != NULL && node->r_child == NULL){
        node->node_weight = node->l_child->node_weight + 1;
    }
    else if(node->l_child == NULL && node->r_child != NULL){
        node->node_weight = node->r_child->node_weight + 1;
    }
    else{
        node->node_weight = 1;
    }
}


//Right Rotation
void Treap::Treap_Rotate_Right(et_tree_Node *& node) {
    if(node == NULL) {
        return;
    }
    et_tree_Node* temp = NULL;
    et_tree_Node* parent = node->parent;
    int left_right_mark = -1;
    if(parent != NULL){
        if(parent->l_child == node){
            left_right_mark = 0;
        }
        else if(parent->r_child == node){
            left_right_mark = 1;
        }
    }
    temp = node->l_child;
    node->l_child = temp->r_child;
    if(node->l_child != NULL){
        node->l_child->parent = node;
        temp->node_weight -= node->l_child->node_weight;
        if(node->l_child->active || node->l_child->act_child){
            node->act_child = true;
        }
        else{
            if(node->r_child != NULL){
                if(!(node->r_child->active || node->r_child->act_child)){
                    node->act_child = false;
                }
                else{
                    node->act_child = true;
                }
            }
            else{
                node->act_child = false;
            }
        }
    }
    else{
        if(node->r_child != NULL){
            if(!(node->r_child->active || node->r_child->act_child)){
                node->act_child = false;
            }
            else{
                node->act_child = true;
            }
        }
        else{
            node->act_child = false;
        }
    }
    if(node->l_child != NULL && node->r_child != NULL){
        node->node_weight = node->l_child->node_weight + node->r_child->node_weight + 1;
    }
    else if(node->l_child != NULL && node->r_child == NULL){
        node->node_weight = node->l_child->node_weight + 1;
    }
    else if(node->l_child == NULL && node->r_child != NULL){
        node->node_weight = node->r_child->node_weight + 1;
    }
    else{
        node->node_weight = 1;
    }
    temp->r_child = node;
    if(temp->r_child != NULL){
        temp->r_child->parent = temp;
        if(temp->r_child->active || temp->r_child->act_child){
            temp->act_child = true;
        }
        else{
            if(temp->l_child != NULL){
                if(!(temp->l_child->active || temp->l_child->act_child)){
                    temp->act_child = false;
                }
                else{
                    temp->act_child = true;
                }
            }
            else{
                temp->act_child = false;
            }
        }
    }
    else{
        if(temp->l_child != NULL){
            if(!(temp->l_child->active || temp->l_child->act_child)){
                temp->act_child = false;
            }
            else{
                temp->act_child = true;
            }
        }
        else{
            temp->act_child = false;
        }
    }
    if(temp->l_child != NULL && temp->r_child != NULL){
        temp->node_weight = temp->l_child->node_weight + temp->r_child->node_weight + 1;
    }
    else if(temp->l_child != NULL && temp->r_child == NULL){
        temp->node_weight = temp->l_child->node_weight + 1;
    }
    else if(temp->l_child == NULL && temp->r_child != NULL){
        temp->node_weight = temp->r_child->node_weight + 1;
    }
    else{
        temp->node_weight = 1;
    }
    node = temp;
    node->parent = parent;
    if(parent != NULL){
        if(left_right_mark == 0){
            parent->l_child = node;
        }
        else if(left_right_mark == 1){
            parent->r_child = node;
        }
    }
}

//Left Rotation
void Treap::Treap_Rotate_Left(et_tree_Node *& node) {
    if(node == NULL){
        return;
    }
    et_tree_Node* temp = NULL;
    et_tree_Node* parent = node->parent;
    int left_right_mark = -1;
    if(parent != NULL){
        if(parent->l_child == node){
            left_right_mark = 0;
        }
        else if(parent->r_child == node){
            left_right_mark = 1;
        }
    }

    temp = node->r_child;
    node->r_child = temp->l_child;
    if(node->r_child != NULL){
        node->r_child->parent = node;
        temp->node_weight -= node->r_child->node_weight;
        if(node->r_child->active || node->r_child->act_child){
            node->act_child = true;
        }
        else{
            if(node->l_child != NULL){
                if(!(node->l_child->active || node->l_child->act_child)){
                    node->act_child = false;
                }
                else{
                    node->act_child = true;
                }
            }
            else{
                node->act_child = false;
            }
        }
    }
    else{
        if(node->l_child != NULL){
            if(!(node->l_child->active || node->l_child->act_child)){
                node->act_child = false;
            }
            else{
                node->act_child = true;
            }
        }
        else{
            node->act_child = false;
        }
    }
    if(node->l_child != NULL && node->r_child != NULL){
        node->node_weight = node->l_child->node_weight + node->r_child->node_weight + 1;
    }
    else if(node->l_child != NULL && node->r_child == NULL){
        node->node_weight = node->l_child->node_weight + 1;
    }
    else if(node->l_child == NULL && node->r_child != NULL){
        node->node_weight = node->r_child->node_weight + 1;
    }
    else{
        node->node_weight = 1;
    }
    temp->l_child = node;
    if(temp->l_child != NULL){
        temp->l_child->parent = temp;
        if(temp->l_child->active || temp->l_child->act_child){
            temp->act_child = true;
        }
        else{
            if(temp->r_child != NULL){
                if(!(temp->r_child->active || temp->r_child->act_child)){
                    temp->act_child = false;
                }
                else{
                    temp->act_child = true;
                }
            }
            else{
                temp->act_child = false;
            }
        }
    }
    else{
        if(temp->r_child != NULL){
            if(!(temp->r_child->active || temp->r_child->act_child)){
                temp->act_child = false;
            }
            else{
                temp->act_child = true;
            }
        }
        else{
            temp->act_child = false;
        }
    }
    if(temp->l_child != NULL && temp->r_child != NULL){
        temp->node_weight = temp->l_child->node_weight + temp->r_child->node_weight + 1;
    }
    else if(temp->l_child != NULL && temp->r_child == NULL){
        temp->node_weight = temp->l_child->node_weight + 1;
    }
    else if(temp->l_child == NULL && temp->r_child != NULL){
        temp->node_weight = temp->r_child->node_weight + 1;
    }
    else{
        temp->node_weight = 1;
    }
    node = temp;

    node->parent = parent;
    if(parent != NULL){
        if(left_right_mark == 0){
            parent->l_child = node;
        }
        else if(left_right_mark == 1){
            parent->r_child = node;
        }
    }
}

Treap::~Treap() {
    for(unordered_set<et_tree_Node*>::iterator iterator1 = backUpTreapNode->begin(); iterator1 != backUpTreapNode->end();
    iterator1++){
        delete *iterator1;
    }
    delete backUpTreapNode;
}