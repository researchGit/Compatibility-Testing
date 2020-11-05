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
// Detailed implementation of CompatibilityAnalyzer.h
//

#include "CompatibilityAnalyzer.h"
#include <fstream>
#include <queue>

using namespace std;

string CompatibilityAnalyzer::convert2newick(tree_node *tree, string newick) {
    if(tree != NULL){
        if(tree->chd.size() > 0){
            //this tree has the children
            newick += "(";
            for(unordered_set<tree_node *>::iterator _child_entry = tree->chd.begin(); _child_entry != tree->chd.end();
                _child_entry++){
                newick = convert2newick((*_child_entry), newick);
                newick += ',';
            }
            if(newick[newick.length()-1] == ','){
                newick = newick.substr(0, newick.length()-1);
            }
            newick += ")";
        }

        string value = tree->label;
        string weight = "";
        if(tree->weight.length() > 0){
            value += ":";
            value += tree->weight;
        }


        newick += value;
    }
    return newick;
}

void CompatibilityAnalyzer::print_tree(tree_node *root) {
    cout << "---------------------------- check tree ----------------------------" << endl;
    queue<tree_node*> queue1;
    if(root != NULL){
        queue1.push(root);
        while(!queue1.empty()){
            tree_node* temp_node = queue1.front();
            queue1.pop();
            cout << temp_node->label << "(" << temp_node->weight  << ") --- ";
            for(unordered_set<dis_tree*>::iterator iterator1 = temp_node->chd.begin(); iterator1 != temp_node->chd.end();
                    iterator1++){
                queue1.push((*iterator1));
                cout << (*iterator1)->label << "(" << (*iterator1)->weight << ") ";
            }
            cout << endl;
        }
    }
}

//Given a path of a source file as the input, this method will read all Newick format strings written in the file
//and then parse them separately into linked-list tree structure
void CompatibilityAnalyzer::initializer(string file_path) {

    ifstream file(file_path);

    string input_str = "";

    int tree_counter = 1;//used to mark the index of the tree, for example, the first Newick format string is assigned with index 1

    Parser *parser = new Parser();
    backUpEntrySet = new unordered_set<linked_list_entry*>();
    int tree_count = 0;
    while(getline(file, input_str)){
        //firstly, the input string will be translated into tree structure
        tree_node* root = parser->parse_label_weight(input_str);
        if(root != NULL){
            tree_count++;
        }
//        cout << "tree " << tree_count << " is being parsed"<< endl;
//        print_tree(root);
//        string correctness = "";
//        correctness = convert2newick(root, correctness);
//        cout << correctness << endl;
        //then, tree structure will be converted into linked-list structure
        if(root != NULL && root->chd.empty()){
//            cout << "here?" << endl;
            linked_list_entry *n1 = new linked_list_entry(root->label, WHITE, tree_counter);
            backUpEntrySet->insert(n1);
            unordered_map<string, linked_list_entry> linked_list;
            linked_list.insert(unordered_map<string, linked_list_entry>::value_type(n1->value, *n1));
            unordered_map<string, linked_list_entry>::iterator _linked_list_node_entry = linked_list.find(root->label);
            if(_linked_list_node_entry != linked_list.end()){
                input_trees.insert(unordered_map<string, unordered_map<string, linked_list_entry> >::value_type(root->label, linked_list));
            }
            unordered_map<string, linked_list_entry>::iterator _linked_list_entry = HPU.find(root->label);
            if(_linked_list_entry != HPU.end()){
                //indicate that this node is in the HPU
                //then check if kl and belongs set need to be updated
                unordered_set<int>::iterator belong_set_entry = _linked_list_entry->second.belongs.find(tree_counter);
                if(belong_set_entry == _linked_list_entry->second.belongs.end()){
                    _linked_list_entry->second.belongs.insert(tree_counter);
                    _linked_list_entry->second.kl += 1;
                }
            }
            else{
                //indicate that this node is not in HPU
                linked_list_entry *n2 = new linked_list_entry(root->label, WHITE, tree_counter);
                backUpEntrySet->insert(n2);
                HPU.insert(unordered_map<string, linked_list_entry>::value_type(n2->value, *n2));
            }
//            cout << "here?" << endl;

        }
        else{
            unordered_map<string, linked_list_entry> linked_list = convert2list(root, tree_counter);
            unordered_map<string, linked_list_entry>::iterator _linked_list_node_entry = linked_list.find(root->label);
            if(_linked_list_node_entry != linked_list.end()){
                input_trees.insert(unordered_map<string, unordered_map<string, linked_list_entry> >::value_type(root->label, linked_list));
            }
        }
        tree_counter++;

        stack<tree_node *> *tempStack = new stack<tree_node *>();
        if(root != nullptr){
            tempStack->push(root);
        }
//        cout << "here?????" << endl;
        while(!tempStack->empty()){
            tree_node *temptr = tempStack->top();
            tempStack->pop();
//            cout << temptr->chd.empty() << endl;
            for(unordered_set<tree_node*>::iterator setIt = temptr->chd.begin(); setIt != temptr->chd.end(); setIt++){
                if(*setIt != nullptr){
                    tempStack->push(*setIt);
                }
            }
            delete temptr;
        }
        delete tempStack;
//        cout << "here???" << endl;
    }
    //finally, initialize other necessary structures that are used by HDT
    Linked_List_Builder lb;

//    int ll = 0;
//    for(multimap<string, unordered_map<string, linked_list_entry> >::iterator iterator1 = input_trees.begin(); iterator1 !=
//    input_trees.end(); iterator1++){
//        cout << ++ll << " " << iterator1->first << " ----- ";
//        for(unordered_map<string, linked_list_entry>::iterator iterator2 = iterator1->second.begin();
//        iterator2 != iterator1->second.end(); iterator2++){
//            for(unordered_map<string, linked_list_entry>::iterator iterator3 = iterator2->second.ajcn_list.begin();
//            iterator3 != iterator2->second.ajcn_list.end(); iterator3++){
//                cout << iterator3->first << " ";
//            }
//            cout << endl;
//        }
//    }

    //need to revise to compute hpu while process each edge
    lb.merge_input_trees(input_trees, HPU);
    sum_kl = lb.get_sum_kl();
    valid_pos = lb.get_valid_position();
    semi_universal_label = lb.get_semi_universal_nodes();

//    for(unordered_map<string, linked_list_entry>::iterator iterator1 = HPU.begin(); iterator1 != HPU.end();
//    iterator1++){
//        cout << iterator1->first << " --- " ;
//        for(unordered_map<string, linked_list_entry>::iterator iterator2 = iterator1->second.ajcn_list.begin();
//        iterator2 != iterator1->second.ajcn_list.end(); iterator2++){
//            cout << iterator2->first << " " ;
//        }
//        cout << endl;
//    }

    delete parser;
}

// Given a tree and its index, this method is designed to convert a tree into a linked list
unordered_map<string, linked_list_entry> CompatibilityAnalyzer::convert2list(tree_node *root, int tree_index) {
    unordered_map<string, linked_list_entry> linked_list;
    if(root != NULL){
        unordered_set<graph_Edge, hash_Edge> parent_child_relation_set;
        parent_child_set.insert(unordered_map<int, unordered_set<graph_Edge, hash_Edge> >::value_type(tree_index, parent_child_relation_set));
        tree_node* temp_node;
        queue<tree_node*> node_queue;
        node_queue.push(root);
        while(!node_queue.empty()){
            temp_node = node_queue.front();
            node_queue.pop();
            for(unordered_set<tree_node*>::iterator _chd_set = temp_node->chd.begin(); _chd_set != temp_node->chd.end();
                _chd_set++){
                //add each child into the node_queue for further computation of its linked list
                node_queue.push(*_chd_set);
                graph_Edge2 ge;
                if(temp_node->label < (*_chd_set)->label){
                    ge.endp1 = temp_node->label;
                    ge.endp2 = (*_chd_set)->label;
                    ge.level = tree_index;
                }
                else{
                    ge.endp1 = (*_chd_set)->label;
                    ge.endp2 = temp_node->label;
                    ge.level = tree_index;
                }
                edge.insert(ge);

                unordered_map<string, linked_list_entry>::iterator _linked_list_finder = linked_list.find(temp_node->label);
                if(_linked_list_finder != linked_list.end()){
                    linked_list_entry *n = new linked_list_entry((*_chd_set)->label, WHITE, tree_index);
                    backUpEntrySet->insert(n);
                    _linked_list_finder->second.ajcn_list.insert(unordered_map<string, linked_list_entry>::value_type(n->value, (*n)));
                }
                else{
                    linked_list_entry *n1 = new linked_list_entry(temp_node->label, WHITE, tree_index);
                    linked_list_entry *n2 = new linked_list_entry((*_chd_set)->label, WHITE, tree_index);
                    backUpEntrySet->insert(n1);
                    backUpEntrySet->insert(n2);
                    n1->ajcn_list.insert(unordered_map<string, linked_list_entry>::value_type(n2->value, *n2));
                    linked_list.insert(map<string, linked_list_entry>::value_type(n1->value, *n1));
                }
                _linked_list_finder = linked_list.find((*_chd_set)->label);
                if(_linked_list_finder != linked_list.end()){
                    linked_list_entry *n = new linked_list_entry(temp_node->label, WHITE, tree_index);
                    backUpEntrySet->insert(n);
                    _linked_list_finder->second.ajcn_list.insert(unordered_map<string, linked_list_entry>::value_type(n->value, (*n)));
                }
                else{
                    linked_list_entry *n1 = new linked_list_entry((*_chd_set)->label, WHITE, tree_index);
                    linked_list_entry *n2 = new linked_list_entry(temp_node->label, WHITE, tree_index);
                    backUpEntrySet->insert(n1);
                    backUpEntrySet->insert(n2);
                    n1->ajcn_list.insert(unordered_map<string, linked_list_entry>::value_type(n2->value, *n2));
                    linked_list.insert(map<string, linked_list_entry>::value_type(n1->value, *n1));
                }

                _linked_list_finder = HPU.find(temp_node->label);
                unordered_map<string, linked_list_entry>::iterator _linked_list_finder2 = HPU.find((*_chd_set)->label);
                if(_linked_list_finder != HPU.end() && _linked_list_finder2 != HPU.end()){
                    //indicate that they have already been in the HPU
                    //need to check if there is an edge between them
                    //if there is, then check if belongs set is correct
                    //if not, then build up the edge, and update corresponding info correctly
                    unordered_map<string, linked_list_entry>::iterator _node_exist_entry = _linked_list_finder->second.ajcn_list.find((*_chd_set)->label);
                    if(_node_exist_entry == _linked_list_finder->second.ajcn_list.end()){
                        //indicate that there is no edge between them from the parent node's side of view
                        linked_list_entry *n = new linked_list_entry((*_chd_set)->label, WHITE, tree_index);
                        backUpEntrySet->insert(n);
                        _linked_list_finder->second.ajcn_list.insert(unordered_map<string, linked_list_entry>::value_type(n->value, *n));
                    }
                    //then check if belongs set contains tree_index
                    unordered_set<int>::iterator belong_set_entry = _linked_list_finder->second.belongs.find(tree_index);
                    if(belong_set_entry == _linked_list_finder->second.belongs.end()){
                        _linked_list_finder->second.belongs.insert(tree_index);
                        _linked_list_finder->second.kl += 1;
                    }

                    _node_exist_entry = _linked_list_finder2->second.ajcn_list.find(temp_node->label);
                    if(_node_exist_entry == _linked_list_finder2->second.ajcn_list.end()){
                        //indicate that there is no edge between them from the child node's side of view
                        linked_list_entry *n = new linked_list_entry(temp_node->label, WHITE, tree_index);
                        backUpEntrySet->insert(n);
                        _linked_list_finder2->second.ajcn_list.insert(unordered_map<string, linked_list_entry>::value_type(n->value, *n));
                    }
                    //then check if belongs set contains tree_index
                    belong_set_entry = _linked_list_finder2->second.belongs.find(tree_index);
                    if(belong_set_entry == _linked_list_finder2->second.belongs.end()){
                        _linked_list_finder2->second.belongs.insert(tree_index);
                        _linked_list_finder2->second.kl += 1;
                    }
                }
                else if(_linked_list_finder == HPU.end() && _linked_list_finder2 == HPU.end()){
                    //indicate that they are not in the HPU
                    linked_list_entry *n1 = new linked_list_entry(temp_node->label, WHITE, tree_index);
                    linked_list_entry *n2 = new linked_list_entry(temp_node->label, WHITE, tree_index);
                    linked_list_entry *n3 = new linked_list_entry((*_chd_set)->label, WHITE, tree_index);
                    linked_list_entry *n4 = new linked_list_entry((*_chd_set)->label, WHITE, tree_index);
                    backUpEntrySet->insert(n1);
                    backUpEntrySet->insert(n2);
                    backUpEntrySet->insert(n3);
                    backUpEntrySet->insert(n4);
                    n1->ajcn_list.insert(unordered_map<string, linked_list_entry>::value_type(n4->value, *n4));
                    n3->ajcn_list.insert(unordered_map<string, linked_list_entry>::value_type(n2->value, *n2));
                    HPU.insert(unordered_map<string, linked_list_entry>::value_type(n1->value, *n1));
                    HPU.insert(unordered_map<string, linked_list_entry>::value_type(n3->value, *n3));
                }
                else if(_linked_list_finder != HPU.end() && _linked_list_finder2 == HPU.end()){
                    //indicate that the parent node is in HPU
                    linked_list_entry *n1 = new linked_list_entry((*_chd_set)->label, WHITE, tree_index);
                    backUpEntrySet->insert(n1);
                    _linked_list_finder->second.ajcn_list.insert(unordered_map<string, linked_list_entry>::value_type(n1->value, *n1));
                    unordered_set<int>::iterator belong_set_entry = _linked_list_finder->second.belongs.find(tree_index);
                    if(belong_set_entry == _linked_list_finder->second.belongs.end()){
                        //indicate the current parent node is from another tree, therefore, belongs set needs to be updated
                        //kl of this parent node needs to be updated
                        _linked_list_finder->second.belongs.insert(tree_index);
                        _linked_list_finder->second.kl += 1;
                    }
                    linked_list_entry *n2 = new linked_list_entry((*_chd_set)->label, WHITE, tree_index);
                    linked_list_entry *n3 = new linked_list_entry(temp_node->label, WHITE, tree_index);
                    backUpEntrySet->insert(n2);
                    backUpEntrySet->insert(n3);
                    n2->ajcn_list.insert(unordered_map<string, linked_list_entry>::value_type(n3->value, *n3));
                    HPU.insert(unordered_map<string, linked_list_entry>::value_type(n2->value, *n2));
                }
                else if(_linked_list_finder == HPU.end() && _linked_list_finder2 != HPU.end()){
                    //indicate that the child node is in HPU
                    linked_list_entry *n1 = new linked_list_entry(temp_node->label, WHITE, tree_index);
                    backUpEntrySet->insert(n1);
                    _linked_list_finder2->second.ajcn_list.insert(unordered_map<string, linked_list_entry>::value_type(n1->value, *n1));
                    unordered_set<int>::iterator belong_set_entry = _linked_list_finder2->second.belongs.find(tree_index);
                    if(belong_set_entry == _linked_list_finder2->second.belongs.end()){
                        _linked_list_finder2->second.belongs.insert(tree_index);
                        _linked_list_finder2->second.kl += 1;
                    }
                    linked_list_entry *n2 = new linked_list_entry(temp_node->label, WHITE, tree_index);
                    linked_list_entry *n3 = new linked_list_entry((*_chd_set)->label, WHITE, tree_index);
                    backUpEntrySet->insert(n2);
                    backUpEntrySet->insert(n3);
                    n2->ajcn_list.insert(unordered_map<string, linked_list_entry>::value_type(n3->value, *n3));
                    HPU.insert(unordered_map<string, linked_list_entry>::value_type(n2->value, *n2));
                }

                //add new parent-child relation into the relation set
                unordered_map<int, unordered_set<graph_Edge, hash_Edge> >::iterator _parent_child_set_entry = parent_child_set.find(tree_index);
                if(_parent_child_set_entry != parent_child_set.end()){
                    graph_Edge ge1;
                    ge1.endp1 = temp_node->label;
                    ge1.endp2 = (*_chd_set)->label;
                    _parent_child_set_entry->second.insert(ge1);
                }

                //add new child-parent relation into the relation set
                unordered_map<string, unordered_set<string> >::iterator _child_parent_set_entry = child_parent_set.find((*_chd_set)->label);
                if(_child_parent_set_entry != child_parent_set.end()){
                    _child_parent_set_entry->second.insert(temp_node->label);
                }
                else{
                    unordered_set<string> tempset;
                    tempset.insert(temp_node->label);
                    child_parent_set.insert(unordered_map<string, unordered_set<string> >::value_type((*_chd_set)->label, tempset));
                }
            }
        }
    }

    return linked_list;
}

CompatibilityAnalyzer::~CompatibilityAnalyzer() {
//    cout << "here" << endl;
    for(unordered_set<linked_list_entry *>::iterator setIt = backUpEntrySet->begin(); setIt != backUpEntrySet->end();
    setIt++){
        delete *setIt;
    }
    delete backUpEntrySet;
}