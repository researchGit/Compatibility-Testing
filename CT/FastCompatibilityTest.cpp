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
// This file contains main body of fast compatibility testing alagorithm described by David Fernandez-Baca and Yun Deng
// references of their original work can be found in the README.md file
//

#include "FastCompatibilityTest.h"
#include <ctime>

using namespace std;

//initialization without parameters is empty
FastCompatibilityTest::FastCompatibilityTest() {

}

//initialization with all necessary parameters
FastCompatibilityTest::FastCompatibilityTest(unordered_map<string, linked_list_entry> component,
                                             unordered_map<int, unordered_set<string> > map,
                                             unordered_map<string, int> semi, int kl, vector<occ_Node> seq,
                                             unordered_map<string, array_Node> *_array,
                                             unordered_map<int, unordered_set<graph_Edge, hash_Edge> > tedge,
                                             unordered_map<int, unordered_set<graph_Edge, hash_Edge> > ntedge,
                                             unordered_map<graph_Edge, int, hash_Edge> edges, et_tree_Node *root,
                                             unordered_map<int, unordered_set<graph_Edge, hash_Edge> > father_children,
                                             unordered_map<int, unordered_map<string, unordered_set<graph_Edge, hash_Edge> > > query_table,
                                             bool edge_promotion) {
    hpu = component;
    initial_semi = semi;
    valpos = map;
    //to build up a supertree, a dummy root is created, it will be deleted later from the result
    tree_root.label = "dummy root";
    tree_root.parent = nullptr;
    dis_graph = &tree_root;
    sum_kl = kl;
    _edges = edges;
    fachild = father_children;

    //initialize subpool structure, which is used for recording the current count of exposed singleton
    for(unordered_map<int, unordered_set<string> >::iterator _valid_position_entry = valpos.begin(); _valid_position_entry != valpos.end(); _valid_position_entry++){

        for(unordered_set<string>::iterator _set_entry = _valid_position_entry->second.begin();
            _set_entry != _valid_position_entry->second.end(); _set_entry++){
            if(initial_semi.find(*_set_entry) != initial_semi.end()){
                continue;
            }
            unordered_map<string, unordered_set<int> >::iterator _pool_entry = subpool.find(*_set_entry);
            if(_pool_entry != subpool.end()){
                _pool_entry->second.insert(_valid_position_entry->first);
            }
            else{
                unordered_set<int> tempset;
                tempset.insert(_valid_position_entry->first);
                subpool.insert(unordered_map<string, unordered_set<int> >::value_type(*(_valid_position_entry->second.begin()), tempset));
            }
        }

    }

    seq = seq;
    array = (*_array);
    _tedge = tedge;
    _ntedge = ntedge;

    //initialize HDT data structure at the same time
    HDT _hdt(seq, array, _tedge, _ntedge, _edges, root, component, (int)floor(log10(seq.size())/log10(2)), query_table, edge_promotion);
    hdt = _hdt;

}

//main method of BUILD
int FastCompatibilityTest::build(bool internal_labeled) {
    //initial_set is the initial semi-universal set in U
    //if s = empty return incompatible
    if(initial_semi.empty()){
        return INCOMPATIBLE;
    }
    else if((initial_semi.size() == 1) && (hpu.size() == 1) &&(hpu.find(initial_semi.begin()->first) != hpu.end())) {
        //if size of s is 1 and the label l does not have proper descendant
        dis_Node *node = new dis_Node(initial_semi.begin()->first);
        node->parent = dis_graph;
        dis_graph->children.push_back(node);
        return FINISHED;
    }
    else{

        //new method of controlling the construction
        if(Q_potential.empty()){
            Y y;
            y.component = hpu;
            y.map = valpos;
            y.semi_potential = initial_semi;
            y.treep = dis_graph;
            y.sum_kl = sum_kl;
            Y obj;
            Q_potential.push_back(obj);
            swap(Q_potential.back(), y);
        }
        while(!Q_potential.empty()){

            Y origin_component;
            swap(origin_component, Q_potential.front());
            Q_potential.pop_front();
            swap(origin_component.semi, origin_component.semi_potential);

            if(origin_component.semi.empty()){
                return INCOMPATIBLE;
            }
            else{
                dis_Node *node = new dis_Node("");
                node->parent = origin_component.treep;

                Y obj;
                Q.push_back(obj);
                swap(Q.back(), origin_component);

                if(node->parent != NULL){
                    node->parent->children.push_back(node);
                    adj_size = 0;

                    while(!Q.empty()){
                        Y cy;
                        swap(cy, Q.front());
                        Q.pop_front();

                        string semi_label = cy.semi.begin()->first;

                        adj_size++;

                        //get rid of dummy labels created for convience
                        if(semi_label.substr(0, 2) == "f_"){
                            node->label += "";
                        }
                        else{
                            node->label += semi_label;
                            node->label += "+";
                        }

                        label_count.insert(semi_label);


                        deletion(semi_label, &cy, node->parent, internal_labeled, node);
                    }

                    //delete the last "+" sign
                    node->label = node->label.substr(0, node->label.length() - 1);
                }
            }
        }
    }

    unordered_map<string, unordered_set<int> > swap_label_tree;
    unordered_map<string, int> swap_semi_label_tree;
    unordered_map<string, unordered_set<int> > swap_unsemi_label_tree;
    swap(label_tree, swap_label_tree);
    swap(semi_label_tree, swap_semi_label_tree);
    swap(unsemi_label_tree, swap_unsemi_label_tree);

    return FINISHED;
}

//iteratively translate a tree into newick format string
string FastCompatibilityTest::convert2newick(dis_Node *tree, string newick) {
    if(tree != NULL){
        if(tree->children.size() > 0){
            //this tree has the children
            newick += "(";
            for(vector<dis_Node *>::iterator _child_entry = tree->children.begin(); _child_entry != tree->children.end();
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

//given two strings, which are original string and compare string
//return true if this two strings form an edge which original string is the parent in the tree and compare string is the child
bool FastCompatibilityTest::father_child_relation(string original, int num, string compare) {
    bool _parent_child_relation_checker = false;
    unordered_map<int, unordered_set<graph_Edge, hash_Edge> >::iterator _tree_finder = fachild.find(num);
    if(_tree_finder != fachild.end()){
        graph_Edge temp;
        temp.endp1 = original;
        temp.endp2 = compare;
        unordered_set<graph_Edge, hash_Edge>::iterator _father_child_relation_finder = _tree_finder->second.find(temp);
        if(_father_child_relation_finder != _tree_finder->second.end()){
            _parent_child_relation_checker = true;
        }
        else{
            //cout <<"Label error! " << num << endl;
        }
    }
    else{
        cout << "Trees error!" << endl;
    }
    return _parent_child_relation_checker;
}

//given two strings u and v
//delete the edge in the HPU structure
void FastCompatibilityTest::delete_component_edge(string u, string v, unordered_map<string, linked_list_entry> *com) {
    unordered_map<string, linked_list_entry>::iterator _hpu_entry = (*com).find(u);
    if(_hpu_entry != (*com).end()){
        unordered_map<string, linked_list_entry>::iterator _ajcn_list_entry = _hpu_entry->second.ajcn_list.find(v);
        if(_ajcn_list_entry != _hpu_entry->second.ajcn_list.end()){
            _hpu_entry->second.ajcn_list.erase(_ajcn_list_entry);
        }
    }
}

//core method of taxa compatibility test algorithm
//this method will make changes on components, maps, and semi-universal nodes
//the method for updating semi-universal nodes has been revised which is different from the one proposed in the paper
//in order to improve the efficiency of the algorithm
void FastCompatibilityTest::deletion(string l, Y *y, dis_Node *node, bool internal_labeled, dis_Node *node_potential) {
    //firstly, delete this label from the original semi universal set of the component
    //and update the map sets with right children labels
    unordered_map<string, int>::iterator _semi_universal_node_entry = (*y).semi.find(l);
    if(_semi_universal_node_entry != (*y).semi.end()){
        (*y).semi.erase(_semi_universal_node_entry);
        //correspondingly, delete the entry in the HPU structure which is related to this label
        unordered_map<string, linked_list_entry>::iterator _hpu_entry = (*y).component.find(l);
        unordered_set<int> belongs;
        if(_hpu_entry != (*y).component.end()) {
            belongs = _hpu_entry->second.belongs;

            //for each index i in the index set of this label such that this label belongs to L(Ti)
            //delete this label from each map that contains this label
            //add all the children that belongs to CH(i) into the corresponding map
            for(unordered_set<int>::iterator _belong_set_entry = belongs.begin(); _belong_set_entry != belongs.end(); _belong_set_entry++) {
                unordered_map<int, unordered_set<string> >::iterator _map_entry = (*y).map.find(*_belong_set_entry);
                if (_map_entry != (*y).map.end()) {
                    (*y).map.erase(_map_entry);
                }
                unordered_map<string, linked_list_entry> adj = _hpu_entry->second.ajcn_list;
                unordered_set<string> _temp_set;
                for(unordered_map<string, linked_list_entry>::iterator _ajcn_list_entry = adj.begin(); _ajcn_list_entry != adj.end(); _ajcn_list_entry++){
                    unordered_map<string, linked_list_entry>::iterator _hpu_entry2 = (*y).component.find(_ajcn_list_entry->first);
                    if(_hpu_entry2 != (*y).component.end()){
                        if ((_hpu_entry2->second.belongs.find(*_belong_set_entry) != _hpu_entry2->second.belongs.end())
                            && father_child_relation(l, (*_belong_set_entry), _ajcn_list_entry->first)) {
                            _temp_set.insert(_ajcn_list_entry->first);
                        }
                    }
                }
                if(!(_temp_set.empty())){
                    (*y).map.insert(unordered_map<int, unordered_set<string> >::value_type(*_belong_set_entry, _temp_set));
                    if(_temp_set.size() == 1){
                        string beta = *(_temp_set.begin());
                        unordered_map<string, unordered_set<int> >::iterator _subpool_entry = subpool.find(beta);
                        if(_subpool_entry != subpool.end()){
                            _subpool_entry->second.insert(*_belong_set_entry);
                        }
                        else{
                            unordered_set<int> tempset;
                            tempset.insert(*_belong_set_entry);
                            subpool.insert(unordered_map<string, unordered_set<int> >::value_type(beta, tempset));
                        }
                        _subpool_entry = subpool.find(beta);
                        if(_subpool_entry != subpool.end()){
                            string string2 = _subpool_entry->first;
                            unordered_map<string, linked_list_entry>::iterator _hpu_entry4 = (*y).component.find(string2);
                            if(_hpu_entry4 != (*y).component.end()){
                                if(_hpu_entry4->second.count < _hpu_entry4->second.kl){
                                    _hpu_entry4->second.count = (int)_subpool_entry->second.size();
                                    if(_hpu_entry4->second.count == _hpu_entry4->second.kl){
                                        (*y).semi.insert(unordered_map<string, int>::value_type(string2, *(_subpool_entry->second.begin())));
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    //secondly, delete every edge starting from label l in the HPU structure
    //this step will create two new components y1 and y2

    unordered_map<string, linked_list_entry>::iterator _hpu_entry = (*y).component.find(l);
    if(_hpu_entry != (*y).component.end()){
        Y ya[2];
        int seperate = 0;
        int fon = 0;
        unordered_map<string, linked_list_entry> adj = _hpu_entry->second.ajcn_list;
        int adjsize = 0;

        //for each edge starting from label l, delete this edge from the HPU structure
        for(unordered_map<string, linked_list_entry>::iterator _ajcn_list_entry = adj.begin();
            _ajcn_list_entry != adj.end(); _ajcn_list_entry++){
            adjsize ++;
            string string1 = _ajcn_list_entry->first;

            delete_component_edge(l,string1,&((*y).component));
            delete_component_edge(string1,l,&((*y).component));

            hdt.delete_Edge(l, string1, false);

            int edge_level = hdt.get_edge_level(l, string1);

            //deletion will continue if a deletion does not create new component
            if(hdt.fct_connected(l, string1, edge_level)){
                continue;
            }

            //when new component is exposed
            seperate += 1;
            fon += 1;
            unordered_set<string> visited;
            queue<string> visit;
            int count_kl1 = 0;
            int count_kl2 = 0;
            int sum_kl = (*y).sum_kl;

            //update HPU structure
            unordered_map<string, linked_list_entry> newlist;
            unordered_map<string, int> semi_of_smaller;
            unordered_map<string, int> semi_potential_of_smaller;

            string sstr = hdt.smaller_tree_label(l, string1, edge_level);
            unordered_map<string, linked_list_entry>::iterator _hpu_entry2 = (*y).component.find(sstr);
            if(_hpu_entry2 != (*y).component.end()){
                count_kl1 += _hpu_entry2->second.kl;
                visited.insert(_hpu_entry2->first);

                //move semi node
                unordered_map<string, int>::iterator semi_it = (*y).semi.find(_hpu_entry2->first);
                if(semi_it != (*y).semi.end()){
                    semi_of_smaller.insert(unordered_map<string, int>::value_type(semi_it->first, semi_it->second));
                    (*y).semi.erase(semi_it);
                }

                unordered_map<string, int>::iterator semi_potential_it = (*y).semi_potential.find(_hpu_entry2->first);
                if(semi_potential_it != (*y).semi_potential.end()){
                    semi_potential_of_smaller.insert(unordered_map<string, int>::value_type(semi_potential_it->first, semi_potential_it->second));
                    (*y).semi_potential.erase(semi_potential_it);
                }

                for(unordered_map<string, linked_list_entry>::iterator _ajcn_list_entry2 = _hpu_entry2->second.ajcn_list.begin();
                    _ajcn_list_entry2 != _hpu_entry2->second.ajcn_list.end(); _ajcn_list_entry2++){
                    visited.insert(_ajcn_list_entry2->first);
                    visit.push(_ajcn_list_entry2->first);

                    //move semi node
                    semi_it = (*y).semi.find(_ajcn_list_entry2->first);
                    if(semi_it != (*y).semi.end()){
                        semi_of_smaller.insert(unordered_map<string, int>::value_type(semi_it->first, semi_it->second));
                        (*y).semi.erase(semi_it);
                    }

                    semi_potential_it = (*y).semi_potential.find(_ajcn_list_entry2->first);
                    if(semi_potential_it != (*y).semi_potential.end()){
                        semi_potential_of_smaller.insert(unordered_map<string, int>::value_type(semi_potential_it->first, semi_potential_it->second));
                        (*y).semi_potential.erase(semi_potential_it);
                    }
                }
                newlist.insert(unordered_map<string, linked_list_entry>::value_type(_hpu_entry2->first,_hpu_entry2->second));
                (*y).component.erase(_hpu_entry2);
                while(!visit.empty()){
                    string string2;
                    string2 = visit.front();
                    visit.pop();
                    unordered_map<string, linked_list_entry>::iterator _hpu_entry3 = (*y).component.find(string2);
                    if(_hpu_entry3 != (*y).component.end()){
                        count_kl1 += _hpu_entry3->second.kl;
                        for(unordered_map<string, linked_list_entry>::iterator _ajcn_list_entry3 = _hpu_entry3->second.ajcn_list.begin();
                            _ajcn_list_entry3 != _hpu_entry3->second.ajcn_list.end(); _ajcn_list_entry3++){
                            unordered_set<string>::iterator _node_visited_entry = visited.find(_ajcn_list_entry3->first);
                            if(_node_visited_entry == visited.end()){
                                visited.insert(_ajcn_list_entry3->first);
                                visit.push(_ajcn_list_entry3->first);

                                //move semi node
                                semi_it = (*y).semi.find(_ajcn_list_entry3->first);
                                if(semi_it != (*y).semi.end()){
                                    semi_of_smaller.insert(unordered_map<string, int>::value_type(semi_it->first, semi_it->second));
                                    (*y).semi.erase(semi_it);
                                }

                                semi_potential_it = (*y).semi_potential.find(_ajcn_list_entry3->first);
                                if(semi_potential_it != (*y).semi_potential.end()){
                                    semi_potential_of_smaller.insert(unordered_map<string, int>::value_type(semi_potential_it->first, semi_potential_it->second));
                                    (*y).semi_potential.erase(semi_potential_it);
                                }
                            }
                        }
                        newlist.insert(unordered_map<string, linked_list_entry>::value_type(_hpu_entry3->first,_hpu_entry3->second));
                        (*y).component.erase(_hpu_entry3);
                    }
                }

                count_kl2 = sum_kl - count_kl1;

                //the original map is assigned to ya[1], which is the bigger component between these two component
                ya[0].sum_kl = count_kl1;
                ya[0].map.clear();
                ya[0].semi.clear();
                ya[0].semi_potential.clear();
                swap(ya[0].component, newlist);
                swap(ya[0].semi, semi_of_smaller);
                swap(ya[0].semi_potential, semi_potential_of_smaller);

                ya[1].sum_kl = count_kl2;
                swap(ya[1].map, (*y).map);
                swap(ya[1].component, (*y).component);
                swap(ya[1].semi, (*y).semi);
                swap(ya[1].semi_potential, (*y).semi_potential);

                if(ya[0].component.size() > ya[1].component.size()){
                    cout << "please check" << endl;
                    cout << ya[0].component.size() << endl;
                    cout << ya[1].component.size() << endl;
                    exit(0);
                }
                //update semi-universal node
                //a pool is used to update and check if a label is exposed as a semi-universal node after moving from ya[1]'s map
                //to ya[0]'s map
                unordered_set<int> J;
                unordered_set<string> jj;
                for(unordered_map<string, linked_list_entry>::iterator _hpu_entry3 = ya[0].component.begin();
                    _hpu_entry3 != ya[0].component.end(); _hpu_entry3++){
                    string beta = _hpu_entry3->second.value;
                    jj.insert(beta);
                    unordered_set<int> belongs = _hpu_entry3->second.belongs;
                    //for each index i of this label
                    //delete this label from the map set in ya[1] component
                    //add this label into the map set with index i in ya[0] component
                    //if the map[i] in ya[0] contains only 1 element after moving
                    //then record this index in the pool
                    //if the map[i] in ya[0] contains 2 elements after moving
                    //delete the label that is in the map[i] before moving from corresponding pool entry
                    //check if the total updates of a label in the pool matches kl
                    //if the number matches, then the label is semi-universal node
                    //otherwise, the label is not semi-universal node
                    for(unordered_set<int>::iterator _belong_set_entry = belongs.begin(); _belong_set_entry != belongs.end(); _belong_set_entry++){
                        unordered_map<int, unordered_set<string> >::iterator _map_index_entry = ya[1].map.find(*_belong_set_entry);
                        if(_map_index_entry != ya[1].map.end()) {
                            unordered_set<string>::iterator _map_entry = _map_index_entry->second.find(beta);
                            if (_map_entry != _map_index_entry->second.end()) {
                                J.insert(*_belong_set_entry);
                                string strinset;

                                int beforedel = (int)_map_index_entry->second.size();

                                _map_index_entry->second.erase(_map_entry);
                                beforedel -= 1;
                                if(beforedel == 0){
                                    ya[1].map.erase(_map_index_entry);
                                }

                                unordered_map<int, unordered_set<string> >::iterator _map_index_entry2 = ya[0].map.find(*_belong_set_entry);
                                if(_map_index_entry2 != ya[0].map.end()){
                                    if(_map_index_entry2->second.size() == 1 && *(_map_index_entry2->second.begin()) != beta){
                                        strinset = *(_map_index_entry2->second.begin());
                                    }
                                    _map_index_entry2->second.insert(beta);
                                }
                                else{
                                    unordered_set<string> set1;
                                    set1.insert(beta);
                                    ya[0].map.insert(unordered_map<int, unordered_set<string> >::value_type(*_belong_set_entry,set1));
                                }

                                _map_index_entry2 = ya[0].map.find(*_belong_set_entry);
                                if(_map_index_entry2 != ya[0].map.end()){
                                    if(_map_index_entry2->second.size() == 1){
                                        unordered_map<string, unordered_set<int> >::iterator _subpool_entry = subpool.find(beta);
                                        if(_subpool_entry != subpool.end()){
                                            _subpool_entry->second.insert(*_belong_set_entry);
                                        }
                                        else{
                                            unordered_set<int> tempset;
                                            tempset.insert(*_belong_set_entry);
                                            subpool.insert(unordered_map<string, unordered_set<int> >::value_type(beta, tempset));
                                        }
                                    }
                                    else if(_map_index_entry2->second.size() == 2){
                                        unordered_map<string, unordered_set<int> >::iterator _subpool_entry = subpool.find(strinset);
                                        if(_subpool_entry != subpool.end()){
                                            unordered_set<int>::iterator _index_set = _subpool_entry->second.find(*_belong_set_entry);
                                            if(_index_set != _subpool_entry->second.end()){
                                                _subpool_entry->second.erase(_index_set);
                                            }
                                        }
                                        _subpool_entry = subpool.find(beta);
                                        if(_subpool_entry != subpool.end()){
                                            unordered_set<int>::iterator _index_set = _subpool_entry->second.find(*_belong_set_entry);
                                            if(_index_set != _subpool_entry->second.end()){
                                                _subpool_entry->second.erase(_index_set);
                                            }
                                        }
                                    }
                                }

                                //double check if all semi-universal nodes are moved correctly
                                unordered_map<string, int>::iterator _semi_universal_node_finder = ya[1].semi.find(beta);
                                if(_semi_universal_node_finder != ya[1].semi.end()){
                                    string string2 = _semi_universal_node_finder->first;
                                    int num = _semi_universal_node_finder->second;
                                    ya[1].semi.erase(_semi_universal_node_finder);
                                    ya[0].semi.insert(unordered_map<string,int>::value_type(string2,num));
                                }

                                _semi_universal_node_finder = ya[1].semi_potential.find(beta);
                                if(_semi_universal_node_finder != ya[1].semi_potential.end()){
                                    string string2 = _semi_universal_node_finder->first;
                                    int num = _semi_universal_node_finder->second;
                                    ya[1].semi_potential.erase(_semi_universal_node_finder);
                                    ya[0].semi_potential.insert(unordered_map<string,int>::value_type(string2,num));
                                }
                            }
                        }
                    }
                }

                for(unordered_set<int>::iterator _j_entry = J.begin(); _j_entry != J.end(); _j_entry++){
                    unordered_map<int, unordered_set<string> >::iterator _map_entry = ya[1].map.find(*_j_entry);
                    if(_map_entry != ya[1].map.end()){
                        if(_map_entry->second.size() == 1){
                            string beta = *(_map_entry->second.begin());
                            jj.insert(beta);
                            unordered_map<string, unordered_set<int> >::iterator _subpool_entry = subpool.find(beta);
                            if(_subpool_entry != subpool.end()){
                                _subpool_entry->second.insert(_map_entry->first);
                            }
                            else{
                                unordered_set<int> tempset;
                                tempset.insert(_map_entry->first);
                                subpool.insert(unordered_map<string, unordered_set<int> >::value_type(beta, tempset));
                            }
                        }
                    }
                }

                //for each element in the jj set
                //update semi-universal node in corresponding component
                for(unordered_set<string>::iterator _jj_entry = jj.begin(); _jj_entry != jj.end(); _jj_entry++){
                    unordered_map<string, unordered_set<int> >::iterator _subpool_entry = subpool.find(*_jj_entry);
                    if(_subpool_entry != subpool.end()){
                        string string2 = _subpool_entry->first;
                        unordered_map<string, linked_list_entry>::iterator _hpu_entry4 = ya[1].component.find(string2);
                        if(_hpu_entry4 != ya[1].component.end()){
                            if(_hpu_entry4->second.count < _hpu_entry4->second.kl){
                                _hpu_entry4->second.count = (int)_subpool_entry->second.size();
                                if(_hpu_entry4->second.count == _hpu_entry4->second.kl){
                                    ya[1].semi_potential.insert(unordered_map<string, int>::value_type(string2, *(_subpool_entry->second.begin())));
                                }
                            }
                        }
                        else{
                            _hpu_entry4 = ya[0].component.find(string2);
                            if(_hpu_entry4 != ya[0].component.end()){
                                if(_hpu_entry4->second.count < _hpu_entry4->second.kl){
                                    _hpu_entry4->second.count = (int)_subpool_entry->second.size();
                                    if(_hpu_entry4->second.count == _hpu_entry4->second.kl){
                                        ya[0].semi_potential.insert(unordered_map<string, int>::value_type(string2, *(_subpool_entry->second.begin())));
                                    }
                                }
                            }
                        }
                    }
                }

                //ya[0] is the smaller one and the ya[1] is the larger one
                //update component structure y for next iteration

                y->component.clear();
                y->map.clear();
                y->semi.clear();
                y->semi_potential.clear();
                y->candidate.clear();

                unordered_map<string, linked_list_entry>::iterator _hpu_entry_in_first_component = ya[0].component.find(l);
                unordered_map<string, linked_list_entry>::iterator _hpu_entry_in_second_component = ya[1].component.find(l);
                if(_hpu_entry_in_first_component != ya[0].component.end() && _hpu_entry_in_second_component == ya[1].component.end()) {
                    if(!ya[1].semi_potential.empty()){
                        if(ya[1].semi.empty()){
                            ya[1].treep = node_potential;
                            Y obj;
                            Q_potential.push_back(obj);
                            swap(Q_potential.back(), ya[1]);
                        }
                        else{
                            ya[1].treep = node;
                            Y obj;
                            Q.push_back(obj);
                            swap(Q.back(), ya[1]);
                        }
                    }
                    else{
                        if(!ya[1].semi.empty()){
                            ya[1].treep = node;
                            Y obj;
                            Q.push_back(obj);
                            swap(Q.back(), ya[1]);
                        }
                        else{
                            ya[1].treep = node;
                            Y obj;
                            Q_potential.push_back(obj);
                        }
                    }

                    swap(y->component, ya[0].component);
                    swap(y->map, ya[0].map);
                    swap(y->semi, ya[0].semi);
                    swap(y->semi_potential, ya[0].semi_potential);
                    y->sum_kl = ya[0].sum_kl;
                    y->treep = node;
                    swap(y->candidate, ya[0].candidate);
                }
                else if(_hpu_entry_in_first_component == ya[0].component.end() && _hpu_entry_in_second_component != ya[1].component.end()){
                    if(!ya[0].semi_potential.empty()){
                        if(ya[0].semi.empty()){
                            ya[0].treep = node_potential;
                            Y obj;
                            Q_potential.push_back(obj);
                            swap(Q_potential.back(), ya[0]);
                        }
                        else{
                            ya[0].treep = node;
                            Y obj;
                            Q.push_back(obj);
                            swap(Q.back(), ya[0]);
                        }
                    }
                    else{
                        if(!ya[0].semi.empty()){
                            ya[0].treep = node;
                            Y obj;
                            Q.push_back(obj);
                            swap(Q.back(), ya[0]);
                        }
                        else{
                            ya[0].treep = node;
                            Y obj;
                            Q_potential.push_back(obj);
                        }
                    }

                    swap(y->component, ya[1].component);
                    swap(y->map, ya[1].map);
                    swap(y->semi, ya[1].semi);
                    swap(y->semi_potential, ya[1].semi_potential);
                    y->sum_kl = ya[1].sum_kl;
                    y->treep = node;
                    swap(y->candidate, ya[1].candidate);
                }
                else if(_hpu_entry_in_first_component == ya[0].component.end() && _hpu_entry_in_second_component == ya[1].component.end()){
                    //current semi label's task is over
                    if(!ya[0].semi_potential.empty()){
                        ya[0].treep = node_potential;
                        Y obj;
                        Q_potential.push_back(obj);
                        swap(Q_potential.back(), ya[0]);
                    }

                    if(!ya[1].semi_potential.empty()){
                        ya[1].treep = node_potential;
                        Y obj;
                        Q_potential.push_back(obj);
                        swap(Q_potential.back(), ya[1]);
                    }
                }
            }
        }

    }
    else{
        cout << " component does not have the corresponding label error! " << endl;
        exit(0);
    }
}

FastCompatibilityTest::~FastCompatibilityTest() {
    stack<dis_Node *> *tempStack = new stack<dis_Node *>();
    if(dis_graph != nullptr){
        if(dis_graph->children[0] != nullptr){
            tempStack->push(dis_graph->children[0]);
        }
    }

    while(!tempStack->empty()){
        dis_Node *temptr = tempStack->top();
        tempStack->pop();
        for(vector<dis_Node *>::iterator _child_entry = temptr->children.begin(); _child_entry != temptr->children.end();
            _child_entry++){
            tempStack->push(*_child_entry);
        }
        delete temptr;
    }
    delete tempStack;
}