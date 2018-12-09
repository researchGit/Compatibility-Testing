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
// Detailed implementation of InputGenerator.h
// This class uses two main algorithms:
// 1. when generating a unbiased binary tree, the algorithm proposed by Martin Orr are used
// 2. when generating a unbiased binary tree, a random number shall be generated as one of the parameter of Martin Orr's method
// the random number is implemented by using the method of , this method will finally return a 32 bit integer number

#include "InputGenerator.h"

using namespace std;

//can be deleted later
void postorder(in_tree* t){
    if(t != NULL){
        postorder(t->l_child);
        postorder(t->r_child);
        cout << "tree label ----- " << t->label << "   < ------- >   ";
        if(t->parent){
            cout << t->parent->label << endl;
        }
        else{
            cout << endl;
        }
    }
}

//can be deleted later
void preorder(in_tree* t){
    if(t != NULL){
        cout << "tree label ----- " << t->label << "   < ------- >   ";
        if(t->parent){
            cout << t->parent->label << endl;
        }
        else{
            cout << endl;
        }
        preorder(t->l_child);
        preorder(t->r_child);
    }
}

//can be deleted later
void preorder_non(in_tree_non2* t){
    if(t != NULL){
        cout << "tree label ----- " << t->label << "   < ------- >   ";
        if(t->parent){
            cout << t->parent->label << endl;
        }
        else{
            cout << endl;
        }
        for(int i = 0 ; i < t->children.size(); i++){
            preorder_non(t->children[i]);
        }
    }
}

//can be deleted later
void preorder_2(in_tree* t){
    if(t != NULL){
        cout << "tree label ---- " << t->label << "   < ------- >   ";
        if(t->substract_parent){
            cout << t->substract_parent->label << endl;
        }
        else
            cout << endl;
        preorder_2(t->substract_l_child);
        preorder_2(t->substract_r_child);
    }
}

void preorder_non2_2(in_tree_non2* t){
    if(t != NULL){
        cout << "tree label ---- " << t->label << "   < ------- >   ";
        if(t->substract_parent){
            cout << t->substract_parent->label << endl;
        }
        else{
            cout << endl;
        }
        for(int i = 0 ; i < t->substract_children.size(); i++){
            preorder_non2_2(t->substract_children[i]);
        }
    }
}

void InputGenerator::check_children_count(in_tree* root){
    if(root != NULL){
        if(root->substract_l_child == NULL && root->substract_r_child != NULL){
            internal_label = true;
        }
        else if(root->substract_l_child != NULL && root->substract_r_child == NULL){
            internal_label = true;
        }
        check_children_count(root->substract_l_child);
        check_children_count(root->substract_r_child);
    }
}

// 1. generate a non-binary degree tree as a seed tree
// 2. generate a list of input trees
// 3. generate a hpu list
multimap<string, unordered_map<string, linked_list_entry> > InputGenerator::construct_non_binary_degree_tree(
        int number_of_nodes, int number_of_subsets, int degree) {
    //initialize the stack structure
    _vec_stack_non2._stack_top = 1;
    _vec_stack_non2._vector_stack = new vector<in_tree_non2*>;
    label_list = new vector<string>;
    node_list_non2 = new vector<in_tree_non2*>;

    //step 1
    in_tree_non2* non2_tree_root = non_binary_tree_builder_leaf_label_only(number_of_nodes, number_of_subsets, degree);

    //step 2
    build_up_preorder_stack_non2(non2_tree_root);

    //step 3
    multimap<string, unordered_map<string, linked_list_entry> > _input_trees = build_hpu_non2(non2_tree_root, degree);

    return _input_trees;
}

//construct a specified binary tree, and a set of basic triplets
//add some more additional triplets randomly
//extract each subtree by using subsets
multimap<string, unordered_map<string, linked_list_entry> > InputGenerator::construct_specifiedTriples(
        int number_of_labels, double portion, double lper) {
    //initialize the stack structure
    _vec_stack._stack_top = -1;
    _vec_stack._vector_stack = new vector<in_tree*>;
    label_list = new vector<string>;
    node_list = new vector<in_tree*>;

    internal_nodes_vec = new vector<string>;
    label_parent_map = new unordered_map<string, in_tree*>;
    in_use_triplet = new unordered_set<string>;

    //step 1
    initial_tree4triplet_test(number_of_labels, portion, lper);

    //step 2
    build_up_preorder_stack(tree_root);

    //step 3
    multimap<string, unordered_map<string, linked_list_entry> > _input_trees = build_triple_hpu(tree_root);

    return _input_trees;
}

//construct a binary tree, and a set of subsets with size of 3
//extract each subtree by using subsets
multimap<string, unordered_map<string, linked_list_entry> > InputGenerator::construct_triples(int number_of_nodes) {
    //initialize the stack structure
    _vec_stack._stack_top = -1;
    _vec_stack._vector_stack = new vector<in_tree*>;
    label_list = new vector<string>;
    node_list = new vector<in_tree*>;

    //step 1
    triple_builder_leaf_label_only(number_of_nodes);

    //step 2
    build_up_preorder_stack(tree_root);

    //step 3
    multimap<string, unordered_map<string, linked_list_entry> > _input_trees = build_triple_hpu(tree_root);

    return _input_trees;
}

//construct a binary tree, and a set of subtrees by extracting each subtree from the binary tree
multimap<string, unordered_map<string, linked_list_entry> > InputGenerator::construct_tree_leaf_label_only(int number_of_nodes, int number_of_subsets) {
    //initialize the stack structure
    _vec_stack._stack_top = -1;
    _vec_stack._vector_stack = new vector<in_tree*>;
    label_list = new vector<string>;
    node_list = new vector<in_tree*>;

    //step 1
    binary_tree_builder_leaf_label_only(number_of_nodes, number_of_subsets);
    //step 2
    build_up_preorder_stack(tree_root);

    //step 3
    multimap<string, unordered_map<string, linked_list_entry> > _input_trees = build_hpu(tree_root);
    return _input_trees;
}

//main entry of input generator
//1. construct the binary tree using buttom-up method
//2. construct the preorder sequence of the binary tree
//3. subtract the trees according to the subsets and convert binary tree to linked list tree(hpu)
multimap<string, unordered_map<string, linked_list_entry> > InputGenerator::construct_tree_with_internal_label(int number_of_nodes, int number_of_subsets) {
    //initialize the stack structure
    _vec_stack._stack_top = -1;
    _vec_stack._vector_stack = new vector<in_tree*>;
    label_list = new vector<string>;
    node_list = new vector<in_tree*>;

    //step 1
    binary_tree_builder_with_internal_label(number_of_nodes, number_of_subsets);

    //step 2
    build_up_preorder_stack(tree_root);
    //step 3
    multimap<string, unordered_map<string, linked_list_entry> > _input_trees = build_hpu(tree_root);

    return _input_trees;
}

//construct the preorder sequence by using non-recursive method
void InputGenerator::build_up_preorder_stack_non2(in_tree_non2 *root) {
    preorder_stack_non2 = new stack<in_tree_non2*>;
    preorder_stack_non2->push(root);
    while(!(preorder_stack_non2->empty())){
        in_tree_non2* cur_node = preorder_stack_non2->top();
        preorder_stack_non2->pop();
        if(cur_node != NULL){
            _vec_stack_non2._vector_stack->push_back(cur_node);
            _vec_stack_non2._stack_top++;
            for(int i = (int)cur_node->children.size() - 1 ; i >= 0; i--){
                preorder_stack_non2->push(cur_node->children[i]);
            }
        }
    }
    delete preorder_stack_non2;
}

//construct the preorder sequence by using non-recursive method
void InputGenerator::build_up_preorder_stack(in_tree *root) {
    preorder_stack = new stack<in_tree*>;
    in_tree* cur_node = root;
    while(cur_node != NULL || !(preorder_stack->empty())){
        while(cur_node != NULL){
            _vec_stack._vector_stack->push_back(cur_node);
            _vec_stack._stack_top++;
            preorder_stack->push(cur_node);
            cur_node = cur_node->l_child;
        }
        if(!(preorder_stack->empty())){
            cur_node = preorder_stack->top();
            preorder_stack->pop();
            cur_node = cur_node->r_child;
        }
    }
    delete preorder_stack;
}

//this method is designed to build up all subsets with size of 3
void InputGenerator::triple_subset() {
    int subset_index = 1;
    for(int i = 0; i < label_list->size() - 2; i++){
        string label1 = (*label_list)[i];
        for(int j = i + 1; j < label_list->size() - 1; j++){
            string label2 = (*label_list)[j];
            for(int k = j + 1; k < label_list->size(); k++){
                string label3 = (*label_list)[k];
                unordered_map<int, unordered_set<string> >::iterator subsetsByindex_finder = subsetsByindex.find(subset_index);
                if(subsetsByindex_finder != subsetsByindex.end()){
                    subsetsByindex_finder->second.insert(label1);
                    subsetsByindex_finder->second.insert(label2);
                    subsetsByindex_finder->second.insert(label3);
                }
                else{
                    unordered_set<string> temp_set;
                    temp_set.insert(label1);
                    temp_set.insert(label2);
                    temp_set.insert(label3);
                    subsetsByindex.insert(unordered_map<int, unordered_set<string> >::value_type(subset_index, temp_set));
                    subset_index++;
                }
            }
        }
    }
}

//this method is designed to insert the label of a node into some subsets
//this method will ensure each label is in at least one subset
void InputGenerator::insert_to_subset(int subset_number, string _node_value){
    //generate random number 0 or 1, insert this label into corresponding subset
    random_device subset_rd;
    mt19937_64 subset_mt(subset_rd());
    uniform_int_distribution<int> subset_dist(0, 1);
    int _label_in_no_subsets = 0;
    for(int i = 0; i < subset_number; i++){
        int temp_random = subset_dist(subset_mt);
        if(temp_random == 1){
            unordered_map<int, unordered_set<string> >::iterator subsetsByindex_finder = subsetsByindex.find(i + 1);
            if(subsetsByindex_finder != subsetsByindex.end()){
                //find it
                subsetsByindex_finder->second.insert(_node_value);
            }
            else{
                unordered_set<string> temp_set;
                temp_set.insert(_node_value);
                subsetsByindex.insert(unordered_map<int, unordered_set<string> >::value_type(i + 1, temp_set));
            }
        }
        else{
            unordered_map<int, unordered_set<string> >::iterator subsetsByindex_finder = subsetsByindex.find(i + 1);
            if(subsetsByindex_finder == subsetsByindex.end()){
                unordered_set<string> temp_set;
                subsetsByindex.insert(unordered_map<int, unordered_set<string> >::value_type(i + 1, temp_set));
            }
            _label_in_no_subsets++;
        }
    }
    //ensure that every label is in the one of the subsets
    if(_label_in_no_subsets == subset_number){
        random_device add_subset_rd;
        mt19937_64 add_subset_mt(add_subset_rd());
        uniform_int_distribution<int> add_subset_dist(1, subset_number);
        int add_subset_random_number = add_subset_dist(add_subset_mt);
        unordered_map<int, unordered_set<string> >::iterator subsetsByindex_finder = subsetsByindex.find(add_subset_random_number);
        if(subsetsByindex_finder != subsetsByindex.end()){
            //find it
            subsetsByindex_finder->second.insert(_node_value);
        }
        else{
            unordered_set<string> temp_set;
            temp_set.insert(_node_value);
            subsetsByindex.insert(unordered_map<int, unordered_set<string> >::value_type(add_subset_random_number, temp_set));
        }
    }
}

//main method for constructing a binary tree from buttom to up
//this method will generate a reasoning number k of leaf nodes which is less than the number of labeled nodes N given
//then it will build up a binary tree with 2k - 1 nodes which contains N labeled nodes and 2k - 1 - N unlabeled nodes
//which means that some of the internal nodes should be labeled
void InputGenerator::binary_tree_builder_with_internal_label(int N, int subset_number) {
    //firstly, randomly generate a reasoning number which is between [N/2 +1/2, N -1] if N is an odd number which is larger than 2
    //between [N/2 + 3/2, N - 1] if N is an even number which is larger than 3
    //when N is 1 and 2, situations will be discussed seperately
    if(N == 1){
        in_tree* _temp_tree_node = new in_tree();
        backUpTreeNode->insert(_temp_tree_node);
        _temp_tree_node->label = "label_1";
        _temp_tree_node->selected_child = false;

        label_list->push_back("label_1");
        for(int i = 0; i < subset_number; i++){
            unordered_map<int, unordered_set<string> >::iterator subsetsByindex_finder = subsetsByindex.find(i + 1);
            if(subsetsByindex_finder != subsetsByindex.end()){
                //find it
                subsetsByindex_finder->second.insert("label_1");
            }
            else{
                unordered_set<string> temp_set;
                temp_set.insert("label_1");
                subsetsByindex.insert(unordered_map<int, unordered_set<string> >::value_type(i + 1, temp_set));
            }
        }
        tree_root = _temp_tree_node;
    }
    else if(N == 2){
        in_tree* _temp_tree_node1 = new in_tree();
        in_tree* _temp_tree_node2 = new in_tree();

        backUpTreeNode->insert(_temp_tree_node1);
        backUpTreeNode->insert(_temp_tree_node2);

        _temp_tree_node1->label = "label_1";
        _temp_tree_node1->selected_child = false;

        _temp_tree_node2->label = "label_2";
        _temp_tree_node2->selected_child = false;

        label_list->push_back("label_1");
        label_list->push_back("label_2");

        _temp_tree_node1->l_child = _temp_tree_node2;
        _temp_tree_node2->parent = _temp_tree_node1;

        for(int i = 0; i < 2; i++){
            stringstream ss;
            ss << i + 1;
            string _temp_tree_node_value = "label_" + ss.str();
            insert_to_subset(subset_number, _temp_tree_node_value);
        }

        tree_root = _temp_tree_node1;
    }
    else if(N >= 3){
        int leaf_number = 0;
        int total_node_number = 0;
        random_device leaf_number_rd;
        mt19937_64 leaf_number_mt(leaf_number_rd());
        if(N % 2 == 0){
            //if the number of input is even
            uniform_int_distribution<int> leaf_number_dist((N + 1)/2 + 1, N - 1);
            leaf_number = leaf_number_dist(leaf_number_mt);
        }
        else{
            //if the number of input is odd
            uniform_int_distribution<int> leaf_number_dist((N + 1)/2, N - 1);
            leaf_number = leaf_number_dist(leaf_number_mt);
        }
        total_node_number = leaf_number * 2 - 1;
        //construct the leaf vector
        for(int i = 0; i < leaf_number; i++){
            stringstream ss;
            ss << label_count++;
            in_tree* _temp_tree_node = new in_tree();
            backUpTreeNode->insert(_temp_tree_node);
            string _temp_tree_node_value = "label_" + ss.str();
            _temp_tree_node->label = _temp_tree_node_value;
            _temp_tree_node->selected_child = false;
            node_list->push_back(_temp_tree_node);
            label_list->push_back(_temp_tree_node_value);
            insert_to_subset(subset_number, _temp_tree_node_value);
        }
        int remaining_label_number = N - leaf_number;
        int remaining_node_number = total_node_number - leaf_number;

        random_device input_index_rd;
        mt19937_64 input_index_mt(input_index_rd());
        while(node_list->size() > 1){
            uniform_int_distribution<int> input_index_dist(1, (int)node_list->size());
            int index_1 = input_index_dist(input_index_mt) - 1;
            int index_2 = input_index_dist(input_index_mt) - 1;
            if(index_1 == index_2){
                if(index_2 < node_list->size() - 1){
                    index_2++;
                }
                else{
                    index_2--;
                }
            }
            in_tree* _temp_tree_node1 = (*node_list)[index_1];
            in_tree* _temp_tree_node2 = (*node_list)[index_2];
            in_tree* _temp_tree_node3 = new in_tree();
            backUpTreeNode->insert(_temp_tree_node3);
            _temp_tree_node3->selected_child = false;
            stringstream ss;
            ss << label_count++;
            if(remaining_label_number < remaining_node_number){
                if(remaining_label_number > 0){
                    random_device label_or_not_rd;
                    mt19937_64 label_or_not_mt(label_or_not_rd());
                    uniform_int_distribution<int> label_or_not_dist(0,1);
                    int label_or_not = label_or_not_dist(label_or_not_mt);
                    if(label_or_not == 1){
                        _temp_tree_node3->label = "ilabel_" + ss.str();
                        remaining_label_number--;
                        label_list->push_back(_temp_tree_node3->label);
                        insert_to_subset(subset_number, _temp_tree_node3->label);
                    }
                    else{
                        _temp_tree_node3->label = "unlabel_" + ss.str();
                    }
                }
                else{
                    _temp_tree_node3->label = "unlabel_" + ss.str();
                }
                remaining_node_number--;
            }
            else{
                _temp_tree_node3->label = "ilabel_" + ss.str();
                remaining_node_number--;
                remaining_label_number--;
                label_list->push_back(_temp_tree_node3->label);
                insert_to_subset(subset_number, _temp_tree_node3->label);
            }
            _temp_tree_node3->l_child = _temp_tree_node1;
            _temp_tree_node3->r_child = _temp_tree_node2;
            _temp_tree_node1->parent = _temp_tree_node3;
            _temp_tree_node2->parent = _temp_tree_node3;

            if(index_1 < index_2){
                vector<in_tree*>::iterator ele = node_list->begin() + index_2;
                node_list->erase(ele);
                ele = node_list->begin() + index_1;
                node_list->erase(ele);
            }
            else{
                vector<in_tree*>::iterator ele = node_list->begin() + index_1;
                node_list->erase(ele);
                ele = node_list->begin() + index_2;
                node_list->erase(ele);
            }
            node_list->push_back(_temp_tree_node3);
        }
        tree_root = (*node_list)[0];
    }
}

//only consider the situation that N >= 3, which is designed for experiments
in_tree_non2* InputGenerator::non_binary_tree_builder_leaf_label_only(int N, int subset_number, int degree) {
    in_tree_non2* return_root = NULL;
    if(N >= degree){
        //construct the leaf vector
        for(int i = 0; i < N; i++){
            stringstream ss;
            ss << label_count++;
            in_tree_non2* _temp_tree_node = new in_tree_non2();
            backUpNonTreeNode->insert(_temp_tree_node);
            string _temp_tree_node_value = "label_" + ss.str();
            _temp_tree_node->label = _temp_tree_node_value;
            node_list_non2->push_back(_temp_tree_node);
            label_list->push_back(_temp_tree_node_value);
            insert_to_subset(subset_number, _temp_tree_node_value);
        }

        random_device input_index_rd;
        mt19937_64 input_index_mt(input_index_rd());
        while(node_list_non2->size() > 1){
            uniform_int_distribution<int> input_index_dist(1, (int)node_list_non2->size());
            unordered_set<int> index_set;
            for(int i = 0; i < degree; i++){
                int temp_index = input_index_dist(input_index_mt) - 1;
                unordered_set<int>::iterator index_iterator = index_set.find(temp_index);
                while(index_iterator != index_set.end()){
                    if(index_set.size() == node_list_non2->size()){
                        break;
                    }
                    temp_index = input_index_dist(input_index_mt) - 1;
                    index_iterator = index_set.find(temp_index);
                }
                index_set.insert(temp_index);
            }

            in_tree_non2* _temp_tree_node1 = new in_tree_non2();
            backUpNonTreeNode->insert(_temp_tree_node1);
            stringstream ss;
            ss << label_count++;
            _temp_tree_node1->label = "unlabel_" + ss.str();
            priority_queue<int> decrease_index;
            for(unordered_set<int>::iterator index_it = index_set.begin(); index_it != index_set.end(); index_it++){
                int temp_index = *index_it;
                decrease_index.push(temp_index);
                in_tree_non2* _temp_tree_node2 = (*node_list_non2)[temp_index];
                _temp_tree_node1->children.push_back(_temp_tree_node2);
                _temp_tree_node2->parent = _temp_tree_node1;
            }

            while(!decrease_index.empty()){
                int temp_index = decrease_index.top();
                decrease_index.pop();
                vector<in_tree_non2*>::iterator ele = node_list_non2->begin() + temp_index;
                node_list_non2->erase(ele);
            }

            node_list_non2->push_back(_temp_tree_node1);
        }
        return_root = (*node_list_non2)[0];
    }
    return return_root;
}

//compute the necessary triplets randomly
void InputGenerator::additional_triplets(int N, double portion) {
    int tripletAmount = N * (N - 1) * (N - 2) / 6;
    int tripletNeeded = (int)ceil(tripletAmount * portion / 100);
    int subsetIndex = (int)subsetsByindex.size() + 1;

    if(tripletNeeded <= tripletAmount){
        int tripletAdditional = tripletNeeded - (int)subsetsByindex.size();

        random_device addition_device;
        mt19937_64 addition_mt(addition_device());
        int tripletCount = 0;
        while(tripletCount < tripletAdditional){
            uniform_int_distribution<int> addition_dist(0, (int)internal_nodes_vec->size() - 1);
            int ranNumber = addition_dist(addition_mt);
            string _temp_label = (*internal_nodes_vec)[ranNumber];
            if(_temp_label != tree_root->label){
                unordered_map<string, vector<unordered_set<string> *> *>::iterator tripletFinder = potential_tripletbylabel->find(_temp_label);
                if(tripletFinder != potential_tripletbylabel->end()){
                    int ponSize = (int)tripletFinder->second->size();
                    if(ponSize > 0){
                        long long seed2 = chrono::system_clock::now().time_since_epoch().count();
                        shuffle((tripletFinder->second)->begin(), (tripletFinder->second)->end(), default_random_engine((unsigned)seed2));
                        subsetsByindex.insert(unordered_map<int, unordered_set<string> >::value_type(subsetIndex++, *((*(tripletFinder->second))[0])));
                        tripletFinder->second->erase(tripletFinder->second->begin());
                        tripletCount++;
                    }
                    else{
                        internal_nodes_vec->erase(internal_nodes_vec->begin() + ranNumber);
                    }
                }
            }
        }
    }
    else{
        cout << "Error: Can not generate needed triplets!" << endl;
        exit(0);
    }
}

//compute the potential triplets that will be used for testing
void InputGenerator::potential_triplets() {
    for(int i = 0; i < internal_nodes_vec->size() - 1; i++){
        string _temp_label = (*internal_nodes_vec)[i];
        if(_temp_label != tree_root->label){

            //compute the labels which do not belong to this current node's descendant lists
            unordered_set<string> *_ld_set = new unordered_set<string>;
            unordered_set<string> *_rd_set = new unordered_set<string>;
            unordered_set<string> *_rest_label = new unordered_set<string>;

            backUpPtrSet->insert(_ld_set);
            backUpPtrSet->insert(_rd_set);
            backUpPtrSet->insert(_rest_label);

            unordered_map<string, unordered_set<string> *>::iterator set_finder = left_descendant->find(_temp_label);
            if(set_finder != left_descendant->end()){
                _ld_set = set_finder->second;
            }

            set_finder = right_descendant->find(_temp_label);
            if(set_finder != right_descendant->end()){
                _rd_set = set_finder->second;
            }


            for(int a = 0; a < label_list->size(); a++){
                unordered_set<string>::iterator label_finder = _ld_set->find((*label_list)[a]);
                if(label_finder != _ld_set->end()){
                    //find it
                    continue;
                }
                else{
                    label_finder = _rd_set->find((*label_list)[a]);
                    if(label_finder != _rd_set->end()){
                        //find it
                        continue;
                    }
                    else{
                        _rest_label->insert((*label_list)[a]);
                    }
                }
            }

            //compute the triplets at this current node, add those which are not used in the basic triplets set
            //into potential triplets set
            for(unordered_set<string>::iterator lf_label = _ld_set->begin(); lf_label != _ld_set->end(); lf_label++){
                string label1 = *lf_label;
                for(unordered_set<string>::iterator rg_label = _rd_set->begin(); rg_label != _rd_set->end(); rg_label++){
                    string label2 = *rg_label;
                    for(unordered_set<string>::iterator rs_label = _rest_label->begin(); rs_label != _rest_label->end(); rs_label++){
                        string label3 = *rs_label;

                        string strA = label1;
                        string strB = label2;
                        string strC = label3;
                        if(strA > strB){
                            string strTemp = strA;
                            strA = strB;
                            strB = strTemp;
                        }

                        if(strA > strC){
                            string strTemp = strA;
                            strA = strC;
                            strC = strTemp;
                        }

                        if(strB > strC){
                            string strTemp = strB;
                            strB = strC;
                            strC = strTemp;
                        }

                        string potential_triplet_str = strA + strB + strC;
                        unordered_set<string>::iterator inuse_finder = in_use_triplet->find(potential_triplet_str);
                        if(inuse_finder == in_use_triplet->end()){
                            //cannot find it
                            unordered_set<string> *_pon_triplet = new unordered_set<string>;
                            backUpPtrSet->insert(_pon_triplet);
                            _pon_triplet->insert(strA);
                            _pon_triplet->insert(strB);
                            _pon_triplet->insert(strC);


                            unordered_map<string, vector<unordered_set<string> *> *>::iterator pTriple_finder = potential_tripletbylabel->find(_temp_label);
                            if(pTriple_finder != potential_tripletbylabel->end()){
                                //find it
                                pTriple_finder->second->push_back(_pon_triplet);
                            }
                            else{
                                //cannot find it
                                vector<unordered_set<string> *> *pTriple_vec = new vector<unordered_set<string>*>;
                                pTriple_vec->push_back(_pon_triplet);
                                potential_tripletbylabel->insert(unordered_map<string, vector<unordered_set<string> *> *>::value_type(_temp_label, pTriple_vec));
                            }
                        }
                    }
                }
            }
        }
    }
}

//compute the basic triplets that can be used to construct the phylogenetic tree these triples
void InputGenerator::basic_triplets() {
    int subset_index = 1;
    for(int i = 0; i < internal_nodes_vec->size() - 1; i++){
        string _temp_label = (*internal_nodes_vec)[i];
        if(_temp_label != tree_root->label){

            //choose two labels from this current node's left descendants and right descendants
            string strA;
            string strB;

            unordered_map<string, unordered_set<string> *>::iterator set_finder = left_descendant->find(_temp_label);
            if(set_finder != left_descendant->end()){
                unordered_set<string> *_l_set = set_finder->second;
                strA = *(_l_set->begin());
            }

            set_finder = right_descendant->find(_temp_label);
            if(set_finder != right_descendant->end()){
                unordered_set<string> *_r_set = set_finder->second;
                strB = *(_r_set->begin());
            }

            in_tree* _parent_node = new in_tree();
            backUpTreeNode->insert(_parent_node);
            unordered_map<string, in_tree*>::iterator parent_finder = label_parent_map->find(_temp_label);
            if(parent_finder != label_parent_map->end()){
                _parent_node = parent_finder->second;
            }

            string strC;
            if(_temp_label == _parent_node->l_child->label){
                unordered_map<string, unordered_set<string> *>::iterator parent_set_finder = right_descendant->find(_parent_node->label);
                if(parent_set_finder != right_descendant->end()){
                    //find it
                    strC = *(parent_set_finder->second->begin());
                }
            }
            else if(_temp_label == _parent_node->r_child->label){
                unordered_map<string, unordered_set<string> *>::iterator parent_set_finder = left_descendant->find(_parent_node->label);
                if(parent_set_finder != left_descendant->end()){
                    //find it
                    strC = *(parent_set_finder->second->begin());
                }
            }

            unordered_set<string> _temp_triplet_set;
            _temp_triplet_set.insert(strA);
            _temp_triplet_set.insert(strB);
            _temp_triplet_set.insert(strC);

            if(strA > strB){
                string strTemp = strA;
                strA = strB;
                strB = strTemp;
            }

            if(strA > strC){
                string strTemp = strA;
                strA = strC;
                strC = strTemp;
            }

            if(strB > strC){
                string strTemp = strB;
                strB = strC;
                strC = strTemp;
            }

            string inuseStr = strA + strB + strC;
            in_use_triplet->insert(inuseStr);
            subsetsByindex.insert(unordered_map<int, unordered_set<string> >::value_type(subset_index++, _temp_triplet_set));
        }
    }
}

//the method for constructing a bianry tree with specified percentage
in_tree* InputGenerator::initial_tree_construction(double lper, int lindex, int hindex) {
    int array_length = hindex - lindex + 1;

    int mindex = 0;
    if(array_length > 1){
        mindex = lindex + (int)ceil(array_length * lper / 100) - 1;
        in_tree* _lchild = initial_tree_construction(lper, lindex, mindex);
        in_tree* _rchild = initial_tree_construction(lper, mindex + 1, hindex);

        in_tree* _pnode = new in_tree();
        backUpTreeNode->insert(_pnode);
        _pnode->selected_child = false;
        stringstream ss;
        ss << label_count++;
        _pnode->label = "unlabel_" + ss.str();

        _pnode->l_child = _lchild;
        _pnode->r_child = _rchild;
        _lchild->parent = _pnode;
        _rchild->parent = _pnode;
        internal_nodes_vec->push_back(_pnode->label);
        label_parent_map->insert(unordered_map<string, in_tree*>::value_type(_lchild->label, _pnode));
        label_parent_map->insert(unordered_map<string, in_tree*>::value_type(_rchild->label, _pnode));

        if(_lchild->l_child == NULL && _lchild->r_child == NULL){
            unordered_set<string> *_temp_set = new unordered_set<string>;
            backUpPtrSet->insert(_temp_set);
            _temp_set->insert(_lchild->label);
            left_descendant->insert(unordered_map<string, unordered_set<string> *>::value_type(_pnode->label, _temp_set));
        }
        else if(_lchild->l_child != NULL && _lchild->r_child != NULL){
            unordered_set<string> *_temp_set = new unordered_set<string>;
            unordered_set<string> *_ld_set = new unordered_set<string>;
            unordered_set<string> *_rd_set = new unordered_set<string>;
            backUpPtrSet->insert(_temp_set);
            backUpPtrSet->insert(_ld_set);
            backUpPtrSet->insert(_rd_set);
            unordered_map<string, unordered_set<string>* >::iterator set_finder = left_descendant->find(_lchild->label);
            if(set_finder != left_descendant->end()){
                //find it
                _ld_set = set_finder->second;
            }

            set_finder = right_descendant->find(_lchild->label);
            if(set_finder != right_descendant->end()){
                //find it
                _rd_set = set_finder->second;
            }


            for(unordered_set<string>::iterator set_it = _ld_set->begin(); set_it != _ld_set->end(); set_it++){
                _temp_set->insert(*set_it);
            }

            for(unordered_set<string>::iterator set_it = _rd_set->begin(); set_it != _rd_set->end(); set_it++){
                _temp_set->insert(*set_it);
            }

            left_descendant->insert(unordered_map<string, unordered_set<string>* >::value_type(_pnode->label, _temp_set));
        }

        if(_rchild->l_child == NULL && _rchild->r_child == NULL){
            unordered_set<string> *_temp_set = new unordered_set<string>;
            backUpPtrSet->insert(_temp_set);
            _temp_set->insert(_rchild->label);
            right_descendant->insert(unordered_map<string, unordered_set<string> *>::value_type(_pnode->label, _temp_set));
        }
        else if(_rchild->l_child != NULL && _rchild->r_child != NULL){
            unordered_set<string> *_temp_set = new unordered_set<string>;
            unordered_set<string> *_ld_set = new unordered_set<string>;
            unordered_set<string> *_rd_set = new unordered_set<string>;
            backUpPtrSet->insert(_temp_set);
            backUpPtrSet->insert(_ld_set);
            backUpPtrSet->insert(_rd_set);
            unordered_map<string, unordered_set<string>* >::iterator set_finder = left_descendant->find(_rchild->label);
            if(set_finder != left_descendant->end()){
                //find it
                _ld_set = set_finder->second;
            }

            set_finder = right_descendant->find(_rchild->label);
            if(set_finder != right_descendant->end()){
                //find it
                _rd_set = set_finder->second;
            }


            for(unordered_set<string>::iterator set_it = _ld_set->begin(); set_it != _ld_set->end(); set_it++){
                _temp_set->insert(*set_it);
            }

            for(unordered_set<string>::iterator set_it = _rd_set->begin(); set_it != _rd_set->end(); set_it++){
                _temp_set->insert(*set_it);
            }

            right_descendant->insert(unordered_map<string, unordered_set<string>* >::value_type(_pnode->label, _temp_set));
        }

        return _pnode;
    }
    else{
        return (*node_list)[lindex];
    }
}

//main method for constructing a binary tree from top to bottom
//this tree is constructed with specified percentage of left children and right children
//at the same time, build up label array and all triples
void InputGenerator::initial_tree4triplet_test(int N, double portion, double lper) {
    if(N >= 3){
        //construct the leaf vector
        for(int i = 0; i < N; i++){
            stringstream ss;
            ss << label_count++;
            in_tree* _temp_tree_node = new in_tree();
            backUpTreeNode->insert(_temp_tree_node);
            string _temp_tree_node_value = "label_" + ss.str();
            _temp_tree_node->label = _temp_tree_node_value;
            _temp_tree_node->selected_child = false;
            node_list->push_back(_temp_tree_node);
            label_list->push_back(_temp_tree_node_value);
        }

        //wlog, make r_per the smaller percentage
        double l_per = 0;
        if(lper < 100 - lper){
            l_per = lper;
        }
        else{
            l_per = 100 - lper;
        }

        //randomly rotate the node_list
        long long seed = chrono::system_clock::now().time_since_epoch().count();
        shuffle(node_list->begin(), node_list->end(), default_random_engine((unsigned)seed));
        left_descendant = new unordered_map<string, unordered_set<string>* >;
        right_descendant = new unordered_map<string, unordered_set<string>* >;
        tree_root = initial_tree_construction(l_per, 0, (int)node_list->size() - 1);

        //compute basic triplets
        basic_triplets();

        //compute all other potential triplets
        potential_triplets();


        //compute all necessary additional triplets randomly
        additional_triplets(N, portion);
    }
}

//main method for constructing a binary tree from bottom to up
//meanwhile, build up label array and all subsets with size of 3
void InputGenerator::triple_builder_leaf_label_only(int N) {
    if(N >= 3){
        //construct the leaf vector
        for(int i = 0; i < N; i++){
            stringstream ss;
            ss << label_count++;
            in_tree* _temp_tree_node = new in_tree();
            backUpTreeNode->insert(_temp_tree_node);
            string _temp_tree_node_value = "label_" + ss.str();
            _temp_tree_node->label = _temp_tree_node_value;
            _temp_tree_node->selected_child = false;
            node_list->push_back(_temp_tree_node);
            label_list->push_back(_temp_tree_node_value);
        }

        //construct the subsets
        triple_subset();

        //random approach
//        random_device input_index_rd;
//        mt19937_64 input_index_mt(input_index_rd());
//        while(node_list->size() > 1){
//            uniform_int_distribution<int> input_index_dist(1, (int)node_list->size());
//            int index_1 = input_index_dist(input_index_mt) - 1;
//            int index_2 = input_index_dist(input_index_mt) - 1;
//            if(index_1 == index_2){
//                if(index_2 < node_list->size() - 1){
//                    index_2++;
//                }
//                else{
//                    index_2--;
//                }
//            }
//            in_tree* _temp_tree_node1 = (*node_list)[index_1];
//            in_tree* _temp_tree_node2 = (*node_list)[index_2];
//            in_tree* _temp_tree_node3 = new in_tree();
//            _temp_tree_node3->selected_child = false;
//            stringstream ss;
//            ss << label_count++;
//            _temp_tree_node3->label = "unlabel_" + ss.str();
//
//            _temp_tree_node3->l_child = _temp_tree_node1;
//            _temp_tree_node3->r_child = _temp_tree_node2;
//            _temp_tree_node1->parent = _temp_tree_node3;
//            _temp_tree_node2->parent = _temp_tree_node3;
//
//            if(index_1 < index_2){
//                vector<in_tree*>::iterator ele = node_list->begin() + index_2;
//                node_list->erase(ele);
//                ele = node_list->begin() + index_1;
//                node_list->erase(ele);
//            }
//            else{
//                vector<in_tree*>::iterator ele = node_list->begin() + index_1;
//                node_list->erase(ele);
//                ele = node_list->begin() + index_2;
//                node_list->erase(ele);
//            }
//            node_list->push_back(_temp_tree_node3);
//        }
//        tree_root = (*node_list)[0];

        //caterpiller approach
//        long long seed = chrono::system_clock::now().time_since_epoch().count();
//        shuffle(node_list->begin(), node_list->end(), default_random_engine((unsigned)seed));
//        while(node_list->size() > 1){
//            int index_1 = (int)node_list->size() - 1;
//            int index_2 = (int)node_list->size() - 2;
//            in_tree* _temp_tree_node1 = (*node_list)[index_1];
//            in_tree* _temp_tree_node2 = (*node_list)[index_2];
//            in_tree* _temp_tree_node3 = new in_tree();
//            _temp_tree_node3->selected_child = false;
//            stringstream ss;
//            ss << label_count++;
//            _temp_tree_node3->label = "unlabel_" + ss.str();
//
//            _temp_tree_node3->l_child = _temp_tree_node1;
//            _temp_tree_node3->r_child = _temp_tree_node2;
//            _temp_tree_node1->parent = _temp_tree_node3;
//            _temp_tree_node2->parent = _temp_tree_node3;
//
//            vector<in_tree*>::iterator ele = node_list->begin() + index_1;
//            node_list->erase(ele);
//            ele = node_list->begin() + index_2;
//            node_list->erase(ele);
//
//            node_list->push_back(_temp_tree_node3);
//        }
//        tree_root = (*node_list)[0];

        //another random approach
        long long seed = chrono::system_clock::now().time_since_epoch().count();
        while(node_list->size() > 1){
            shuffle(node_list->begin(), node_list->end(), default_random_engine((unsigned)seed));
            int index_1 = (int)node_list->size() - 1;
            int index_2 = (int)node_list->size() - 2;
            in_tree* _temp_tree_node1 = (*node_list)[index_1];
            in_tree* _temp_tree_node2 = (*node_list)[index_2];
            in_tree* _temp_tree_node3 = new in_tree();
            backUpTreeNode->insert(_temp_tree_node3);
            _temp_tree_node3->selected_child = false;
            stringstream ss;
            ss << label_count++;
            _temp_tree_node3->label = "unlabel_" + ss.str();

            _temp_tree_node3->l_child = _temp_tree_node1;
            _temp_tree_node3->r_child = _temp_tree_node2;
            _temp_tree_node1->parent = _temp_tree_node3;
            _temp_tree_node2->parent = _temp_tree_node3;

            vector<in_tree*>::iterator ele = node_list->begin() + index_1;
            node_list->erase(ele);
            ele = node_list->begin() + index_2;
            node_list->erase(ele);

            node_list->push_back(_temp_tree_node3);
        }
        tree_root = (*node_list)[0];
    }
    else{
        cout << "Size of labels is too small!" << endl;
    }
}

//main method for constructing a binary tree from buttom to up
//this method is going to construct a binary tree with N leaves
//based on the properties of a binary tree, if a binary tree contains N leaves, then
//it will have N - 1 non-leaf nodes.
void InputGenerator::binary_tree_builder_leaf_label_only(int N, int subset_number) {
    if(N == 1){
        in_tree* _temp_tree_node = new in_tree();
        backUpTreeNode->insert(_temp_tree_node);
        _temp_tree_node->label = "label_1";
        _temp_tree_node->selected_child = false;

        label_list->push_back("label_1");
        for(int i = 0; i < subset_number; i++){
            unordered_map<int, unordered_set<string> >::iterator subsetsByindex_finder = subsetsByindex.find(i + 1);
            if(subsetsByindex_finder != subsetsByindex.end()){
                //find it
                subsetsByindex_finder->second.insert("label_1");
            }
            else{
                unordered_set<string> temp_set;
                temp_set.insert("label_1");
                subsetsByindex.insert(unordered_map<int, unordered_set<string> >::value_type(i + 1, temp_set));
            }
        }
        tree_root = _temp_tree_node;
    }
    else if(N > 1){
        //construct the leaf vector
        for(int i = 0; i < N; i++){
            stringstream ss;
            ss << label_count++;
            in_tree* _temp_tree_node = new in_tree();
            backUpTreeNode->insert(_temp_tree_node);
            string _temp_tree_node_value = "label_" + ss.str();
            _temp_tree_node->label = _temp_tree_node_value;
            _temp_tree_node->selected_child = false;
            node_list->push_back(_temp_tree_node);
            label_list->push_back(_temp_tree_node_value);
            insert_to_subset(subset_number, _temp_tree_node_value);
        }

        random_device input_index_rd;
        mt19937_64 input_index_mt(input_index_rd());
        while(node_list->size() > 1){
            uniform_int_distribution<int> input_index_dist(1, (int)node_list->size());
            int index_1 = input_index_dist(input_index_mt) - 1;
            int index_2 = input_index_dist(input_index_mt) - 1;
            if(index_1 == index_2){
                if(index_2 < node_list->size() - 1){
                    index_2++;
                }
                else{
                    index_2--;
                }
            }
            in_tree* _temp_tree_node1 = (*node_list)[index_1];
            in_tree* _temp_tree_node2 = (*node_list)[index_2];
            in_tree* _temp_tree_node3 = new in_tree();
            backUpTreeNode->insert(_temp_tree_node3);
            _temp_tree_node3->selected_child = false;
            stringstream ss;
            ss << label_count++;
            _temp_tree_node3->label = "unlabel_" + ss.str();

            _temp_tree_node3->l_child = _temp_tree_node1;
            _temp_tree_node3->r_child = _temp_tree_node2;
            _temp_tree_node1->parent = _temp_tree_node3;
            _temp_tree_node2->parent = _temp_tree_node3;

            if(index_1 < index_2){
                vector<in_tree*>::iterator ele = node_list->begin() + index_2;
                node_list->erase(ele);
                ele = node_list->begin() + index_1;
                node_list->erase(ele);
            }
            else{
                vector<in_tree*>::iterator ele = node_list->begin() + index_1;
                node_list->erase(ele);
                ele = node_list->begin() + index_2;
                node_list->erase(ele);
            }
            node_list->push_back(_temp_tree_node3);
        }
        tree_root = (*node_list)[0];
    }
}

//subtract the input trees from the original binary tree
void InputGenerator::subtract_tree_non2(unordered_set<string> subset, in_tree_non2* _tree_root, int degree) {
    if(_tree_root != NULL){
        while(_vec_stack_non2._stack_top >= 0){
            in_tree_non2* _temp_node = (*_vec_stack_non2._vector_stack)[_vec_stack_non2._stack_top--];
            if(_temp_node != _tree_root){
                //if the current node is not the root of the tree
                if(_temp_node->children.empty()){
                    //if this node is the leaf
                    //check if this node is in the current subset
                    unordered_set<string>::iterator _subset_element_finder = subset.find(_temp_node->label);
                    if(_subset_element_finder != subset.end()){
                        //indicate that this node is selected as one of the element in the current subset
                        _temp_node->substract_parent = _temp_node->parent;
                        for(int i = 0 ; i < _temp_node->parent->children.size(); i++){
                            if(_temp_node->parent->children[i] == _temp_node){
                                _temp_node->parent->substract_children.push_back(_temp_node);
                            }
                        }
                    }
                }
                else{
                    //if this node is not the leaf
                    //check if this node is in the current subset
                    unordered_set<string>::iterator _subset_element_finder = subset.find(_temp_node->label);
                    if(_subset_element_finder != subset.end()){
                        //indicate that this node is selected as one of the element in the current subset
                        _temp_node->substract_parent = _temp_node->parent;
                        for(int i = 0 ; i < _temp_node->parent->children.size(); i++){
                            if(_temp_node->parent->children[i] == _temp_node){
                                _temp_node->parent->substract_children.push_back(_temp_node);
                            }
                        }
                    }
                    else{
                        //indicate that this node is not selected as one of the element in the current subset
                        if(!_temp_node->substract_children.empty()){
                            if(_temp_node->substract_children.size() == 1){
                                for(int i = 0 ; i < _temp_node->substract_children.size(); i++){
                                    _temp_node->substract_children[i]->substract_parent = _temp_node->parent;
                                    _temp_node->parent->substract_children.push_back(_temp_node->substract_children[i]);
                                }
                            }
                            else{
                                in_tree_non2* _new_node = new in_tree_non2();
                                backUpNonTreeNode->insert(_new_node);
                                stringstream ss;
                                ss << final_label++;
                                _new_node->label = "f_" + ss.str();
                                for(int i = 0; i < _temp_node->substract_children.size(); i++){
                                    _new_node->substract_children.push_back(_temp_node->substract_children[i]);
                                    _temp_node->substract_children[i]->substract_parent = _new_node;
                                }
                                _new_node->substract_parent = _temp_node->parent;
                                _temp_node->parent->substract_children.push_back(_new_node);
                            }
                        }
                    }
                }
            }
            else{
                unordered_set<string>::iterator _subset_element_finder = subset.find(_temp_node->label);
                if(_subset_element_finder != subset.end()){
                    //if the root is in the subset
                    sub_tree_root_non2 = _temp_node;
                }
                else{
                    //if the root is not in the subset
                    if(!_temp_node->substract_children.empty()){
                        if(_temp_node->substract_children.size() == 1){
                            sub_tree_root_non2 = _temp_node->substract_children[0];
                            sub_tree_root_non2->substract_parent = NULL;
                        }
                        else{
                            in_tree_non2* _new_node = new in_tree_non2();
                            backUpNonTreeNode->insert(_new_node);
                            stringstream ss;
                            ss << final_label++;
                            _new_node->label = "f_" + ss.str();
                            _new_node->substract_parent = NULL;
                            for(int i = 0 ; i < _temp_node->substract_children.size(); i++){
                                _new_node->substract_children.push_back(_temp_node->substract_children[i]);
                                _temp_node->substract_children[i]->substract_parent = _new_node;
                            }
                            sub_tree_root_non2 = _new_node;
                        }
                    }
                    else{
                        in_tree_non2* _new_node = new in_tree_non2();
                        backUpNonTreeNode->insert(_new_node);
                        _new_node->label = _temp_node->label;
                        sub_tree_root_non2 = _new_node;
                    }
                }
            }
        }
    }
}

//given a set of subsets, subtract the input trees from the original binary tree
void InputGenerator::subtract_tree(unordered_set<string> subset) {
    if(tree_root != NULL){
        while(_vec_stack._stack_top >= 0){
            in_tree* _temp_node = (*_vec_stack._vector_stack)[_vec_stack._stack_top--];
            if(_temp_node != tree_root){
                if(_temp_node->l_child == NULL && _temp_node->r_child == NULL){
                    //if this node is the leaf
                    //check if this node is in the current subset
                    unordered_set<string>::iterator _subset_element_finder = subset.find(_temp_node->label);
                    if(_subset_element_finder != subset.end()){
                        //indicate that this node is selected as one of the element in the current subset
                        _temp_node->substract_parent = _temp_node->parent;
                        if(_temp_node->parent->l_child == _temp_node){
                            _temp_node->parent->substract_l_child = _temp_node;
                        }
                        else if(_temp_node->parent->r_child == _temp_node){
                            _temp_node->parent->substract_r_child = _temp_node;
                        }
                    }
                    else{
                        //indicate that this node is not selected as one of the element in the current subset
                        if(_temp_node->parent->l_child == _temp_node){
                            _temp_node->parent->substract_l_child = NULL;
                        }
                        else if(_temp_node->parent->r_child == _temp_node){
                            _temp_node->parent->substract_r_child = NULL;
                        }
                    }
                }
                else{
                    //if this node is not the leaf
                    //check if this node is in the current subset
                    unordered_set<string>::iterator _subset_element_finder = subset.find(_temp_node->label);
                    if(_subset_element_finder != subset.end()){
                        //indicate that this node is selected as one of the element in the current subset
                        _temp_node->substract_parent = _temp_node->parent;
                        if(_temp_node->parent->l_child == _temp_node){
                            _temp_node->parent->substract_l_child = _temp_node;
                        }
                        else if(_temp_node->parent->r_child == _temp_node){
                            _temp_node->parent->substract_r_child = _temp_node;
                        }
                    }
                    else{
                        //indicate that this node is not selected as one of the element in the current subset
                        if(_temp_node->substract_l_child == NULL && _temp_node->substract_r_child != NULL){
                            _temp_node->substract_r_child->substract_parent = _temp_node->parent;
                            if(_temp_node->parent->l_child == _temp_node){
                                _temp_node->parent->substract_l_child = _temp_node->substract_r_child;
                            }
                            else if(_temp_node->parent->r_child == _temp_node){
                                _temp_node->parent->substract_r_child = _temp_node->substract_r_child;
                            }

                        }
                        else if(_temp_node->substract_l_child != NULL && _temp_node->substract_r_child == NULL){
                            _temp_node->substract_l_child->substract_parent = _temp_node->parent;
                            if(_temp_node->parent->l_child == _temp_node){
                                _temp_node->parent->substract_l_child = _temp_node->substract_l_child;
                            }
                            else if(_temp_node->parent->r_child == _temp_node){
                                _temp_node->parent->substract_r_child = _temp_node->substract_l_child;
                            }
                        }
                        else if(_temp_node->substract_l_child != NULL && _temp_node->substract_r_child != NULL){
                            in_tree* _new_node = new in_tree();
                            backUpTreeNode->insert(_new_node);
                            stringstream ss;
                            ss << final_label++;
                            _new_node->label = "f_" + ss.str();
                            _new_node->substract_l_child = _temp_node->substract_l_child;
                            _temp_node->substract_l_child->substract_parent = _new_node;
                            _new_node->substract_r_child = _temp_node->substract_r_child;
                            _temp_node->substract_r_child->substract_parent = _new_node;
                            _new_node->substract_parent = _temp_node->parent;
                            if(_temp_node->parent->l_child == _temp_node){
                                _temp_node->parent->substract_l_child = _new_node;
                            }
                            else if(_temp_node->parent->r_child == _temp_node){
                                _temp_node->parent->substract_r_child = _new_node;
                            }
                        }
                        else{
                            if(_temp_node->parent->l_child == _temp_node){
                                _temp_node->parent->substract_l_child = NULL;
                            }
                            else if(_temp_node->parent->r_child == _temp_node){
                                _temp_node->parent->substract_r_child = NULL;
                            }
                        }
                    }
                }
            }
            else{
                unordered_set<string>::iterator _subset_element_finder = subset.find(_temp_node->label);
                if(_subset_element_finder != subset.end()){
                    //if the root is in the subset
                    sub_tree_root = _temp_node;
                }
                else{
                    //if the root is not in the subset
                    if(_temp_node->substract_l_child != NULL && _temp_node->substract_r_child != NULL){
                        in_tree* _new_node = new in_tree();
                        backUpTreeNode->insert(_new_node);
                        stringstream ss;
                        ss << final_label++;
                        _new_node->label = "f_" + ss.str();
                        _new_node->substract_parent = NULL;
                        _new_node->substract_l_child = _temp_node->substract_l_child;
                        _temp_node->substract_l_child->substract_parent = _new_node;
                        _new_node->substract_r_child = _temp_node->substract_r_child;
                        _temp_node->substract_r_child->substract_parent = _new_node;
                        sub_tree_root = _new_node;
                    }
                    else if(_temp_node->substract_l_child != NULL && _temp_node->substract_r_child == NULL){
                        sub_tree_root = _temp_node->substract_l_child;
                        sub_tree_root->substract_parent = NULL;
                    }
                    else if(_temp_node->substract_l_child == NULL && _temp_node->substract_r_child != NULL){
                        sub_tree_root = _temp_node->substract_r_child;
                        sub_tree_root->substract_parent = NULL;
                    }
                    else{
                        in_tree* _new_node = new in_tree();
                        _new_node->label = _temp_node->label;
                        sub_tree_root = _new_node;
                    }
                }
            }
        }
    }
}

//this method is designed for converting a non-binary subtree from tree-like structure to linked-list like structure
void InputGenerator::convert_non2_subtree_to_linked_list(unordered_map<string, linked_list_entry> &l_list,
                                                         int tree_num, in_tree_non2* subtree_root) {
    queue<in_tree_non2*> *_tree_node_queue = new queue<in_tree_non2*>;
    _tree_node_queue->push(subtree_root);
    while(!_tree_node_queue->empty()){
        in_tree_non2* _temp_node = _tree_node_queue->front();
        _tree_node_queue->pop();

        for(int i = 0; i < _temp_node->substract_children.size(); i++){
            if(_temp_node->substract_children[i] != NULL){
                _tree_node_queue->push(_temp_node->substract_children[i]);
                unordered_map<string, linked_list_entry>::iterator _linked_list_entry = l_list.find(_temp_node->label);
                if(_linked_list_entry != l_list.end()){
                    linked_list_entry *n = new linked_list_entry(_temp_node->substract_children[i]->label, WHITE, tree_num);
                    backUpEntrySet->insert(n);
                    _linked_list_entry->second.ajcn_list.insert(unordered_map<string, linked_list_entry>::value_type(n->value, *n));
                }
                else{
                    linked_list_entry *n1 = new linked_list_entry(_temp_node->label, WHITE, tree_num);
                    linked_list_entry *n2 = new linked_list_entry(_temp_node->substract_children[i]->label, WHITE, tree_num);
                    backUpEntrySet->insert(n1);
                    backUpEntrySet->insert(n2);
                    n1->ajcn_list.insert(unordered_map<string, linked_list_entry>::value_type(n2->value, *n2));
                    l_list.insert(map<string, linked_list_entry>::value_type(n1->value, *n1));
                }

                _linked_list_entry = l_list.find(_temp_node->substract_children[i]->label);
                if(_linked_list_entry != l_list.end()){
                    linked_list_entry *n = new linked_list_entry(_temp_node->label, WHITE, tree_num);
                    backUpEntrySet->insert(n);
                    _linked_list_entry->second.ajcn_list.insert(unordered_map<string, linked_list_entry>::value_type(n->value, *n));
                }
                else{
                    linked_list_entry *n1 = new linked_list_entry(_temp_node->substract_children[i]->label, WHITE, tree_num);
                    linked_list_entry *n2 = new linked_list_entry(_temp_node->label, WHITE, tree_num);
                    backUpEntrySet->insert(n1);
                    backUpEntrySet->insert(n2);
                    n1->ajcn_list.insert(unordered_map<string, linked_list_entry>::value_type(n2->value, *n2));
                    l_list.insert(map<string, linked_list_entry>::value_type(n1->value, *n1));
                }

                //incrementally insert this pair of edge into HPU
                _linked_list_entry = global_hpu.find(_temp_node->label);
                unordered_map<string, linked_list_entry>::iterator _linked_list_entry2 = global_hpu.find(_temp_node->substract_children[i]->label);
                if(_linked_list_entry != global_hpu.end() && _linked_list_entry2 != global_hpu.end()){
                    //indicate that they have already been in the HPU
                    //need to check if there is an edge between them
                    //if there is, then check if belongs set is correct
                    //if not, then build up the edge, and update corresponding info correctly
                    unordered_map<string, linked_list_entry>::iterator _node_exist_entry = _linked_list_entry->second.ajcn_list.find(_temp_node->substract_children[i]->label);
                    if(_node_exist_entry == _linked_list_entry->second.ajcn_list.end()){
                        //indicate that there is no edge between them from the parent node's side of view
                        linked_list_entry *n = new linked_list_entry(_temp_node->substract_children[i]->label, WHITE, tree_num);
                        backUpEntrySet->insert(n);
                        _linked_list_entry->second.ajcn_list.insert(unordered_map<string, linked_list_entry>::value_type(n->value, *n));
                    }
                    //then check if belongs set contains tree_num
                    unordered_set<int>::iterator belong_set_entry = _linked_list_entry->second.belongs.find(tree_num);
                    if(belong_set_entry == _linked_list_entry->second.belongs.end()){
                        _linked_list_entry->second.belongs.insert(tree_num);
                        _linked_list_entry->second.kl += 1;
                    }

                    _node_exist_entry = _linked_list_entry2->second.ajcn_list.find(_temp_node->label);
                    if(_node_exist_entry == _linked_list_entry2->second.ajcn_list.end()){
                        //indicate that there is no edge between them from the child node's side of view
                        linked_list_entry *n = new linked_list_entry(_temp_node->label, WHITE, tree_num);
                        backUpEntrySet->insert(n);
                        _linked_list_entry2->second.ajcn_list.insert(unordered_map<string, linked_list_entry>::value_type(n->value, *n));
                    }
                    //then check if belongs set contains tree_num
                    belong_set_entry = _linked_list_entry2->second.belongs.find(tree_num);
                    if(belong_set_entry == _linked_list_entry2->second.belongs.end()){
                        _linked_list_entry2->second.belongs.insert(tree_num);
                        _linked_list_entry2->second.kl += 1;
                    }
                }
                else if(_linked_list_entry == global_hpu.end() && _linked_list_entry2 == global_hpu.end()){
                    //indicate that they are not in the HPU
                    linked_list_entry *n1 = new linked_list_entry(_temp_node->label, WHITE, tree_num);
                    linked_list_entry *n2 = new linked_list_entry(_temp_node->label, WHITE, tree_num);
                    linked_list_entry *n3 = new linked_list_entry(_temp_node->substract_children[i]->label, WHITE, tree_num);
                    linked_list_entry *n4 = new linked_list_entry(_temp_node->substract_children[i]->label, WHITE, tree_num);
                    backUpEntrySet->insert(n1);
                    backUpEntrySet->insert(n2);
                    backUpEntrySet->insert(n3);
                    backUpEntrySet->insert(n4);
                    n1->ajcn_list.insert(unordered_map<string, linked_list_entry>::value_type(n4->value, *n4));
                    n3->ajcn_list.insert(unordered_map<string, linked_list_entry>::value_type(n2->value, *n2));
                    global_hpu.insert(unordered_map<string, linked_list_entry>::value_type(n1->value, *n1));
                    global_hpu.insert(unordered_map<string, linked_list_entry>::value_type(n3->value, *n3));
                }
                else if(_linked_list_entry != global_hpu.end() && _linked_list_entry2 == global_hpu.end()){
                    //indicate that the parent node is in HPU
                    linked_list_entry *n1 = new linked_list_entry(_temp_node->substract_children[i]->label, WHITE, tree_num);
                    backUpEntrySet->insert(n1);
                    _linked_list_entry->second.ajcn_list.insert(unordered_map<string, linked_list_entry>::value_type(n1->value, *n1));
                    unordered_set<int>::iterator belong_set_entry = _linked_list_entry->second.belongs.find(tree_num);
                    if(belong_set_entry == _linked_list_entry->second.belongs.end()){
                        //indicate the current parent node is from another tree, therefore, belongs set needs to be updated
                        //kl of this parent node needs to be updated
                        _linked_list_entry->second.belongs.insert(tree_num);
                        _linked_list_entry->second.kl += 1;
                    }
                    linked_list_entry *n2 = new linked_list_entry(_temp_node->substract_children[i]->label, WHITE, tree_num);
                    linked_list_entry *n3 = new linked_list_entry(_temp_node->label, WHITE, tree_num);
                    backUpEntrySet->insert(n2);
                    backUpEntrySet->insert(n3);
                    n2->ajcn_list.insert(unordered_map<string, linked_list_entry>::value_type(n3->value, *n3));
                    global_hpu.insert(unordered_map<string, linked_list_entry>::value_type(n2->value, *n2));
                }
                else if(_linked_list_entry == global_hpu.end() && _linked_list_entry2 != global_hpu.end()){
                    //indicate that the child node is in HPU
                    linked_list_entry *n1 = new linked_list_entry(_temp_node->label, WHITE, tree_num);
                    backUpEntrySet->insert(n1);
                    _linked_list_entry2->second.ajcn_list.insert(unordered_map<string, linked_list_entry>::value_type(n1->value, *n1));
                    unordered_set<int>::iterator belong_set_entry = _linked_list_entry2->second.belongs.find(tree_num);
                    if(belong_set_entry == _linked_list_entry2->second.belongs.end()){
                        _linked_list_entry2->second.belongs.insert(tree_num);
                        _linked_list_entry2->second.kl += 1;
                    }
                    linked_list_entry *n2 = new linked_list_entry(_temp_node->label, WHITE, tree_num);
                    linked_list_entry *n3 = new linked_list_entry(_temp_node->substract_children[i]->label, WHITE, tree_num);
                    backUpEntrySet->insert(n2);
                    backUpEntrySet->insert(n3);
                    n2->ajcn_list.insert(unordered_map<string, linked_list_entry>::value_type(n3->value, *n3));
                    global_hpu.insert(unordered_map<string, linked_list_entry>::value_type(n2->value, *n2));
                }

                graph_Edge ge;
                ge.endp1 = _temp_node->label;
                ge.endp2 = _temp_node->substract_children[i]->label;
                unordered_map<int, unordered_set<graph_Edge, hash_Edge> >::iterator _parent_child_relation_entry = father_child.find(tree_num);
                if(_parent_child_relation_entry != father_child.end()){
                    _parent_child_relation_entry->second.insert(ge);
                }
                else{
                    unordered_set<graph_Edge, hash_Edge> tempset;
                    tempset.insert(ge);
                    father_child.insert(unordered_map<int, unordered_set<graph_Edge, hash_Edge> >::value_type(tree_num, tempset));
                }
            }
        }
    }
    delete _tree_node_queue;
}

//this method is designed for converting a subtree from tree-like structure to linked list like structure
void InputGenerator::convert_subtree_to_linked_list(unordered_map<string, linked_list_entry> &l_list, int tree_num) {
    queue<in_tree*> *_tree_node_queue = new queue<in_tree*>;
    _tree_node_queue->push(sub_tree_root);
    while(!(_tree_node_queue->empty())){
        in_tree* _temp_node = _tree_node_queue->front();
        _tree_node_queue->pop();

        if(_temp_node->substract_l_child != NULL){
            _tree_node_queue->push(_temp_node->substract_l_child);
            unordered_map<string, linked_list_entry>::iterator _linked_list_entry = l_list.find(_temp_node->label);
            if(_linked_list_entry != l_list.end()){
                linked_list_entry *n = new linked_list_entry(_temp_node->substract_l_child->label, WHITE, tree_num);
                backUpEntrySet->insert(n);
                _linked_list_entry->second.ajcn_list.insert(unordered_map<string, linked_list_entry>::value_type(n->value, (*n)));
            }
            else{
                linked_list_entry *n1 = new linked_list_entry(_temp_node->label, WHITE, tree_num);
                linked_list_entry *n2 = new linked_list_entry(_temp_node->substract_l_child->label, WHITE, tree_num);
                backUpEntrySet->insert(n1);
                backUpEntrySet->insert(n2);
                n1->ajcn_list.insert(unordered_map<string, linked_list_entry>::value_type(n2->value, *n2));
                l_list.insert(map<string, linked_list_entry>::value_type(n1->value, *n1));
            }
            _linked_list_entry = l_list.find(_temp_node->substract_l_child->label);
            if(_linked_list_entry != l_list.end()){
                linked_list_entry *n = new linked_list_entry(_temp_node->label, WHITE, tree_num);
                backUpEntrySet->insert(n);
                _linked_list_entry->second.ajcn_list.insert(unordered_map<string, linked_list_entry>::value_type(n->value, (*n)));
            }
            else{
                linked_list_entry *n1 = new linked_list_entry(_temp_node->substract_l_child->label, WHITE, tree_num);
                linked_list_entry *n2 = new linked_list_entry(_temp_node->label, WHITE, tree_num);
                backUpEntrySet->insert(n1);
                backUpEntrySet->insert(n2);
                n1->ajcn_list.insert(unordered_map<string, linked_list_entry>::value_type(n2->value, *n2));
                l_list.insert(map<string, linked_list_entry>::value_type(n1->value, *n1));
            }

            //incrementally insert this pair of edge into HPU
            _linked_list_entry = global_hpu.find(_temp_node->label);
            unordered_map<string, linked_list_entry>::iterator _linked_list_entry2 = global_hpu.find(_temp_node->substract_l_child->label);
            if(_linked_list_entry != global_hpu.end() && _linked_list_entry2 != global_hpu.end()){
                //indicate that they have already been in the HPU
                //need to check if there is an edge between them
                //if there is, then check if belongs set is correct
                //if not, then build up the edge, and update corresponding info correctly
                unordered_map<string, linked_list_entry>::iterator _node_exist_entry = _linked_list_entry->second.ajcn_list.find(_temp_node->substract_l_child->label);
                if(_node_exist_entry == _linked_list_entry->second.ajcn_list.end()){
                    //indicate that there is no edge between them from the parent node's side of view
                    linked_list_entry *n = new linked_list_entry(_temp_node->substract_l_child->label, WHITE, tree_num);
                    backUpEntrySet->insert(n);
                    _linked_list_entry->second.ajcn_list.insert(unordered_map<string, linked_list_entry>::value_type(n->value, *n));
                }
                //then check if belongs set contains tree_num
                unordered_set<int>::iterator belong_set_entry = _linked_list_entry->second.belongs.find(tree_num);
                if(belong_set_entry == _linked_list_entry->second.belongs.end()){
                    _linked_list_entry->second.belongs.insert(tree_num);
                    _linked_list_entry->second.kl += 1;
                }

                _node_exist_entry = _linked_list_entry2->second.ajcn_list.find(_temp_node->label);
                if(_node_exist_entry == _linked_list_entry2->second.ajcn_list.end()){
                    //indicate that there is no edge between them from the child node's side of view
                    linked_list_entry *n = new linked_list_entry(_temp_node->label, WHITE, tree_num);
                    backUpEntrySet->insert(n);
                    _linked_list_entry2->second.ajcn_list.insert(unordered_map<string, linked_list_entry>::value_type(n->value, *n));
                }
                //then check if belongs set contains tree_num
                belong_set_entry = _linked_list_entry2->second.belongs.find(tree_num);
                if(belong_set_entry == _linked_list_entry2->second.belongs.end()){
                    _linked_list_entry2->second.belongs.insert(tree_num);
                    _linked_list_entry2->second.kl += 1;
                }
            }
            else if(_linked_list_entry == global_hpu.end() && _linked_list_entry2 == global_hpu.end()){
                //indicate that they are not in the HPU
                linked_list_entry *n1 = new linked_list_entry(_temp_node->label, WHITE, tree_num);
                linked_list_entry *n2 = new linked_list_entry(_temp_node->label, WHITE, tree_num);
                linked_list_entry *n3 = new linked_list_entry(_temp_node->substract_l_child->label, WHITE, tree_num);
                linked_list_entry *n4 = new linked_list_entry(_temp_node->substract_l_child->label, WHITE, tree_num);
                backUpEntrySet->insert(n1);
                backUpEntrySet->insert(n2);
                backUpEntrySet->insert(n3);
                backUpEntrySet->insert(n4);
                n1->ajcn_list.insert(unordered_map<string, linked_list_entry>::value_type(n4->value, *n4));
                n3->ajcn_list.insert(unordered_map<string, linked_list_entry>::value_type(n2->value, *n2));
                global_hpu.insert(unordered_map<string, linked_list_entry>::value_type(n1->value, *n1));
                global_hpu.insert(unordered_map<string, linked_list_entry>::value_type(n3->value, *n3));
            }
            else if(_linked_list_entry != global_hpu.end() && _linked_list_entry2 == global_hpu.end()){
                //indicate that the parent node is in HPU
                linked_list_entry *n1 = new linked_list_entry(_temp_node->substract_l_child->label, WHITE, tree_num);
                backUpEntrySet->insert(n1);
                _linked_list_entry->second.ajcn_list.insert(unordered_map<string, linked_list_entry>::value_type(n1->value, *n1));
                unordered_set<int>::iterator belong_set_entry = _linked_list_entry->second.belongs.find(tree_num);
                if(belong_set_entry == _linked_list_entry->second.belongs.end()){
                    //indicate the current parent node is from another tree, therefore, belongs set needs to be updated
                    //kl of this parent node needs to be updated
                    _linked_list_entry->second.belongs.insert(tree_num);
                    _linked_list_entry->second.kl += 1;
                }
                linked_list_entry *n2 = new linked_list_entry(_temp_node->substract_l_child->label, WHITE, tree_num);
                linked_list_entry *n3 = new linked_list_entry(_temp_node->label, WHITE, tree_num);
                backUpEntrySet->insert(n2);
                backUpEntrySet->insert(n3);
                n2->ajcn_list.insert(unordered_map<string, linked_list_entry>::value_type(n3->value, *n3));
                global_hpu.insert(unordered_map<string, linked_list_entry>::value_type(n2->value, *n2));
            }
            else if(_linked_list_entry == global_hpu.end() && _linked_list_entry2 != global_hpu.end()){
                //indicate that the child node is in HPU
                linked_list_entry *n1 = new linked_list_entry(_temp_node->label, WHITE, tree_num);
                backUpEntrySet->insert(n1);
                _linked_list_entry2->second.ajcn_list.insert(unordered_map<string, linked_list_entry>::value_type(n1->value, *n1));
                unordered_set<int>::iterator belong_set_entry = _linked_list_entry2->second.belongs.find(tree_num);
                if(belong_set_entry == _linked_list_entry2->second.belongs.end()){
                    _linked_list_entry2->second.belongs.insert(tree_num);
                    _linked_list_entry2->second.kl += 1;
                }
                linked_list_entry *n2 = new linked_list_entry(_temp_node->label, WHITE, tree_num);
                linked_list_entry *n3 = new linked_list_entry(_temp_node->substract_l_child->label, WHITE, tree_num);
                backUpEntrySet->insert(n2);
                backUpEntrySet->insert(n3);
                n2->ajcn_list.insert(unordered_map<string, linked_list_entry>::value_type(n3->value, *n3));
                global_hpu.insert(unordered_map<string, linked_list_entry>::value_type(n2->value, *n2));
            }

            graph_Edge ge;
            ge.endp1 = _temp_node->label;
            ge.endp2 = _temp_node->substract_l_child->label;
            unordered_map<int, unordered_set<graph_Edge, hash_Edge> >::iterator _parent_child_relation_entry = father_child.find(tree_num);
            if(_parent_child_relation_entry != father_child.end()){
                _parent_child_relation_entry->second.insert(ge);
            }
            else{
                unordered_set<graph_Edge, hash_Edge> tempset;
                tempset.insert(ge);
                father_child.insert(unordered_map<int, unordered_set<graph_Edge, hash_Edge> >::value_type(tree_num, tempset));
            }
            _temp_node->substract_l_child->substract_parent = NULL;
            _temp_node->substract_l_child = NULL;
        }

        if(_temp_node->substract_r_child != NULL){
            _tree_node_queue->push(_temp_node->substract_r_child);
            unordered_map<string, linked_list_entry>::iterator _linked_list_entry = l_list.find(_temp_node->label);
            if(_linked_list_entry != l_list.end()){
                linked_list_entry *n = new linked_list_entry(_temp_node->substract_r_child->label, WHITE, tree_num);
                backUpEntrySet->insert(n);
                _linked_list_entry->second.ajcn_list.insert(unordered_map<string, linked_list_entry>::value_type(n->value, (*n)));
            }
            else{
                linked_list_entry *n1 = new linked_list_entry(_temp_node->label, WHITE, tree_num);
                linked_list_entry *n2 = new linked_list_entry(_temp_node->substract_r_child->label, WHITE, tree_num);
                backUpEntrySet->insert(n1);
                backUpEntrySet->insert(n2);
                n1->ajcn_list.insert(unordered_map<string, linked_list_entry>::value_type(n2->value, *n2));
                l_list.insert(map<string, linked_list_entry>::value_type(n1->value, *n1));
            }
            _linked_list_entry = l_list.find(_temp_node->substract_r_child->label);
            if(_linked_list_entry != l_list.end()){
                linked_list_entry *n = new linked_list_entry(_temp_node->label, WHITE, tree_num);
                backUpEntrySet->insert(n);
                _linked_list_entry->second.ajcn_list.insert(unordered_map<string, linked_list_entry>::value_type(n->value, (*n)));
            }
            else{
                linked_list_entry *n1 = new linked_list_entry(_temp_node->substract_r_child->label, WHITE, tree_num);
                linked_list_entry *n2 = new linked_list_entry(_temp_node->label, WHITE, tree_num);
                backUpEntrySet->insert(n1);
                backUpEntrySet->insert(n2);
                n1->ajcn_list.insert(unordered_map<string, linked_list_entry>::value_type(n2->value, *n2));
                l_list.insert(map<string, linked_list_entry>::value_type(n1->value, *n1));
            }

            //incrementally insert this pair of edge into HPU
            _linked_list_entry = global_hpu.find(_temp_node->label);
            unordered_map<string, linked_list_entry>::iterator _linked_list_entry2 = global_hpu.find(_temp_node->substract_r_child->label);
            if(_linked_list_entry != global_hpu.end() && _linked_list_entry2 != global_hpu.end()){
                //indicate that they have already been in the HPU
                //need to check if there is an edge between them
                //if there is, then check if belongs set is correct
                //if not, then build up the edge, and update corresponding info correctly
                unordered_map<string, linked_list_entry>::iterator _node_exist_entry = _linked_list_entry->second.ajcn_list.find(_temp_node->substract_r_child->label);
                if(_node_exist_entry == _linked_list_entry->second.ajcn_list.end()){
                    //indicate that there is no edge between them from the parent node's side of view
                    linked_list_entry *n = new linked_list_entry(_temp_node->substract_r_child->label, WHITE, tree_num);
                    backUpEntrySet->insert(n);
                    _linked_list_entry->second.ajcn_list.insert(unordered_map<string, linked_list_entry>::value_type(n->value, *n));
                }
                //then check if belongs set contains tree_num
                unordered_set<int>::iterator belong_set_entry = _linked_list_entry->second.belongs.find(tree_num);
                if(belong_set_entry == _linked_list_entry->second.belongs.end()){
                    _linked_list_entry->second.belongs.insert(tree_num);
                    _linked_list_entry->second.kl += 1;
                }

                _node_exist_entry = _linked_list_entry2->second.ajcn_list.find(_temp_node->label);
                if(_node_exist_entry == _linked_list_entry2->second.ajcn_list.end()){
                    //indicate that there is no edge between them from the child node's side of view
                    linked_list_entry *n = new linked_list_entry(_temp_node->label, WHITE, tree_num);
                    backUpEntrySet->insert(n);
                    _linked_list_entry2->second.ajcn_list.insert(unordered_map<string, linked_list_entry>::value_type(n->value, *n));
                }
                //then check if belongs set contains tree_num
                belong_set_entry = _linked_list_entry2->second.belongs.find(tree_num);
                if(belong_set_entry == _linked_list_entry2->second.belongs.end()){
                    _linked_list_entry2->second.belongs.insert(tree_num);
                    _linked_list_entry2->second.kl += 1;
                }
            }
            else if(_linked_list_entry == global_hpu.end() && _linked_list_entry2 == global_hpu.end()){
                //indicate that they are not in the HPU
                linked_list_entry *n1 = new linked_list_entry(_temp_node->label, WHITE, tree_num);
                linked_list_entry *n2 = new linked_list_entry(_temp_node->label, WHITE, tree_num);
                linked_list_entry *n3 = new linked_list_entry(_temp_node->substract_r_child->label, WHITE, tree_num);
                linked_list_entry *n4 = new linked_list_entry(_temp_node->substract_r_child->label, WHITE, tree_num);
                backUpEntrySet->insert(n1);
                backUpEntrySet->insert(n2);
                backUpEntrySet->insert(n3);
                backUpEntrySet->insert(n4);
                n1->ajcn_list.insert(unordered_map<string, linked_list_entry>::value_type(n4->value, *n4));
                n3->ajcn_list.insert(unordered_map<string, linked_list_entry>::value_type(n2->value, *n2));
                global_hpu.insert(unordered_map<string, linked_list_entry>::value_type(n1->value, *n1));
                global_hpu.insert(unordered_map<string, linked_list_entry>::value_type(n3->value, *n3));
            }
            else if(_linked_list_entry != global_hpu.end() && _linked_list_entry2 == global_hpu.end()){
                //indicate that the parent node is in HPU
                linked_list_entry *n1 = new linked_list_entry(_temp_node->substract_r_child->label, WHITE, tree_num);
                backUpEntrySet->insert(n1);
                _linked_list_entry->second.ajcn_list.insert(unordered_map<string, linked_list_entry>::value_type(n1->value, *n1));
                unordered_set<int>::iterator belong_set_entry = _linked_list_entry->second.belongs.find(tree_num);
                if(belong_set_entry == _linked_list_entry->second.belongs.end()){
                    //indicate the current parent node is from another tree, therefore, belongs set needs to be updated
                    //kl of this parent node needs to be updated
                    _linked_list_entry->second.belongs.insert(tree_num);
                    _linked_list_entry->second.kl += 1;
                }
                linked_list_entry *n2 = new linked_list_entry(_temp_node->substract_r_child->label, WHITE, tree_num);
                linked_list_entry *n3 = new linked_list_entry(_temp_node->label, WHITE, tree_num);
                backUpEntrySet->insert(n2);
                backUpEntrySet->insert(n3);
                n2->ajcn_list.insert(unordered_map<string, linked_list_entry>::value_type(n3->value, *n3));
                global_hpu.insert(unordered_map<string, linked_list_entry>::value_type(n2->value, *n2));
            }
            else if(_linked_list_entry == global_hpu.end() && _linked_list_entry2 != global_hpu.end()){
                //indicate that the child node is in HPU
                linked_list_entry *n1 = new linked_list_entry(_temp_node->label, WHITE, tree_num);
                backUpEntrySet->insert(n1);
                _linked_list_entry2->second.ajcn_list.insert(unordered_map<string, linked_list_entry>::value_type(n1->value, *n1));
                unordered_set<int>::iterator belong_set_entry = _linked_list_entry2->second.belongs.find(tree_num);
                if(belong_set_entry == _linked_list_entry2->second.belongs.end()){
                    _linked_list_entry2->second.belongs.insert(tree_num);
                    _linked_list_entry2->second.kl += 1;
                }
                linked_list_entry *n2 = new linked_list_entry(_temp_node->label, WHITE, tree_num);
                linked_list_entry *n3 = new linked_list_entry(_temp_node->substract_r_child->label, WHITE, tree_num);
                backUpEntrySet->insert(n2);
                backUpEntrySet->insert(n3);
                n2->ajcn_list.insert(unordered_map<string, linked_list_entry>::value_type(n3->value, *n3));
                global_hpu.insert(unordered_map<string, linked_list_entry>::value_type(n2->value, *n2));
            }

            graph_Edge ge;
            ge.endp1 = _temp_node->label;
            ge.endp2 = _temp_node->substract_r_child->label;
            unordered_map<int, unordered_set<graph_Edge, hash_Edge> >::iterator _parent_child_relation_entry = father_child.find(tree_num);
            if(_parent_child_relation_entry != father_child.end()){
                _parent_child_relation_entry->second.insert(ge);
            }
            else{
                unordered_set<graph_Edge, hash_Edge> tempset;
                tempset.insert(ge);
                father_child.insert(unordered_map<int, unordered_set<graph_Edge, hash_Edge> >::value_type(tree_num, tempset));
            }
            _temp_node->substract_r_child->substract_parent = NULL;
            _temp_node->substract_r_child = NULL;
        }
    }
    delete _tree_node_queue;
}

//convert each input tree to linked-list structure
multimap<string, unordered_map<string, linked_list_entry> > InputGenerator::build_hpu_non2(in_tree_non2 *&tree, int degree) {
    final_label = 1;

    multimap<string, unordered_map<string, linked_list_entry> > graph;
    graph.clear();

    random_device random_label_rd;
    mt19937_64 random_label_mt(random_label_rd());
    uniform_int_distribution<int> random_label_dist(0, (int)label_list->size() - 1);
    int random_index_for_label = random_label_dist(random_label_mt);
    string random_label = (*label_list)[random_index_for_label];

    for(unordered_map<int, unordered_set<string> >::iterator _subsets_viewer = subsetsByindex.begin();
        _subsets_viewer != subsetsByindex.end(); _subsets_viewer++) {
        _vec_stack_non2._stack_top = (int)_vec_stack_non2._vector_stack->size() - 1;
        //firstly, we need to pre-process the tree according to each subset
        int _index_of_subsets = _subsets_viewer->first;
        _subsets_viewer->second.insert(random_label);
        unordered_set<string> _subset = _subsets_viewer->second;
        //subtracting the tree in order to get the linked list
        unordered_map<string, linked_list_entry> _linked_list_for_subset;
        _linked_list_for_subset.clear();

        subtract_tree_non2(_subset, tree, degree);

        if(sub_tree_root_non2->substract_children.empty()){
            //if the input tree only contains one single node
            linked_list_entry *n1 = new linked_list_entry(sub_tree_root_non2->label, WHITE, _index_of_subsets);
            backUpEntrySet->insert(n1);
            unordered_map<string, linked_list_entry>::iterator _linked_list_entry = global_hpu.find(sub_tree_root_non2->label);
            if(_linked_list_entry != global_hpu.end()){
                //indicate that this node has already been in the HPU
                //then check if kl and belongs set need to be updated
                unordered_set<int>::iterator belong_set_entry = _linked_list_entry->second.belongs.find(_index_of_subsets);
                if(belong_set_entry == _linked_list_entry->second.belongs.end()){
                    _linked_list_entry->second.belongs.insert(_index_of_subsets);
                    _linked_list_entry->second.kl += 1;
                }
            }
            else{
                //indicate that this node is not in the HPU
                linked_list_entry *n2 = new linked_list_entry(sub_tree_root_non2->label, WHITE, _index_of_subsets);
                backUpEntrySet->insert(n2);
                global_hpu.insert(unordered_map<string, linked_list_entry>::value_type(n2->value, *n2));
            }
        }
        else{
            convert_non2_subtree_to_linked_list(_linked_list_for_subset, _index_of_subsets, sub_tree_root_non2);
        }

        linked_list_entry _temp_entry;
        if(sub_tree_root_non2 != NULL){
            unordered_map<string, linked_list_entry>::iterator _linked_list_entry_iter = _linked_list_for_subset.find(sub_tree_root_non2->label);
            if(_linked_list_entry_iter != _linked_list_for_subset.end()){
                _temp_entry = _linked_list_entry_iter->second;
                graph.insert(multimap<string, unordered_map<string, linked_list_entry> >::value_type(_temp_entry.value, _linked_list_for_subset));
            }
            else{
                cout << "Error occurs when generating the HPU!" << endl;
            }
        }

        clear_substract_structure(tree);
    }

    return graph;
}

//convert each input tree to linked-list structure
multimap<string, unordered_map<string, linked_list_entry> > InputGenerator::build_triple_hpu(in_tree *&tree) {
    final_label = 1;

    multimap<string, unordered_map<string, linked_list_entry> > graph;
    graph.clear();

    for(unordered_map<int, unordered_set<string> >::iterator _subsets_viewer = subsetsByindex.begin(); _subsets_viewer !=
                                                                                                       subsetsByindex.end(); _subsets_viewer++){
        _vec_stack._stack_top = (int)_vec_stack._vector_stack->size() - 1;
        //firstly, we need to pre-process the tree according to each subset
        int _index_of_subsets = _subsets_viewer->first;
        unordered_set<string> _subset = _subsets_viewer->second;
        //subtracting the tree in order to get the linked list
        unordered_map<string, linked_list_entry> _linked_list_for_subset;
        _linked_list_for_subset.clear();
        subtract_tree(_subset);
        if(sub_tree_root->substract_l_child == NULL && sub_tree_root->substract_r_child == NULL){
            linked_list_entry *n1 = new linked_list_entry(sub_tree_root->label, WHITE, _index_of_subsets);
            backUpEntrySet->insert(n1);
            _linked_list_for_subset.insert(unordered_map<string, linked_list_entry>::value_type(n1->value, *n1));
            unordered_map<string, linked_list_entry>::iterator _linked_list_entry = global_hpu.find(sub_tree_root->label);
            if(_linked_list_entry != global_hpu.end()){
                //indicate that this node is in the HPU
                //then check if kl and belongs set need to be updated
                unordered_set<int>::iterator belong_set_entry = _linked_list_entry->second.belongs.find(_index_of_subsets);
                if(belong_set_entry == _linked_list_entry->second.belongs.end()){
                    _linked_list_entry->second.belongs.insert(_index_of_subsets);
                    _linked_list_entry->second.kl += 1;
                }
            }
            else{
                //indicate that this node is not in HPU
                linked_list_entry *n2 = new linked_list_entry(sub_tree_root->label, WHITE, _index_of_subsets);
                backUpEntrySet->insert(n2);
                global_hpu.insert(unordered_map<string, linked_list_entry>::value_type(n2->value, *n2));
            }
        }
        else{
            convert_subtree_to_linked_list(_linked_list_for_subset, _index_of_subsets);
        }
        linked_list_entry _temp_entry;
        if(sub_tree_root != NULL){
            unordered_map<string, linked_list_entry>::iterator _linked_list_entry_finder = _linked_list_for_subset.find(sub_tree_root->label);
            if(_linked_list_entry_finder != _linked_list_for_subset.end()){
                _temp_entry = _linked_list_entry_finder->second;
                graph.insert(multimap<string, unordered_map<string, linked_list_entry> >::value_type(_temp_entry.value, _linked_list_for_subset));
            }
            else{
                cout << "Error occurs when generating the HPU!" << endl;
            }
        }
    }

    return graph;
}

//convert each input tree to linked-list structure
multimap<string, unordered_map<string, linked_list_entry> > InputGenerator::build_hpu(in_tree *&tree) {
    final_label = 1;

    multimap<string, unordered_map<string, linked_list_entry> > graph;
    graph.clear();

    random_device random_label_rd;
    mt19937_64 random_label_mt(random_label_rd());
    uniform_int_distribution<int> random_label_dist(0, (int)label_list->size() - 1);
    int random_index_for_label = random_label_dist(random_label_mt);
    string random_label = (*label_list)[random_index_for_label];

    for(unordered_map<int, unordered_set<string> >::iterator _subsets_viewer = subsetsByindex.begin(); _subsets_viewer !=
            subsetsByindex.end(); _subsets_viewer++){
        _vec_stack._stack_top = (int)_vec_stack._vector_stack->size() - 1;
        //firstly, we need to pre-process the tree according to each subset
        int _index_of_subsets = _subsets_viewer->first;
        _subsets_viewer->second.insert(random_label);
        unordered_set<string> _subset = _subsets_viewer->second;
        //subtracting the tree in order to get the linked list
        unordered_map<string, linked_list_entry> _linked_list_for_subset;
        _linked_list_for_subset.clear();
        subtract_tree(_subset);
        check_children_count(sub_tree_root);
        if(sub_tree_root->substract_l_child == NULL && sub_tree_root->substract_r_child == NULL){
            linked_list_entry *n1 = new linked_list_entry(sub_tree_root->label, WHITE, _index_of_subsets);
            backUpEntrySet->insert(n1);
            _linked_list_for_subset.insert(unordered_map<string, linked_list_entry>::value_type(n1->value, *n1));
            unordered_map<string, linked_list_entry>::iterator _linked_list_entry = global_hpu.find(sub_tree_root->label);
            if(_linked_list_entry != global_hpu.end()){
                //indicate that this node is in the HPU
                //then check if kl and belongs set need to be updated
                unordered_set<int>::iterator belong_set_entry = _linked_list_entry->second.belongs.find(_index_of_subsets);
                if(belong_set_entry == _linked_list_entry->second.belongs.end()){
                    _linked_list_entry->second.belongs.insert(_index_of_subsets);
                    _linked_list_entry->second.kl += 1;
                }
            }
            else{
                //indicate that this node is not in HPU
                linked_list_entry *n2 = new linked_list_entry(sub_tree_root->label, WHITE, _index_of_subsets);
                backUpEntrySet->insert(n2);
                global_hpu.insert(unordered_map<string, linked_list_entry>::value_type(n2->value, *n2));
            }
        }
        else{
            convert_subtree_to_linked_list(_linked_list_for_subset, _index_of_subsets);
        }
        linked_list_entry _temp_entry;
        if(sub_tree_root != NULL){
            unordered_map<string, linked_list_entry>::iterator _linked_list_entry_finder = _linked_list_for_subset.find(sub_tree_root->label);
            if(_linked_list_entry_finder != _linked_list_for_subset.end()){
                _temp_entry = _linked_list_entry_finder->second;
                graph.insert(multimap<string, unordered_map<string, linked_list_entry> >::value_type(_temp_entry.value, _linked_list_for_subset));
            }
            else{
                cout << "Error occurs when generating the HPU!" << endl;
            }
        }
    }

    return graph;
}

void InputGenerator::clear_substract_structure(in_tree_non2 *_tree_root) {
    stack<in_tree_non2*> temp_stack;
    temp_stack.push(_tree_root);
    while(!temp_stack.empty()){
        in_tree_non2* temp_node = temp_stack.top();
        temp_stack.pop();
        if(temp_node != NULL){
            for(int i = 0 ; i < temp_node->children.size(); i++){
                temp_stack.push(temp_node->children[i]);
            }
            temp_node->substract_children.clear();
            temp_node->substract_parent = NULL;
        }
    }
}

InputGenerator::~InputGenerator() {
    for(unordered_set<linked_list_entry*>::iterator iterator1 = backUpEntrySet->begin(); iterator1 != backUpEntrySet->end();
    iterator1++){
        delete *iterator1;
    }
    delete backUpEntrySet;

    for(unordered_set<in_tree*>::iterator iterator2 = backUpTreeNode->begin(); iterator2 != backUpTreeNode->end(); iterator2++){
        delete *iterator2;
    }
    delete backUpTreeNode;

    delete _vec_stack._vector_stack;
    delete label_list;
    delete node_list;

    delete node_list_non2;

    delete internal_nodes_vec;
    delete label_parent_map;
    delete in_use_triplet;

    for(unordered_set<unordered_set<string>*>::iterator iterator3 = backUpPtrSet->begin(); iterator3 != backUpPtrSet->end();
    iterator3++){
        delete *iterator3;
    }
    delete backUpPtrSet;

    for(unordered_map<string, vector<unordered_set<string> *> *>::iterator iterator3 = potential_tripletbylabel->begin();
    iterator3 != potential_tripletbylabel->end(); iterator3++){
        delete iterator3->second;
    }

    delete potential_tripletbylabel;
    delete left_descendant;
    delete right_descendant;
    for(unordered_set<in_tree_non2*>::iterator iterator4 = backUpNonTreeNode->begin(); iterator4 != backUpNonTreeNode->end();
    iterator4++){
        delete *iterator4;
    }
    delete backUpNonTreeNode;

    delete _vec_stack_non2._vector_stack;


}