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
// Detailed implementation of HDT.h
//

#include "HDT.h"

using namespace std;

random_device index_rd;
mt19937_64 index_mt(index_rd());

HDT::HDT(){ }

HDT::HDT(vector<occ_Node> et_seq, unordered_map<string, array_Node> array, unordered_map<int, unordered_set<graph_Edge, hash_Edge> > t_edge,
         unordered_map<int, unordered_set<graph_Edge, hash_Edge> > nt_edge, unordered_map<graph_Edge, int, hash_Edge> edges,
         et_tree_Node* t, unordered_map<string, linked_list_entry> graph, int h, unordered_map<int, unordered_map<string, unordered_set<graph_Edge, hash_Edge> > > query_table,
        bool edge_promotion) {
    et_sequence = et_seq;
    node_array = array;
    tree_edge = t_edge;
    non_tree_edge = nt_edge;
    edge_list = edges;
    treap = t;
    treap_list.insert(unordered_map<string, et_tree_Node*>::value_type(treap->value, treap));
    treap_list_with_level.insert(unordered_map<int, unordered_map<string, et_tree_Node*> >::value_type(0, treap_list));
    hpu = graph;
    if(edge_promotion){
        height = (h - 1);
    }
    else{
        height = (h - h);
    }

    priority_g = treap->node_weight + 1;
    non_tree_edge_query_table = query_table;
}

int HDT::get_edge_level(string str1, string str2) {
    graph_Edge ge;
    int lv = 0;
    if(str1 < str2){
        ge.endp1 = str1;
        ge.endp2 = str2;
    }
    else{
        ge.endp1 = str2;
        ge.endp2 = str1;
    }
    unordered_map<graph_Edge, int, hash_Edge>::iterator _edge_list_finder = edge_list.find(ge);
    if(_edge_list_finder != edge_list.end()){
        lv = _edge_list_finder->second;
    }

    return lv;
}

//This is the main access and core algorithm for HDT
//In order to delete some edge from the original graph, actions will be taken are listed as follows:
//   check if the edge is non-tree edge
//   if the edge is the non-tree edge, delete this edge from the non-tree edge list
//   if this edge is not the non-tree edge, check if this edge is tree edge
//   if this edge is tree edge, we need to:
//   a. split and concatenate the euler tour sequence
//   b. update the corresponding treaps accordingly
//   c. find the replacement edge, and then update the information related to that edge
//   if the edge is neither non-tree edge nor tree edge, we do nothing
void HDT::delete_Edge(string u, string v, bool fake) {

    graph_Edge edge;
    if(u < v){
        edge.endp1 = u;
        edge.endp2 = v;
    }
    else{
        edge.endp1 = v;
        edge.endp2 = u;
    }
    //search for the edge in the summarized edge list, and record the level that this edge is on
    unordered_map<graph_Edge, int, hash_Edge>::iterator _edge_list_finder = edge_list.find(edge);
    if(_edge_list_finder != edge_list.end()) {
        int edge_level = _edge_list_finder->second;

        edge_list.erase(_edge_list_finder);
        //then we need to discuss if this edge is tree edge or not
        bool _is_tree_or_nontree_edge = false;
        //check if this edge is non-tree edge
        unordered_map<int, unordered_set<graph_Edge, hash_Edge> >::iterator _non_tree_edge_finder = non_tree_edge.find(edge_level);
        if (_non_tree_edge_finder != non_tree_edge.end()) {
            //it is a non-tree edge in level of edge_level
            unordered_set<graph_Edge, hash_Edge>::iterator _non_tree_edge_entry_finder = _non_tree_edge_finder->second.find(edge);
            if (_non_tree_edge_entry_finder != _non_tree_edge_finder->second.end()) {

                //and we find it. and we delete it
                _is_tree_or_nontree_edge = true;
                _non_tree_edge_finder->second.erase(_non_tree_edge_entry_finder);
                //update the connection inofrmation in the node array
                unordered_map<string, array_Node>::iterator _node_array_finder = node_array.find(u);
                if (_node_array_finder != node_array.end()) {
                    graph_Edge gp;
                    gp.endp1 = u;
                    gp.endp2 = v;
                    unordered_map<int, unordered_map<string, unordered_set<graph_Edge, hash_Edge> > >::iterator query_table_entry = non_tree_edge_query_table.find(edge_level);
                    if(query_table_entry != non_tree_edge_query_table.end()){
                        unordered_map<string, unordered_set<graph_Edge, hash_Edge> >::iterator table_entry = query_table_entry->second.find(gp.endp1);
                        if(table_entry != query_table_entry->second.end()){
                            unordered_set<graph_Edge, hash_Edge>::iterator edge_entry = table_entry->second.find(gp);
                            if(edge_entry != table_entry->second.end()){
                                table_entry->second.erase(edge_entry);
                            }
                        }
                    }
                }
                _node_array_finder = node_array.find(v);
                if (_node_array_finder != node_array.end()) {
                    graph_Edge gp;
                    gp.endp1 = v;
                    gp.endp2 = u;
                    unordered_map<int, unordered_map<string, unordered_set<graph_Edge, hash_Edge> > >::iterator query_table_entry = non_tree_edge_query_table.find(edge_level);
                    if(query_table_entry != non_tree_edge_query_table.end()){
                        unordered_map<string, unordered_set<graph_Edge, hash_Edge> >::iterator table_entry = query_table_entry->second.find(gp.endp1);
                        if(table_entry != query_table_entry->second.end()){
                            unordered_set<graph_Edge, hash_Edge>::iterator edge_entry = table_entry->second.find(gp);
                            if(edge_entry != table_entry->second.end()){
                                table_entry->second.erase(edge_entry);
                            }
                        }
                    }
                }
                if (!fake) {
                    delete_hpu_edge(u, v);
                    delete_hpu_edge(v, u);
                }
            }
        }
        if(!_is_tree_or_nontree_edge){
            //check if the edge is a tree edge
            unordered_map<int, unordered_set<graph_Edge, hash_Edge> >::iterator _tree_edge_finder = tree_edge.find(edge_level);
            if (_tree_edge_finder != tree_edge.end()) {
                //it would be the tree edge
                unordered_set<graph_Edge, hash_Edge>::iterator _tree_edge_entry_finder = _tree_edge_finder->second.find(edge);
                if (_tree_edge_entry_finder != _tree_edge_finder->second.end()) {


                    //we find it
                    _tree_edge_finder->second.erase(_tree_edge_entry_finder);
                    if (!fake) {
                        delete_hpu_edge(u, v);
                        delete_hpu_edge(v, u);
                    }
                    t1 = NULL;
                    t2 = NULL;
                    //update treap
                    for(int j = edge_level; j >= 0; j--){
                        split_treap(u, v, j);
                        vector<et_tree_Node*> treap_vec;
                        treap_vec.push_back(t1);
                        treap_vec.push_back(t2);
                        unordered_map<int, vector<et_tree_Node*> >::iterator treap_entry = current_treaps.find(j);
                        if(treap_entry != current_treaps.end()){
                            current_treaps.erase(treap_entry);
                        }
                        current_treaps.insert(unordered_map<int, vector<et_tree_Node*> >::value_type(j, treap_vec));
                    }

                    graph_Edge ge;
                    ge.endp1 = " ";
                    ge.endp2 = " ";
                    int count = 0;
                    unordered_map<int, vector<et_tree_Node *> >::iterator treap_entry;

                    for(int j = edge_level; j >= 0; j--) {
                        count = j;
                        if (height > 0) {
                            increase_level_of_smaller_tree(j, edge_level);
                        }
                        treap_entry = current_treaps.find(j);
                        if (treap_entry != current_treaps.end()) {
                            t1 = treap_entry->second[0];
                            t2 = treap_entry->second[1];

                            if (t1->node_weight < t2->node_weight) {
                                if (t1 != NULL) {
                                    if (connected(t1->value, u, j)) {
                                        ge = find_replace(j, t1, u, v, t2);
                                    } else if (connected(t1->value, v, j)) {
                                        ge = find_replace(j, t1, v, u, t2);
                                    }
                                }
                            } else {
                                if (t2 != NULL) {
                                    if (connected(t2->value, u, j)) {
                                        ge = find_replace(j, t2, u, v, t1);
                                    } else if (connected(t2->value, v, j)) {
                                        ge = find_replace(j, t2, v, u, t1);
                                    }
                                }
                            }
                            if (ge.endp1 != " " && ge.endp2 != " ") {
                                break;
                            }
                        }
                    }

                    if (ge.endp1 != " " && ge.endp2 != " ") {
                        //delete that edge from the level i non-tree edge
                        graph_Edge eg;
                        if (ge.endp1 < ge.endp2) {
                            eg.endp1 = ge.endp1;
                            eg.endp2 = ge.endp2;
                        } else {
                            eg.endp1 = ge.endp2;
                            eg.endp2 = ge.endp1;
                        }


                        moved_edge.insert(eg);
                        //we need to do more here
                        //delete
                        for(int j = edge_level; j >= 0; j--){
                            treap_entry = current_treaps.find(j);
                            if(treap_entry != current_treaps.end()){
                                t1 = treap_entry->second[0];
                                t2 = treap_entry->second[1];
                                if(j > count){
                                    unordered_map<int, unordered_map<string, et_tree_Node*> >::iterator level_list_entry = treap_list_with_level.find(j);
                                    if(level_list_entry != treap_list_with_level.end()) {
                                        level_list_entry->second.insert(unordered_map<string, et_tree_Node *>::value_type(t1->value, t1));
                                        level_list_entry->second.insert(unordered_map<string, et_tree_Node *>::value_type(t2->value, t2));
                                    }
                                    else{
                                        unordered_map<string, et_tree_Node*> temp_map;
                                        temp_map.insert(unordered_map<string, et_tree_Node*>::value_type(t1->value, t1));
                                        temp_map.insert(unordered_map<string, et_tree_Node*>::value_type(t2->value, t2));
                                        treap_list_with_level.insert(unordered_map<int, unordered_map<string, et_tree_Node*> >::iterator::value_type(j, temp_map));
                                    }
                                }
                                else{
                                    if (connected(ge.endp1, t1->value, j)) {
                                        rerooting(ge.endp1, t1, j);
                                        rerooting(ge.endp2, t2, j);
                                    }
                                    else if(connected(ge.endp1, t2->value, j)) {
                                        rerooting(ge.endp1, t2, j);
                                        rerooting(ge.endp2, t1, j);
                                    }

                                    et_tree_Node *lm = find_leftmost(t1);
                                    et_tree_Node *newnode = new et_tree_Node();
                                    backUpNodeSet->insert(newnode);
                                    newnode->node_weight = 1;
                                    newnode->act_child = false;
                                    newnode->active = false;
                                    newnode->l_child = NULL;
                                    newnode->r_child = NULL;
                                    newnode->number = priority_g++;


                                    //generate random priority, edit on jun 22
                                    newnode->priority = ((rand() << 15) + rand()) % 200000000;

                                    newnode->parent = NULL;
                                    newnode->value = lm->value;

                                    t1 = union_link(t1, t2, j);
                                    t1 = union_link(t1, newnode, j);
                                    unordered_map<int, unordered_map<string, et_tree_Node*> >::iterator level_list_entry = treap_list_with_level.find(j);
                                    if(level_list_entry != treap_list_with_level.end()){
                                        level_list_entry->second.insert(unordered_map<string, et_tree_Node *>::value_type(t1->value, t1));
                                    }
                                    else{
                                        unordered_map<string, et_tree_Node*> temp_map;
                                        temp_map.insert(unordered_map<string, et_tree_Node *>::value_type(t1->value, t1));
                                        treap_list_with_level.insert(unordered_map<int, unordered_map<string, et_tree_Node*> >::value_type(j, temp_map));
                                    }
                                }
                            }
                        }
                    }
                    else {
                        for(int j = edge_level; j >= 0; j--){
                            treap_entry = current_treaps.find(j);
                            if(treap_entry != current_treaps.end()){
                                t1 = treap_entry->second[0];
                                t2 = treap_entry->second[1];
                                unordered_map<int, unordered_map<string, et_tree_Node*> >::iterator level_list_entry = treap_list_with_level.find(j);
                                if(level_list_entry != treap_list_with_level.end()) {
                                    level_list_entry->second.insert(unordered_map<string, et_tree_Node *>::value_type(t1->value, t1));
                                    level_list_entry->second.insert(unordered_map<string, et_tree_Node *>::value_type(t2->value, t2));
                                }
                                else{
                                    unordered_map<string, et_tree_Node*> temp_map;
                                    temp_map.insert(unordered_map<string, et_tree_Node*>::value_type(t1->value, t1));
                                    temp_map.insert(unordered_map<string, et_tree_Node*>::value_type(t2->value, t2));
                                    treap_list_with_level.insert(unordered_map<int, unordered_map<string, et_tree_Node*> >::iterator::value_type(j, temp_map));
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    else{
        cout << "There is no edge in the list" << endl;
    }
    t1 = NULL;
    t2 = NULL;
    current_treaps.clear();
}

//given two strings u and v, delete the linked-list entry in each adjacent list
void HDT::delete_hpu_edge(string u, string v) {
    unordered_map<string, linked_list_entry>::iterator _hpu_finder = hpu.find(u);
    if(_hpu_finder != hpu.end()){
        unordered_map<string, linked_list_entry>::iterator _ajcn_list_finder = _hpu_finder->second.ajcn_list.find(v);
        if(_ajcn_list_finder != _hpu_finder->second.ajcn_list.end()){
            _hpu_finder->second.ajcn_list.erase(_ajcn_list_finder);
        }
    }
}

graph_Edge HDT::find_replace(int level, et_tree_Node *ct, string u, string v, et_tree_Node *ct2) {

    //cannot delete
    graph_Edge edge;
    edge.endp1 = " ";
    edge.endp2 = " ";
    et_tree_Node *root = ct;
    unordered_set<string> visited_label;
    queue<et_tree_Node *> _euler_tour_entry_queue;
    if (root != NULL && (root->act_child || root->active)) {
        _euler_tour_entry_queue.push(root);

    }
    while (!(_euler_tour_entry_queue.empty())) {
        et_tree_Node* _tree_root = _euler_tour_entry_queue.front();
        _euler_tour_entry_queue.pop();

        if (_tree_root->active) {
            unordered_set<string>::iterator visited_label_entry = visited_label.find(_tree_root->value);
            if(visited_label_entry == visited_label.end()){
                visited_label.insert(_tree_root->value);
                unordered_map<int, unordered_map<string, unordered_set<graph_Edge, hash_Edge> > >::iterator query_table_entry = non_tree_edge_query_table.find(level);
                if(query_table_entry != non_tree_edge_query_table.end()){
                    unordered_map<string, unordered_set<graph_Edge, hash_Edge> >::iterator edge_entry = query_table_entry->second.find(_tree_root->value);
                    if(edge_entry != query_table_entry->second.end()){
                        for(unordered_set<graph_Edge, hash_Edge>::iterator _non_tree_edge_entry_finder = edge_entry->second.begin();
                            _non_tree_edge_entry_finder != edge_entry->second.end();){

                            if(connected(ct2->value, (*_non_tree_edge_entry_finder).endp2, level) && connected(ct->value, (*_non_tree_edge_entry_finder).endp1, level)) {
                                edge.endp1 = (*_non_tree_edge_entry_finder).endp1;
                                edge.endp2 = (*_non_tree_edge_entry_finder).endp2;

                                _non_tree_edge_entry_finder = edge_entry->second.erase(_non_tree_edge_entry_finder);
                                graph_Edge eg;
                                eg.endp1 = edge.endp2;
                                eg.endp2 = edge.endp1;
                                unordered_map<string, unordered_set<graph_Edge, hash_Edge> >::iterator edge_entry2 = query_table_entry->second.find(edge.endp2);
                                if(edge_entry2 != query_table_entry->second.end()){
                                    unordered_set<graph_Edge, hash_Edge>::iterator _non_tree_edge_entry_finder2 = edge_entry2->second.find(eg);
                                    if(_non_tree_edge_entry_finder2 != edge_entry2->second.end()){
                                        edge_entry2->second.erase(_non_tree_edge_entry_finder2);
                                        graph_Edge gee;
                                        if(eg.endp1 < eg.endp2){
                                            gee.endp1 = eg.endp1;
                                            gee.endp2 = eg.endp2;
                                        }
                                        else{
                                            gee.endp2 = eg.endp1;
                                            gee.endp1 = eg.endp2;
                                        }
                                        unordered_map<int, unordered_set<graph_Edge, hash_Edge> >::iterator _non_tree_edge_finder3 = non_tree_edge.find(
                                                level);
                                        if (_non_tree_edge_finder3 != non_tree_edge.end()) {
                                            unordered_set<graph_Edge, hash_Edge>::iterator _non_tree_edge_entry_finder3 = _non_tree_edge_finder3->second.find(gee);
                                            if (_non_tree_edge_entry_finder3 != _non_tree_edge_finder3->second.end()) {
                                                _non_tree_edge_finder3->second.erase(_non_tree_edge_entry_finder3);
                                                //add that edge into the level 0 tree edge
                                                unordered_map<int, unordered_set<graph_Edge, hash_Edge> >::iterator _tree_edge_finder = tree_edge.find(
                                                        level);
                                                if (_tree_edge_finder != tree_edge.end()) {
                                                    _tree_edge_finder->second.insert(gee);
                                                    //add this edge into edge list
                                                    unordered_map<graph_Edge, int, hash_Edge>::iterator _edge_list_finder = edge_list.find(gee);
                                                    if (_edge_list_finder != edge_list.end()) {
                                                        edge_list.erase(_edge_list_finder);
                                                        edge_list.insert(unordered_map<graph_Edge, int, hash_Edge>::value_type(gee, level));
                                                        if(edge.endp1 != " " && edge.endp2 != " "){

                                                            return edge;
                                                        }
                                                    }
                                                }
                                                else {
                                                    unordered_set<graph_Edge, hash_Edge> tempset;
                                                    tempset.insert(gee);
                                                    tree_edge.insert(
                                                            unordered_map<int, unordered_set<graph_Edge, hash_Edge> >::value_type(level,
                                                                                                                                  tempset));
                                                    //add this edge into edge list
                                                    unordered_map<graph_Edge, int, hash_Edge>::iterator _edge_list_finder = edge_list.find(gee);
                                                    if (_edge_list_finder != edge_list.end()) {
                                                        edge_list.erase(_edge_list_finder);
                                                        edge_list.insert(unordered_map<graph_Edge, int, hash_Edge>::value_type(gee, level));
                                                        if(edge.endp1 != " " && edge.endp2 != " "){

                                                            return edge;
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                            else if(level < height && connected(ct->value, (*_non_tree_edge_entry_finder).endp2, level) && connected(ct->value, (*_non_tree_edge_entry_finder).endp1, level)){
                                edge.endp1 = (*_non_tree_edge_entry_finder).endp1;
                                edge.endp2 = (*_non_tree_edge_entry_finder).endp2;
                                int lv = 0;
                                lv = level + 1;
                                if(lv > height){
                                    lv = height;
                                }
                                _non_tree_edge_entry_finder = edge_entry->second.erase(_non_tree_edge_entry_finder);
                                graph_Edge eg;
                                eg.endp1 = edge.endp2;
                                eg.endp2 = edge.endp1;
                                unordered_map<string, unordered_set<graph_Edge, hash_Edge> >::iterator edge_entry2 = query_table_entry->second.find(edge.endp2);
                                if(edge_entry2 != query_table_entry->second.end()) {
                                    unordered_set<graph_Edge, hash_Edge>::iterator _non_tree_edge_entry_finder2 = edge_entry2->second.find(
                                            eg);
                                    if (_non_tree_edge_entry_finder2 != edge_entry2->second.end()) {
                                        edge_entry2->second.erase(_non_tree_edge_entry_finder2);
                                        graph_Edge gee;
                                        if(eg.endp1 < eg.endp2){
                                            gee.endp1 = eg.endp1;
                                            gee.endp2 = eg.endp2;
                                        }
                                        else{
                                            gee.endp2 = eg.endp1;
                                            gee.endp1 = eg.endp2;
                                        }
                                        unordered_map<int, unordered_set<graph_Edge, hash_Edge> >::iterator _non_tree_edge_finder3 = non_tree_edge.find(
                                                level);
                                        if (_non_tree_edge_finder3 != non_tree_edge.end()) {
                                            unordered_set<graph_Edge, hash_Edge>::iterator _non_tree_edge_entry_finder3 = _non_tree_edge_finder3->second.find(
                                                    gee);
                                            if (_non_tree_edge_entry_finder3 != _non_tree_edge_finder3->second.end()) {
                                                _non_tree_edge_finder3->second.erase(_non_tree_edge_entry_finder3);
                                            }
                                        }
                                    }
                                }
                                unordered_map<int, unordered_map<string, unordered_set<graph_Edge, hash_Edge> > >::iterator _query_table_level_entry = non_tree_edge_query_table.find(lv);
                                if(_query_table_level_entry != non_tree_edge_query_table.end()){
                                    unordered_map<string, unordered_set<graph_Edge, hash_Edge> >::iterator _label_set_entry = _query_table_level_entry->second.find(edge.endp1);
                                    if(_label_set_entry != _query_table_level_entry->second.end()){
                                        _label_set_entry->second.insert(edge);
                                    }
                                    else{
                                        unordered_set<graph_Edge, hash_Edge> tempset;
                                        tempset.insert(edge);
                                        _query_table_level_entry->second.insert(unordered_map<string, unordered_set<graph_Edge, hash_Edge>>::value_type(edge.endp1, tempset));
                                    }

                                    _label_set_entry = _query_table_level_entry->second.find(eg.endp1);
                                    if(_label_set_entry != _query_table_level_entry->second.end()){
                                        _label_set_entry->second.insert(eg);
                                    }
                                    else{
                                        unordered_set<graph_Edge, hash_Edge> tempset;
                                        tempset.insert(eg);
                                        _query_table_level_entry->second.insert(unordered_map<string, unordered_set<graph_Edge, hash_Edge>>::value_type(eg.endp1, tempset));
                                    }
                                }
                                else{
                                    unordered_set<graph_Edge, hash_Edge> tempset1;
                                    tempset1.insert(edge);
                                    unordered_set<graph_Edge, hash_Edge> tempset2;
                                    tempset2.insert(eg);
                                    unordered_map<string, unordered_set<graph_Edge, hash_Edge> > tempmap;
                                    tempmap.insert(unordered_map<string, unordered_set<graph_Edge, hash_Edge> >::value_type(edge.endp1, tempset1));
                                    tempmap.insert(unordered_map<string, unordered_set<graph_Edge, hash_Edge> >::value_type(eg.endp1, tempset2));
                                    non_tree_edge_query_table.insert(unordered_map<int, unordered_map<string, unordered_set<graph_Edge, hash_Edge> > >::value_type(lv, tempmap));
                                }

                                graph_Edge gee;
                                if(eg.endp1 < eg.endp2){
                                    gee.endp1 = eg.endp1;
                                    gee.endp2 = eg.endp2;
                                }
                                else{
                                    gee.endp2 = eg.endp1;
                                    gee.endp1 = eg.endp2;
                                }
                                unordered_map<int, unordered_set<graph_Edge, hash_Edge> >::iterator _level_set_entry = non_tree_edge.find(lv);
                                if(_level_set_entry != non_tree_edge.end()){
                                    _level_set_entry->second.insert(gee);
                                }
                                else{
                                    unordered_set<graph_Edge, hash_Edge> tempset;
                                    tempset.insert(gee);
                                    non_tree_edge.insert(unordered_map<int, unordered_set<graph_Edge, hash_Edge> >::value_type(lv, tempset));
                                }

                                unordered_map<graph_Edge, int, hash_Edge>::iterator edge_list_entry = edge_list.find(gee);
                                if(edge_list_entry != edge_list.end()){
                                    edge_list.erase(edge_list_entry);
                                    edge_list.insert(unordered_map<graph_Edge, int, hash_Edge>::value_type(gee, lv));
                                }
                            }
                            else{
                                _non_tree_edge_entry_finder++;
                            }
                        }
                        edge.endp1 = " ";
                        edge.endp2 = " ";
                    }
                }
            }
        }
        //push the child into the queue for further process
        if (_tree_root->r_child != NULL && (_tree_root->r_child->act_child || _tree_root->r_child->active))
            _euler_tour_entry_queue.push(_tree_root->r_child);
        if (_tree_root->l_child != NULL && (_tree_root->l_child->act_child || _tree_root->l_child->active))
            _euler_tour_entry_queue.push(_tree_root->l_child);
    }

    return edge;
}

void HDT::increase_level_of_smaller_tree(int level, int e_level) {
    et_tree_Node* tree_to_increase = NULL;
    unordered_map<int, vector<et_tree_Node*> >::iterator current_treap_entry = current_treaps.find(level);
    if(current_treap_entry != current_treaps.end()){
        t1 = current_treap_entry->second[0];
        t2 = current_treap_entry->second[1];
        if(t1->node_weight < t2->node_weight){
            tree_to_increase = t1;
        }
        else{
            tree_to_increase = t2;
        }

        queue<et_tree_Node*> node_queue;
        unordered_set<string> _node_visited;
        unordered_set<graph_Edge,hash_Edge> _edge_visited;
        if(tree_to_increase != NULL){
            node_queue.push(tree_to_increase);
            while(!(node_queue.empty())){
                et_tree_Node* temp = node_queue.front();
                node_queue.pop();
                if(temp->l_child != NULL){
                    node_queue.push(temp->l_child);
                }
                if(temp->r_child != NULL){
                    node_queue.push(temp->r_child);
                }
                unordered_set<string>::iterator _vidited_node_finder = _node_visited.find(temp->value);
                if(_vidited_node_finder != _node_visited.end()){
                    continue;
                }
                else{
                    _node_visited.insert(temp->value);
                    graph_Edge edge;
                    string end1 = "";
                    string end2 = "";
                    unordered_map<string, linked_list_entry>::iterator _hpu_entry_finder = hpu.find(temp->value);
                    if(_hpu_entry_finder != hpu.end()){
                        for(unordered_map<string, linked_list_entry>::iterator _ajcn_list_finder = _hpu_entry_finder->second.ajcn_list.begin();
                            _ajcn_list_finder != _hpu_entry_finder->second.ajcn_list.end(); _ajcn_list_finder++){
                            graph_Edge edge;
                            if(temp->value < _ajcn_list_finder->first){
                                end1 = temp->value;
                                end2 = _ajcn_list_finder->first;
                                edge.endp1 = end1;
                                edge.endp2 = end2;
                            }
                            else{
                                end2 = temp->value;
                                end1 = _ajcn_list_finder->first;
                                edge.endp1 = end2;
                                edge.endp2 = end1;
                            }
                            unordered_map<graph_Edge, int, hash_Edge>::iterator edge_level_entry = edge_list.find(edge);
                            if(edge_level_entry != edge_list.end()){
                                if(edge_level_entry->second == level){
                                    if(connected(end1,end2, level)){
                                        //if they are in the same component
                                        graph_Edge edge1;
                                        edge1.endp1 = end1;
                                        edge1.endp2 = end2;
                                        unordered_set<graph_Edge, hash_Edge>::iterator _visited_edge_finder = _edge_visited.find(edge1);
                                        if(_visited_edge_finder != _edge_visited.end()){
                                            continue;
                                        }
                                        //increase edge list
                                        unordered_map<graph_Edge, int, hash_Edge>::iterator _edge_finder = edge_list.find(edge1);
                                        if(_edge_finder != edge_list.end()){
                                            graph_Edge edge2;
                                            edge2.endp1 = _edge_finder->first.endp1;
                                            edge2.endp2 = _edge_finder->first.endp2;
                                            int lv = _edge_finder->second;
                                            int before_increase = lv;
                                            if(lv == level && lv < (e_level + 1)){
                                                lv = lv + 1;
                                            }
                                            else{
                                                continue;
                                            }
                                            if(lv > height){
                                                lv = height;
                                            }
                                            if(lv == before_increase){
                                                continue;
                                            }
                                            edge_list.erase(_edge_finder);
                                            edge_list.insert(unordered_map<graph_Edge, int, hash_Edge>::value_type(edge2, lv));
                                            //increase tree edge
                                            unordered_map<int, unordered_set<graph_Edge, hash_Edge> >::iterator _tree_edge_finder = tree_edge.find(level);
                                            if(_tree_edge_finder != tree_edge.end()){
                                                unordered_set<graph_Edge, hash_Edge>::iterator _tree_edge_entry_finder = _tree_edge_finder->second.find(edge1);
                                                if(_tree_edge_entry_finder != _tree_edge_finder->second.end()){
                                                    _tree_edge_finder->second.erase(_tree_edge_entry_finder);
                                                    lv = level + 1;
                                                    if(lv > height){
                                                        lv = height;
                                                    }
                                                    unordered_map<int, unordered_set<graph_Edge, hash_Edge> >::iterator _tree_edge_finder2 = tree_edge.find(lv);
                                                    if(_tree_edge_finder2 != tree_edge.end()){
                                                        _tree_edge_finder2->second.insert(edge1);
                                                    }
                                                    else{
                                                        unordered_set<graph_Edge, hash_Edge> tempset;
                                                        tempset.insert(edge1);
                                                        tree_edge.insert(unordered_map<int, unordered_set<graph_Edge, hash_Edge> >::value_type(lv, tempset));
                                                    }
                                                    _edge_visited.insert(edge1);
                                                    unordered_map<int, unordered_map<string, et_tree_Node*> >::iterator lv_exist_checker = treap_list_with_level.find((lv));
                                                    if(lv_exist_checker == treap_list_with_level.end()){
                                                        unordered_map<string, et_tree_Node*> temp_map;
                                                        treap_list_with_level.insert(unordered_map<int, unordered_map<string, et_tree_Node*> >::value_type(lv, temp_map));
                                                    }

                                                    unordered_map<string, array_Node>::iterator label_occur1 = node_array.find(edge2.endp1);
                                                    unordered_map<string, array_Node>::iterator label_occur2 = node_array.find(edge2.endp2);
                                                    if(label_occur1 != node_array.end() && label_occur2 != node_array.end()){
                                                        unordered_map<int, et_tree_Node*>::iterator level_occur_entry1 = label_occur1->second.act_Node1.find(lv);
                                                        unordered_map<int, et_tree_Node*>::iterator level_occur_entry2 = label_occur2->second.act_Node1.find(lv);
                                                        if(level_occur_entry1 != label_occur1->second.act_Node1.end() &&
                                                           level_occur_entry2 != label_occur2->second.act_Node1.end()){
                                                            et_tree_Node* r1 = find_root(level_occur_entry1->second);
                                                            et_tree_Node* r2 = find_root(level_occur_entry2->second);
                                                            unordered_map<int, unordered_map<string, et_tree_Node*> >::iterator level_treap_entry = treap_list_with_level.find(lv);
                                                            if(level_treap_entry != treap_list_with_level.end()){
                                                                unordered_map<string, et_tree_Node*>::iterator label_treap_entry = level_treap_entry->second.find(r1->value);
                                                                if(label_treap_entry != level_treap_entry->second.end()){
                                                                    level_treap_entry->second.erase(label_treap_entry);
                                                                }
                                                                label_treap_entry = level_treap_entry->second.find(r2->value);
                                                                if(label_treap_entry != level_treap_entry->second.end()){
                                                                    level_treap_entry->second.erase(label_treap_entry);
                                                                }
                                                                rerooting(edge2.endp1, r1, lv);
                                                                rerooting(edge2.endp2, r2, lv);
                                                                et_tree_Node *lm = find_leftmost(r1);
                                                                et_tree_Node *newnode = new et_tree_Node();
                                                                backUpNodeSet->insert(newnode);
                                                                newnode->node_weight = 1;
                                                                newnode->act_child = false;
                                                                newnode->active = false;
                                                                newnode->l_child = NULL;
                                                                newnode->r_child = NULL;
                                                                newnode->number = priority_g++;

                                                                //generate random priority, edit on jun 22
                                                                newnode->priority = ((rand() << 15) + rand()) % 200000000;

                                                                newnode->parent = NULL;
                                                                newnode->value = lm->value;
                                                                r1 = union_link(r1, r2, lv);
                                                                r1 = union_link(r1, newnode, lv);
                                                                level_treap_entry->second.insert(unordered_map<string, et_tree_Node*>::value_type(r1->value, r1));
                                                            }
                                                        }
                                                        else if(level_occur_entry1 != label_occur1->second.act_Node1.end() &&
                                                                level_occur_entry2 == label_occur2->second.act_Node1.end()){
                                                            et_tree_Node* r1 = find_root(level_occur_entry1->second);
                                                            unordered_map<int, unordered_map<string, et_tree_Node*> >::iterator level_treap_entry = treap_list_with_level.find(lv);
                                                            if(level_treap_entry != treap_list_with_level.end()){
                                                                unordered_map<string, et_tree_Node*>::iterator label_treap_entry = level_treap_entry->second.find(r1->value);
                                                                if(label_treap_entry != level_treap_entry->second.end()){
                                                                    level_treap_entry->second.erase(label_treap_entry);
                                                                }
                                                                unordered_map<int, et_tree_Node*>::iterator level_occur_entry3 = label_occur2->second.act_Node1.find(lv - 1);
                                                                if(level_occur_entry3 != label_occur2->second.act_Node1.end()){
                                                                    et_tree_Node* r2 = level_occur_entry3->second;
                                                                    et_tree_Node* newnode = new et_tree_Node();
                                                                    backUpNodeSet->insert(newnode);
                                                                    newnode->node_weight = 1;
                                                                    newnode->act_child = false;
                                                                    if(r2->active){
                                                                        newnode->active = true;
                                                                    }
                                                                    else{
                                                                        newnode->active = false;
                                                                    }
                                                                    newnode->l_child = NULL;
                                                                    newnode->r_child = NULL;
                                                                    newnode->number = priority_g++;

                                                                    //generate random priority, edit on Jun 22
                                                                    newnode->priority = ((rand() << 15) + rand()) % 200000000;

                                                                    newnode->parent = NULL;
                                                                    newnode->value = r2->value;
                                                                    level_occur_entry2 = label_occur2->second.act_Node1.find(lv);
                                                                    if(level_occur_entry2 == label_occur2->second.act_Node1.end()){
                                                                        label_occur2->second.act_Node1.insert(unordered_map<int, et_tree_Node*>::value_type(lv, newnode));
                                                                    }
                                                                    rerooting(edge2.endp1, r1, lv);
                                                                    et_tree_Node *lm = find_leftmost(r1);
                                                                    et_tree_Node *newnode1 = new et_tree_Node();
                                                                    backUpNodeSet->insert(newnode1);
                                                                    newnode1->node_weight = 1;
                                                                    newnode1->act_child = false;
                                                                    newnode1->active = false;
                                                                    newnode1->l_child = NULL;
                                                                    newnode1->r_child = NULL;
                                                                    newnode1->number = priority_g++;

                                                                    //generate random priority, edit on Jun 22
                                                                    newnode1->priority = ((rand() << 15) + rand()) % 200000000;

                                                                    newnode1->parent = NULL;
                                                                    newnode1->value = lm->value;
                                                                    r1 = union_link(r1, newnode, lv);
                                                                    r1 = union_link(r1, newnode1, lv);
                                                                    level_treap_entry->second.insert(unordered_map<string, et_tree_Node*>::value_type(r1->value, r1));
                                                                }
                                                            }
                                                        }
                                                        else if(level_occur_entry1 == label_occur1->second.act_Node1.end() &&
                                                                level_occur_entry2 != label_occur2->second.act_Node1.end()){
                                                            et_tree_Node* r1 = find_root(level_occur_entry2->second);
                                                            unordered_map<int, unordered_map<string, et_tree_Node*> >::iterator level_treap_entry = treap_list_with_level.find(lv);
                                                            if(level_treap_entry != treap_list_with_level.end()){
                                                                unordered_map<string, et_tree_Node*>::iterator label_treap_entry = level_treap_entry->second.find(r1->value);
                                                                if(label_treap_entry != level_treap_entry->second.end()){
                                                                    level_treap_entry->second.erase(label_treap_entry);
                                                                }
                                                                unordered_map<int, et_tree_Node*>::iterator level_occur_entry3 = label_occur1->second.act_Node1.find(lv - 1);
                                                                if(level_occur_entry3 != label_occur1->second.act_Node1.end()){
                                                                    et_tree_Node* newnode = new et_tree_Node();
                                                                    backUpNodeSet->insert(newnode);
                                                                    newnode->node_weight = 1;
                                                                    newnode->act_child = false;
                                                                    if(level_occur_entry3->second->active){
                                                                        newnode->active = true;
                                                                    }
                                                                    else{
                                                                        newnode->active = false;
                                                                    }
                                                                    newnode->l_child = NULL;
                                                                    newnode->r_child = NULL;
                                                                    newnode->number = priority_g++;

                                                                    //generate random priority, edit on Jun 22
                                                                    newnode->priority = ((rand() << 15) + rand()) % 200000000;

                                                                    newnode->parent = NULL;
                                                                    newnode->value = level_occur_entry3->second->value;
                                                                    level_occur_entry1 = label_occur1->second.act_Node1.find(lv);
                                                                    if(level_occur_entry1 == label_occur1->second.act_Node1.end()){
                                                                        label_occur1->second.act_Node1.insert(unordered_map<int, et_tree_Node*>::value_type(lv, newnode));
                                                                    }
                                                                    rerooting(edge2.endp2, r1, lv);
                                                                    et_tree_Node *lm = find_leftmost(r1);
                                                                    et_tree_Node *newnode1 = new et_tree_Node();
                                                                    backUpNodeSet->insert(newnode1);
                                                                    newnode1->node_weight = 1;
                                                                    newnode1->act_child = false;
                                                                    newnode1->active = false;
                                                                    newnode1->l_child = NULL;
                                                                    newnode1->r_child = NULL;
                                                                    newnode1->number = priority_g++;

                                                                    //generate random priority, edit on Jun 22
                                                                    newnode1->priority = ((rand() << 15) + rand()) % 200000000;

                                                                    newnode1->parent = NULL;
                                                                    newnode1->value = lm->value;
                                                                    r1 = union_link(r1, newnode, lv);
                                                                    r1 = union_link(r1, newnode1, lv);
                                                                    level_treap_entry->second.insert(unordered_map<string, et_tree_Node*>::value_type(r1->value, r1));
                                                                }
                                                            }
                                                        }
                                                        else{
                                                            unordered_map<int, unordered_map<string, et_tree_Node*> >::iterator level_treap_entry = treap_list_with_level.find(lv);
                                                            if(level_treap_entry != treap_list_with_level.end()){
                                                                level_occur_entry1 = label_occur1->second.act_Node1.find(lv - 1);
                                                                level_occur_entry2 = label_occur2->second.act_Node1.find(lv - 1);
                                                                if(level_occur_entry1 != label_occur1->second.act_Node1.end() &&
                                                                   level_occur_entry2 != label_occur2->second.act_Node1.end()){

                                                                    et_tree_Node* newnode1 = new et_tree_Node();
                                                                    backUpNodeSet->insert(newnode1);
                                                                    newnode1->node_weight = 1;
                                                                    newnode1->act_child = false;
                                                                    if(level_occur_entry1->second->active){
                                                                        newnode1->active = true;
                                                                    }
                                                                    else{
                                                                        newnode1->active = false;
                                                                    }
                                                                    newnode1->l_child = NULL;
                                                                    newnode1->r_child = NULL;
                                                                    newnode1->number = priority_g++;

                                                                    //generate random priority, edit on Jun 22
                                                                    newnode1->priority = ((rand() << 15) + rand()) % 200000000;

                                                                    newnode1->parent = NULL;
                                                                    newnode1->value = level_occur_entry1->second->value;

                                                                    et_tree_Node* newnode3 = new et_tree_Node();
                                                                    backUpNodeSet->insert(newnode3);
                                                                    newnode3->node_weight = 1;
                                                                    newnode3->act_child = false;
                                                                    if(level_occur_entry2->second->active){
                                                                        newnode3->active = true;
                                                                    }
                                                                    else{
                                                                        newnode3->active = false;
                                                                    }
                                                                    newnode3->l_child = NULL;
                                                                    newnode3->r_child = NULL;
                                                                    newnode3->number = priority_g++;

                                                                    //generate random priority, edit on Jun 22
                                                                    newnode3->priority = ((rand() << 15) + rand()) % 200000000;

                                                                    newnode3->parent = NULL;
                                                                    newnode3->value = level_occur_entry2->second->value;

                                                                    et_tree_Node* newnode2 = new et_tree_Node();
                                                                    backUpNodeSet->insert(newnode2);
                                                                    newnode2->node_weight = 1;
                                                                    newnode2->act_child = false;
                                                                    newnode2->active = false;
                                                                    newnode2->l_child = NULL;
                                                                    newnode2->r_child = NULL;
                                                                    newnode2->number = priority_g++;


                                                                    //generate random priority, edit on Jun 22
                                                                    newnode2->priority = ((rand() << 15) + rand()) % 200000000;

                                                                    newnode2->parent = NULL;
                                                                    newnode2->value = level_occur_entry1->second->value;

                                                                    level_occur_entry1 = label_occur1->second.act_Node1.find(lv);
                                                                    if(level_occur_entry1 == label_occur1->second.act_Node1.end()){
                                                                        label_occur1->second.act_Node1.insert(unordered_map<int, et_tree_Node*>::value_type(lv, newnode1));
                                                                    }
                                                                    level_occur_entry2 = label_occur2->second.act_Node1.find(lv);
                                                                    if(level_occur_entry2 == label_occur2->second.act_Node1.end()){
                                                                        label_occur2->second.act_Node1.insert(unordered_map<int, et_tree_Node*>::value_type(lv, newnode3));
                                                                    }
                                                                    newnode1 = union_link(newnode1, newnode3, lv);
                                                                    newnode1 = union_link(newnode1, newnode2, lv);
                                                                    level_treap_entry->second.insert(unordered_map<string, et_tree_Node*>::value_type(newnode1->value, newnode1));
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

//find the rightmost node of the treap
et_tree_Node* HDT::find_rightmost(et_tree_Node *tree) {
    et_tree_Node* right = NULL;
    for(et_tree_Node* cur = tree; cur != NULL; cur = cur->r_child){
        right = cur;
    }
    return right;
}

//find the leftmost node of the treap
et_tree_Node* HDT::find_leftmost(et_tree_Node *tree) {
    et_tree_Node* left = NULL;
    for(et_tree_Node* cur = tree; cur != NULL; cur = cur->l_child){
        left = cur;
    }
    return left;
}

//build up a sequence from bottom to up showing the node sequence
vector<et_tree_Node*> HDT::compute_left_right_sequence(int dist, et_tree_Node *node) {
    vector<et_tree_Node*> left_right_sequence((unsigned long)dist);
    int temp_index = dist;
    for(et_tree_Node* temp_node = node; temp_node->parent != NULL; temp_node = temp_node->parent){
        temp_index--;
        left_right_sequence[temp_index] = temp_node;
    }
    return left_right_sequence;
}

//compute the length from the node to root
int HDT::compute_distance_to_root(et_tree_Node *node) {
    int distance = 0;
    for(et_tree_Node* temp_node = node; temp_node->parent != NULL;){
        temp_node = temp_node->parent;
        distance++;
    }
    return distance;
}

//This method modifies the method from Raj D. Iyer and H. S. Rahul's implementation of HDT algorithm
//full citation: R. Iyer, D. Karger, H. Rahul, and M. Thorup. An experimental study of polylogarithmic,
//fully dynamic, connectivity algorithms. J. Exp. Algorithmics, 6, Dec. 2001
//URL:http://people.csail.mit.edu/rahul/blurbs/dyncon-jea2001.html

//given two pointers of two nodes, compute the paths from the nodes to the root
//return the smaller one as the result
bool HDT::check_smaller(et_tree_Node *first_node, et_tree_Node *second_node) {
    if(first_node == NULL || second_node == NULL){
        return false;
    }

    if(first_node == second_node) {
        return false;
    }

    et_tree_Node* temp_first = first_node;
    et_tree_Node* temp_second = second_node;

    int dist_first = compute_distance_to_root(temp_first);
    vector<et_tree_Node*> first_sequence = compute_left_right_sequence(dist_first, first_node);

    int dist_second = compute_distance_to_root(temp_second);
    vector<et_tree_Node*> second_sequence = compute_left_right_sequence(dist_second, second_node);

    int mark = 0;
    for(int i = 0; (i < dist_first) && (i < dist_second); i++){
        if(first_sequence[i] == second_sequence[i]){
            mark++;
        }
        else{
            break;
        }
    }

    bool flag = false;

    if(mark < dist_first){
        if(first_sequence[mark] == first_sequence[mark]->parent->l_child){
            flag = true;
        }
    }
    else{
        if(mark < dist_second){
            if(second_sequence[mark] == second_sequence[mark]->parent->r_child){
                flag = true;
            }
        }
    }

    return flag;
}

//given the position to cut
//this method will cut the treap at the right side of the given postition
//it results two smaller treaps
void HDT::cut_right(et_tree_Node *&tree, et_tree_Node *&ct1, et_tree_Node *&ct2, et_tree_Node*& position) {
    if(position == NULL){
        ct1 = NULL;
        ct2 = NULL;
        return;
    }

    //initialize a dummy node, which will be deleted later
    et_tree_Node* dum = new et_tree_Node();
    backUpNodeSet->insert(dum);
    dum->parent = NULL;
    dum->l_child = NULL;
    dum->r_child = NULL;
    dum->value = "dummy";
    dum->node_weight = 1;
    dum->act_child =false;
    dum->active = false;
    dum->number = -999;

    //find the subtree successor of node position
    et_tree_Node* successor;
    if(position->r_child != NULL){
        for(successor = position->r_child; successor->l_child; successor = successor->l_child);
    }
    else{
        successor = NULL;
    }

    if(successor == NULL){
        position->r_child = dum;
        dum->parent = position;
    }
    else{
        successor->l_child = dum;
        dum->parent = successor;
    }

    //then rotate dummy node to the root

    Treap treap1;
    if(dum->parent != NULL){
        for(et_tree_Node* p = dum->parent; p; p = p->parent){
            if(p->r_child == dum){
                treap1.Treap_Rotate_Left(p);
            }
            else if(p->l_child == dum){
                treap1.Treap_Rotate_Right(p);
            }
        }
    }

    if(dum->l_child != NULL) {
        ct1 = dum->l_child;
    }
    else{
        ct1 = NULL;
    }

    if(dum->r_child != NULL){
        ct2 = dum->r_child;
    }
    else{
        ct2 = NULL;
    }

    if(ct1 != NULL){
        ct1->parent = NULL;
    }
    if(ct2 != NULL){
        ct2->parent = NULL;
    }

    dum->l_child = NULL;
    dum->r_child = NULL;
}

//given the position to cut
//this method will cut the treap at the left side of the given postition
//it results two smaller treaps
void HDT::cut_left(et_tree_Node *&tree, et_tree_Node *&ct1, et_tree_Node *&ct2, et_tree_Node *&position) {
    if(position == NULL){
        ct1 = NULL;
        ct2 = NULL;
        return;
    }

    //initialize a dummy node, which will be deleted later
    et_tree_Node* dum = new et_tree_Node();
    backUpNodeSet->insert(dum);
    dum->l_child = NULL;
    dum->r_child = NULL;
    dum->parent = NULL;
    dum->node_weight = 1;
    dum->act_child = false;
    dum->active = false;
    dum->value = "dummy";
    dum->number = -999;

    //find the subtree successor of node position
    et_tree_Node* pinor;
    if(position->l_child != NULL){
        for(pinor = position->l_child; pinor->r_child; pinor = pinor->r_child);
    }
    else{
        pinor = NULL;
    }

    if(pinor == NULL){
        position->l_child = dum;
        dum->parent = position;
    }
    else{
        pinor->r_child = dum;
        dum->parent = pinor;
    }

    //then rotate dummy node to the root

    Treap treap1;
    if(dum->parent != NULL){
        for(et_tree_Node* p = dum->parent; p; p = p->parent){
            if(p->r_child == dum){
                treap1.Treap_Rotate_Left(p);
            }
            else if(p->l_child == dum){
                treap1.Treap_Rotate_Right(p);
            }
        }
    }

    if(dum->l_child != NULL) {
        ct1 = dum->l_child;
    }
    else{
        ct1 = NULL;
    }

    if(dum->r_child != NULL){
        ct2 = dum->r_child;
    }
    else{
        ct2 = NULL;
    }

    if(ct1 != NULL){
        ct1->parent = NULL;
    }
    if(ct2 != NULL){
        ct2->parent = NULL;
    }

    dum->l_child = NULL;
    dum->r_child = NULL;
}

//cut the treap
void HDT::cut_treap(string node1, string node2, et_tree_Node *&tree, int level_at) {
    unordered_map<string, array_Node>::iterator _node1_entry = node_array.find(node1);
    unordered_map<string, array_Node>::iterator _node2_entry = node_array.find(node2);

    unordered_map<string, unordered_set<et_tree_Node*> >::iterator _node2_occurrence_entry;
    unordered_map<string, unordered_set<et_tree_Node*> >::iterator _node1_occurrence_entry;

    et_tree_Node* e1;
    et_tree_Node* e2;
    et_tree_Node* e3;
    et_tree_Node* e4;

    //find all four occurrences of these two nodes
    if(_node1_entry != node_array.end()){
        unordered_map<int, unordered_map<string, unordered_set<et_tree_Node*> >>::iterator level_occur = _node1_entry->second.node_occr.find(level_at);
        if(level_occur != _node1_entry->second.node_occr.end()){
            _node2_occurrence_entry = level_occur->second.find(node2);
            if(_node2_occurrence_entry != level_occur->second.end()){
                unordered_set<et_tree_Node*>::iterator iterator5 = _node2_occurrence_entry->second.begin();
                e2 = *iterator5;
                iterator5++;
                if(iterator5 != _node2_occurrence_entry->second.end()){
                    e3 = *iterator5;
                }
                else{
                    e3 = NULL;
                }
                level_occur->second.erase(_node2_occurrence_entry);
            }
        }

    }

    if(_node2_entry != node_array.end()){
        unordered_map<int, unordered_map<string, unordered_set<et_tree_Node*> >>::iterator level_occur = _node2_entry->second.node_occr.find(level_at);
        if(level_occur != _node2_entry->second.node_occr.end()){
            _node1_occurrence_entry = level_occur->second.find(node1);
            if(_node1_occurrence_entry != level_occur->second.end()){
                unordered_set<et_tree_Node*>::iterator iterator5 = _node1_occurrence_entry->second.begin();
                e1 = *iterator5;
                iterator5++;
                if(iterator5 != _node1_occurrence_entry->second.end()){
                    e4 = *iterator5;
                }
                else{
                    e4 = NULL;
                }
                level_occur->second.erase(_node1_occurrence_entry);
            }
        }

    }

    //sort these four occurrences from smaller one to larger one
    //which means that the left to right order in the tree

    et_tree_Node* tmp;
    if(e1 != NULL && e4 != NULL){
        if(check_smaller(e4,e1)){
            tmp = e1;
            e1 = e4;
            e4 = tmp;
        }
    }
    else{
        if(e4 != NULL){
            e1 = e4;
            e4 = NULL;
        }
    }

    if(e2 != NULL && e3 != NULL){
        if(check_smaller(e3,e2)){
            tmp = e2;
            e2 = e3;
            e3 = tmp;
        }
    }
    else{
        if(e3 != NULL){
            e2 = e3;
            e3 = NULL;
        }
    }

    et_tree_Node* ct1;
    et_tree_Node* ct2;
    et_tree_Node* ct3;
    et_tree_Node* ct4;

    //cut the treap and return correct sub-treaps
    cut_right(tree, ct1, ct2, e1);
    cut_left(ct2, ct3, ct4, e4);
    if(ct1->node_weight == 1 && ct4->node_weight == 1){
        if(ct1->active){
            t1 = ct1;
        }
        else if(ct4->active){
            t1 = ct4;
        }

        t2 = ct3;
    }
    else if(ct1->node_weight == 1 && ct4->node_weight > 1){
        et_tree_Node* tnode1 = find_leftmost(ct4);
        if(tnode1 != NULL){
            string str_value;
            et_tree_Node* tnode2 = find_rightmost(ct1);
            if(tnode2 != NULL && tnode2->active){
                str_value = tnode1->value;
                tnode1->active = true;
                unordered_map<string, array_Node>::iterator _node_array_entry = node_array.find(str_value);
                if(_node_array_entry != node_array.end()){
                    unordered_map<int, et_tree_Node*>::iterator level_occur_entry = _node_array_entry->second.act_Node1.find(level_at);
                    if(level_occur_entry != _node_array_entry->second.act_Node1.end()){
                        level_occur_entry->second = tnode1;
                    }
                }
                for(et_tree_Node* p = tnode1; p; p = p->parent){
                    if(p->parent != NULL){
                        p->parent->act_child = true;
                    }
                }
            }
        }

        t1 = ct4;
        t2 = ct3;
    }
    else if(ct1->node_weight > 1 && ct4->node_weight == 1){
        et_tree_Node* tnode1 = find_rightmost(ct1);

        if(tnode1 != NULL){
            string str_value;
            et_tree_Node* tnode2 = find_leftmost(ct4);
            if(tnode2 != NULL && tnode2->active){
                str_value = tnode1->value;
                tnode1->active = true;
                unordered_map<string, array_Node>::iterator _node_array_entry = node_array.find(str_value);
                if(_node_array_entry != node_array.end()){
                    unordered_map<int, et_tree_Node*>::iterator level_occur_entry = _node_array_entry->second.act_Node1.find(level_at);
                    if(level_occur_entry != _node_array_entry->second.act_Node1.end()){
                        level_occur_entry->second = tnode1;
                    }
                }
                for(et_tree_Node* p = tnode1; p; p = p->parent){
                    if(p->parent != NULL){
                        p->parent->act_child = true;
                    }
                }
            }
        }

        t1 = ct1;
        t2 = ct3;
    }
    else if(ct1->node_weight > 1 && ct4->node_weight > 1){
        et_tree_Node* tnode1 = find_leftmost(ct4);
        et_tree_Node* st1;
        et_tree_Node* st2;
        cut_right(ct4, st1, st2, tnode1);

        if(st1 != NULL && st1->node_weight == 1){
            string str_value;
            et_tree_Node* tnode2 = find_rightmost(ct1);
            if(st1->active){
                str_value = tnode2->value;
                tnode2->active = true;
                unordered_map<string, array_Node>::iterator _node_array_entry = node_array.find(str_value);
                if(_node_array_entry != node_array.end()){
                    unordered_map<int, et_tree_Node*>::iterator level_occur_entry = _node_array_entry->second.act_Node1.find(level_at);
                    if(level_occur_entry != _node_array_entry->second.act_Node1.end()){
                        level_occur_entry->second = tnode2;
                    }
                }
                for(et_tree_Node* p = tnode2; p; p = p->parent){
                    if(p->parent != NULL){
                        p->parent->act_child = true;
                    }
                }
            }
        }

        et_tree_Node* l = find_leftmost(st2);
        unordered_map<string, array_Node>::iterator _node_array_entry = node_array.find(l->value);
        if(_node_array_entry != node_array.end()){
            unordered_map<int, unordered_map<string, unordered_set<et_tree_Node*> > >::iterator level_occur_entry = _node_array_entry->second.node_occr.find(level_at);
            if(level_occur_entry != _node_array_entry->second.node_occr.end()){
                unordered_map<string, unordered_set<et_tree_Node*> >::iterator _occurrence_entry = level_occur_entry->second.find(st1->value);
                if(_occurrence_entry != level_occur_entry->second.end()){
                    unordered_set<et_tree_Node*>::iterator _occurrence_finder = _occurrence_entry->second.find(st1);
                    if(_occurrence_finder != _occurrence_entry->second.end()){
                        _occurrence_entry->second.erase(_occurrence_finder);
                    }
                }
            }

        }

        _node_array_entry = node_array.find(st1->value);
        if(_node_array_entry != node_array.end()){
            unordered_map<int, unordered_map<string, unordered_set<et_tree_Node*> > >::iterator level_occur_entry = _node_array_entry->second.node_occr.find(level_at);
            if(level_occur_entry != _node_array_entry->second.node_occr.end()){
                unordered_map<string, unordered_set<et_tree_Node*> >::iterator _occurrence_entry = level_occur_entry->second.find(l->value);
                if(_occurrence_entry != level_occur_entry->second.end()){
                    unordered_set<et_tree_Node*>::iterator _occurrence_finder = _occurrence_entry->second.find(l);
                    if(_occurrence_finder != _occurrence_entry->second.end()){
                        _occurrence_entry->second.erase(_occurrence_finder);
                    }
                }
            }
        }

        l = union_link(ct1, st2, level_at);

        t1 = l;
        t2 = ct3;
    }
}

//link two trees
//add a dummy node as the root of the treap, the left and right child are the treaps need to be linked
//then rotate the dummy node to the leaf and then delete it
//most important thing is to update the occurrences of these two nodes in the node array
et_tree_Node* HDT::union_link(et_tree_Node *ct1, et_tree_Node *ct2, int level_at) {
    if(ct1 == NULL && ct2== NULL){
        return NULL;
    }

    if(ct2 == NULL && ct1 != NULL){
        return ct1;
    }

    et_tree_Node* dum = new et_tree_Node();
    backUpNodeSet->insert(dum);
    dum->parent = NULL;
    dum->l_child = ct1;
    dum->r_child = ct2;
    dum->node_weight = ct1->node_weight + ct2->node_weight + 1;
    dum->active = false;
    dum->act_child = (ct1->act_child || ct2->act_child);
    ct1->parent = dum;
    ct2->parent = dum;

    et_tree_Node* lm_ct2 = find_leftmost(ct2);
    et_tree_Node* rm = find_rightmost(ct1);

    unordered_map<string, array_Node>::iterator _node_array_entry = node_array.find(rm->value);
    unordered_map<string, unordered_set<et_tree_Node*> >::iterator _occurrence_finder;
    if(_node_array_entry != node_array.end()){
        unordered_map<int, unordered_map<string, unordered_set<et_tree_Node*> > >::iterator level_occur = _node_array_entry->second.node_occr.find(level_at);
        if(level_occur != _node_array_entry->second.node_occr.end()){
            _occurrence_finder = level_occur->second.find(lm_ct2->value);
            if(_occurrence_finder == level_occur->second.end()){
                string string1 = lm_ct2->value;
                unordered_set<et_tree_Node*> tempset;
                tempset.insert(lm_ct2);
                level_occur->second.insert(unordered_map<string, unordered_set<et_tree_Node*> >::value_type(string1, tempset));
            }
            else{
                _occurrence_finder->second.insert(lm_ct2);
            }
        }
        else{
            string string1 = lm_ct2->value;
            unordered_set<et_tree_Node*> tempset;
            tempset.insert(lm_ct2);
            unordered_map<string, unordered_set<et_tree_Node*> > occur_map;
            occur_map.insert(unordered_map<string, unordered_set<et_tree_Node*>>::value_type(string1, tempset));
            _node_array_entry->second.node_occr.insert(unordered_map<int, unordered_map<string, unordered_set<et_tree_Node*> > >::value_type(level_at, occur_map));
        }
    }

    _node_array_entry = node_array.find(lm_ct2->value);
    if(_node_array_entry != node_array.end()){
        unordered_map<int, unordered_map<string, unordered_set<et_tree_Node*> > >::iterator level_occur = _node_array_entry->second.node_occr.find(level_at);
        if(level_occur != _node_array_entry->second.node_occr.end()){
            _occurrence_finder = level_occur->second.find(rm->value);
            if(_occurrence_finder == level_occur->second.end()){
                string string1 = rm->value;
                unordered_set<et_tree_Node*> tempset;
                tempset.insert(rm);
                level_occur->second.insert(unordered_map<string, unordered_set<et_tree_Node*> >::value_type(string1, tempset));
            }
            else{
                _occurrence_finder->second.insert(rm);
            }
        }
        else{
            string string1 = rm->value;
            unordered_set<et_tree_Node*> tempset;
            tempset.insert(rm);
            unordered_map<string, unordered_set<et_tree_Node*> > occur_map;
            occur_map.insert(unordered_map<string, unordered_set<et_tree_Node*>>::value_type(string1, tempset));
            _node_array_entry->second.node_occr.insert(unordered_map<int, unordered_map<string, unordered_set<et_tree_Node*> > >::value_type(level_at, occur_map));
        }
    }

    Treap treap_obj;
    while(dum->l_child || dum->r_child){
        if(dum->l_child && dum->r_child){
            //left child and right child both exist
            if(dum->l_child->priority > dum->r_child->priority){
                treap_obj.Treap_Rotate_Left(dum);
                dum->node_weight -= 1;
                dum = dum->l_child;
            }
            else{
                treap_obj.Treap_Rotate_Right(dum);
                dum->node_weight -= 1;
                dum = dum->r_child;
            }
        }
        else if(dum->l_child && !dum->r_child){
            treap_obj.Treap_Rotate_Right(dum);
            dum->node_weight -= 1;
            dum = dum->r_child;
        }
        else if(dum->r_child && !dum->l_child){
            treap_obj.Treap_Rotate_Left(dum);
            dum->node_weight -= 1;
            dum = dum->l_child;

        }
    }

    if(dum->parent->l_child == dum){
        dum->parent->l_child = NULL;
        dum->parent = NULL;
    }
    else if(dum->parent->r_child == dum){
        dum->parent->r_child = NULL;
        dum->parent = NULL;
    }

    if(ct1->parent == NULL){
        return ct1;
    }
    else{
        return ct2;
    }
}

//neither one of the given two nodes are the root of the treap
//this method will be called to reroot the treap with one of the node
void HDT::rerooting(string u, et_tree_Node *&tree, int level_at) {

    if(tree->l_child == NULL && tree->r_child == NULL){
        return;
    }

    if(u == find_leftmost(tree)->value){
        return;
    }

    unordered_map<string, array_Node>::iterator _node_array_entry = node_array.find(u);
    //unordered_map<string, unordered_set<et_tree_Node*> >::iterator iterator3;
    et_tree_Node* e1;
    if(_node_array_entry != node_array.end()){
        unordered_map<int, et_tree_Node*>::iterator level_occur = _node_array_entry->second.act_Node1.find(level_at);
        if(level_occur != _node_array_entry->second.act_Node1.end()){
            e1 = level_occur->second;
        }
    }

    et_tree_Node* ct1;
    et_tree_Node* ct2;

    //ct1 is the left tree before e1, ct2 is the right tree including e1
    cut_left(tree, ct1, ct2, e1);
    et_tree_Node* newnode = new et_tree_Node();
    backUpNodeSet->insert(newnode);
    newnode->act_child = false;
    newnode->parent = NULL;
    newnode->active = false;
    newnode->l_child = NULL;
    newnode->node_weight = 1;
    newnode->number = e1->number;
    newnode->r_child = NULL;
    newnode->value = e1->value;
    //delete ct1 from occurrence list of value of ct2
    et_tree_Node* l_m = find_leftmost(ct2);
    et_tree_Node* r_m = find_rightmost(ct1);
    unordered_map<string, array_Node>::iterator _node_array_entry2 = node_array.find(r_m->value);
    unordered_map<string, unordered_set<et_tree_Node*> >::iterator _occurrence_entry;
    if(_node_array_entry2 != node_array.end()){
        unordered_map<int, unordered_map<string, unordered_set<et_tree_Node*> > >::iterator level_occur_entry = _node_array_entry2->second.node_occr.find(level_at);
        if(level_occur_entry != _node_array_entry2->second.node_occr.end()){
            _occurrence_entry = level_occur_entry->second.find(l_m->value);
            if(_occurrence_entry != level_occur_entry->second.end()){
                if(_occurrence_entry->second.size() == 2){
                    unordered_set<et_tree_Node*>::iterator _occurrence = _occurrence_entry->second.find(l_m);
                    if(_occurrence != _occurrence_entry->second.end()){
                        _occurrence_entry->second.erase(_occurrence);
                    }
                }
                else if(_occurrence_entry->second.size() == 1){

                }
                else{
                    cout << "ERROR WITH SIZE" << endl;
                }
            }
        }
    }

    _node_array_entry2 = node_array.find(l_m->value);
    if(_node_array_entry2 != node_array.end()){
        unordered_map<int, unordered_map<string, unordered_set<et_tree_Node*> > >::iterator level_occur_entry = _node_array_entry2->second.node_occr.find(level_at);
        if(level_occur_entry != _node_array_entry2->second.node_occr.end()){
            _occurrence_entry = level_occur_entry->second.find(r_m->value);
            if(_occurrence_entry != level_occur_entry->second.end()){
                unordered_set<et_tree_Node*>::iterator _occurrence = _occurrence_entry->second.find(r_m);
                if(_occurrence != _occurrence_entry->second.end()){
                    _occurrence_entry->second.erase(_occurrence);
                }
            }
        }
    }
    //delete the leftmost node in the ct1 tree
    et_tree_Node* ct3;
    et_tree_Node* leftmost;
    leftmost = find_leftmost(ct1);
    et_tree_Node* rightmost;//find the rightmost node in the ct2 tree
    rightmost = find_rightmost(ct2);
    if(leftmost != NULL){
        _node_array_entry2 = node_array.find(leftmost->value);
        if(_node_array_entry2 != node_array.end()){
            if(leftmost->active){
                rightmost->active = true;
                //edit on Jun 22
                newnode->priority = ((rand() << 15) + rand()) % 200000000;
                unordered_map<int, et_tree_Node*>::iterator lv_occur = _node_array_entry2->second.act_Node1.find(level_at);
                if(lv_occur != _node_array_entry2->second.act_Node1.end()){
                    lv_occur->second = rightmost;
                    for(et_tree_Node* p = rightmost; p; p = p->parent){
                        if(p->parent){
                            p->parent->act_child = true;
                        }
                    }
                }
            }
        }
    }
    cut_right(ct1, ct1, ct3, leftmost);//ct1 is the leftmost node, ct3 is the remaining tree
    if(ct3 != NULL){
        l_m = find_leftmost(ct3);
        _node_array_entry2 = node_array.find(l_m->value);
        if(_node_array_entry2 != node_array.end()){
            unordered_map<int, unordered_map<string, unordered_set<et_tree_Node*> > >::iterator lv_occur = _node_array_entry2->second.node_occr.find(level_at);
            if(lv_occur != _node_array_entry2->second.node_occr.end()){
                _occurrence_entry = lv_occur->second.find(ct1->value);
                if(_occurrence_entry != lv_occur->second.end()){
                    unordered_set<et_tree_Node*>::iterator _occurrence = _occurrence_entry->second.find(ct1);
                    if(_occurrence != _occurrence_entry->second.end()){
                        _occurrence_entry->second.erase(_occurrence);
                    }
                }
            }
        }
        _node_array_entry2 = node_array.find(ct1->value);
        if(_node_array_entry2 != node_array.end()){
            unordered_map<int, unordered_map<string, unordered_set<et_tree_Node*> > >::iterator lv_occur = _node_array_entry2->second.node_occr.find(level_at);
            if(lv_occur != _node_array_entry2->second.node_occr.end()) {
                _occurrence_entry = lv_occur->second.find(l_m->value);
                if(_occurrence_entry != lv_occur->second.end()){
                    unordered_set<et_tree_Node*>::iterator _occurrence = _occurrence_entry->second.find(l_m);
                    if(_occurrence != _occurrence_entry->second.end()){
                        _occurrence_entry->second.erase(_occurrence);
                    }
                }
            }

        }
    }

    if(ct3 != NULL){
        ct3 = union_link(ct3, newnode, level_at);
        ct2 = union_link(ct2, ct3, level_at);
    }
    else{
        ct2 = union_link(ct2, newnode, level_at);
    }

    tree = ct2;
}

//find the root node in the treap that a given node is in
et_tree_Node* HDT::find_root(et_tree_Node *node) {
    if(node == NULL){
        return NULL;
    }
    et_tree_Node* temp;
    for(et_tree_Node* p = node; p; p = p->parent){
        temp = p;
    }
    return temp;
}

//study why fast, compute the height of the treap
int maxDepth(et_tree_Node *treeNode){
    if(treeNode == NULL){
        return 0;
    }
    else{
        int depth_l = maxDepth(treeNode->l_child);
        int depth_r = maxDepth(treeNode->r_child);

        if(depth_l > depth_r){
            return (depth_l + 1);
        }
        else{
            return (depth_r + 1);
        }
    }
}

//this method is the main entry for splitting the treap
void HDT::split_treap(string u, string v, int level_at) {
    unordered_map<string, array_Node>::iterator _node1_entry = node_array.find(u);
    unordered_map<string, array_Node>::iterator _node2_entry = node_array.find(v);
    unordered_map<int, et_tree_Node*>::iterator lv_occur;
    et_tree_Node* n1;
    et_tree_Node* n2;
    et_tree_Node* tree = NULL;
    if(_node1_entry != node_array.end()){
        lv_occur = _node1_entry->second.act_Node1.find(level_at);
        if(lv_occur != _node1_entry->second.act_Node1.end()){
            n1 = find_root(lv_occur->second);
        }

    }
    else{
        return;
    }
    if(_node2_entry != node_array.end()){
        lv_occur = _node2_entry->second.act_Node1.find(level_at);
        if(lv_occur != _node2_entry->second.act_Node1.end()){
            n2 = find_root(lv_occur->second);
        }
    }
    else{
        return;
    }
    string root1 = find_root(n1)->value;
    string root2 = find_root(n2)->value;
    if(root1 == root2){
        //u and v are in the same treap
        //find that sequence in the treap list
        unordered_map<int, unordered_map<string, et_tree_Node*> >::iterator level_treap_list = treap_list_with_level.find(level_at);
        if(level_treap_list != treap_list_with_level.end()){
            unordered_map<string, et_tree_Node*>::iterator _treap_list_entry = level_treap_list->second.find(root1);
            if(_treap_list_entry != level_treap_list->second.end()){
                tree = n1;
                //find the first node of the treap(leftmost one)
                et_tree_Node* left = find_leftmost(tree);
                if(u == left->value && v != left->value){
                    //v is the internal node
                    cut_treap(u, v, tree, level_at);
                }
                else if(v == left->value && u != left->value){
                    //u is the internal node
                    cut_treap(v, u, tree, level_at);
                }
                else if(v != left->value && u != left->value){
                    //u and v are both internal nodes
                    //firstly, we reroot by u for convenience
                    rerooting(u, tree, level_at);

                    cut_treap(u, v, tree, level_at);
                }
                level_treap_list->second.erase(_treap_list_entry);
            }
            else{
                return;
            }
        }
    }
    else{
        //u and v are not in the same treap
        cout << "This edge does not exist!" << endl;
    }
}

//check if the given nodes are connected
//which means that checking if two nodes are in the same trees
bool HDT::connected(string u, string v, int level_at) {
    et_tree_Node* lk1 = NULL;
    et_tree_Node* lk2 = NULL;
    unordered_map<string, array_Node>::iterator _node1_entry = node_array.find(u);
    unordered_map<string, array_Node>::iterator _node2_entry = node_array.find(v);
    if(_node1_entry != node_array.end() && _node2_entry != node_array.end()){
        unordered_map<int, et_tree_Node*>::iterator lv_occur1 = _node1_entry->second.act_Node1.find(level_at);
        unordered_map<int, et_tree_Node*>::iterator lv_occur2 = _node2_entry->second.act_Node1.find(level_at);
        if(lv_occur1 != _node1_entry->second.act_Node1.end() && lv_occur2 != _node2_entry->second.act_Node1.end()){
            lk1 = find_root(lv_occur1->second);
            lk2 = find_root(lv_occur2->second);

            if(lk1->value == lk2->value){
                return true;
            }
        }

    }
    return false;
}

//given two labels of nodes in two treaps, return the smaller treap
string HDT::smaller_tree_label(string l1, string l2, int level_at) {
    et_tree_Node* t1;
    et_tree_Node* t2;
    unordered_map<string, array_Node>::iterator iterator1 = node_array.find(l1);
    unordered_map<int, et_tree_Node*>::iterator lv_occur;
    if(iterator1 != node_array.end()){
        lv_occur = iterator1->second.act_Node1.find(level_at);
        if(lv_occur != iterator1->second.act_Node1.end()){
            t1 = find_root(lv_occur->second);
        }

    }
    iterator1 = node_array.find(l2);
    if(iterator1 != node_array.end()){
        lv_occur = iterator1->second.act_Node1.find(level_at);
        if(lv_occur != iterator1->second.act_Node1.end()){
            t2 = find_root(lv_occur->second);
        }
    }
    if(t1->node_weight < t2->node_weight){
        return l1;
    }
    else{
        return l2;
    }
}

//check if the given nodes are connected
//which means that checking if two nodes are in the same trees
bool HDT::fct_connected(string u, string v, int level_at) {
    et_tree_Node* lk1 = NULL;
    et_tree_Node* lk2 = NULL;
    unordered_map<string, array_Node>::iterator iterator1 = node_array.find(u);
    unordered_map<string, array_Node>::iterator iterator2 = node_array.find(v);
    if(iterator1 != node_array.end() && iterator2 != node_array.end()){
        unordered_map<int, et_tree_Node*>::iterator lv_occur1 = iterator1->second.act_Node1.find(level_at);
        unordered_map<int, et_tree_Node*>::iterator lv_occur2 = iterator2->second.act_Node1.find(level_at);
        if(lv_occur1 != iterator1->second.act_Node1.end() && lv_occur2 != iterator2->second.act_Node1.end()){
            lk1 = find_root(lv_occur1->second);
            lk2 = find_root(lv_occur2->second);
            if(lk1 == lk2){
                return true;
            }
        }

    }
    return false;
}

HDT::~HDT() {
    for(unordered_set<et_tree_Node*>::iterator iterator1 = backUpNodeSet->begin(); iterator1 != backUpNodeSet->end();
    iterator1++){
        delete *iterator1;
    }
}