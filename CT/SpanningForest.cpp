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
// Detailed implementation of SpanningForest.h
//

#include "SpanningForest.h"

using namespace std;

//main access to generating euler tour sequence by using DFS algorithm
void SpanningForest::generate_spanning_forest(unordered_map<string, linked_list_entry> _hpu) {
    //original method
    for(unordered_map<string, linked_list_entry>::iterator _hpu_iterator = _hpu.begin();
        _hpu_iterator != _hpu.end(); _hpu_iterator++){
        if(_hpu_iterator->second.color == WHITE){
            string element_value = _hpu_iterator->second.value;
            non_recursive_DFS_visit(&_hpu, element_value, element_value);
        }
    }
}

//non-recursive DFS algorithm
void SpanningForest::non_recursive_DFS_visit(unordered_map<string, linked_list_entry> *graph_entry, string node_value, string first_value){
    stack<linked_list_entry> *_entry_stack = new stack<linked_list_entry>();

    unordered_map<string, linked_list_entry>::iterator _entry_finder = graph_entry->find(node_value);
    if(_entry_finder != graph_entry->end()){
        //find it
        //then visit each node connecting with it
        _entry_finder->second.color = GRAY;
        _entry_stack->push(_entry_finder->second);
        while(!(_entry_stack->empty())){
            linked_list_entry _top_list_entry;
            swap(_top_list_entry, _entry_stack->top());
            _entry_stack->pop();
            if(_top_list_entry.reinsert == 0){
                string _entry_value = _top_list_entry.value;
                _top_list_entry.first_occur_number = OCC_NUMBER_COUNTER;
                //create the first occurrence of a new value in the euler tour sequence
//                build_euler_tour_sequence(OCC_NUMBER_COUNTER, _entry_value, true);
                occ_Node occ1(OCC_NUMBER_COUNTER,_entry_value, true);
                et_tour_sequence.push_back(occ1);
                OCC_NUMBER_COUNTER++;
//                build_NodeArray(_entry_value, first_value);
                array_Node an(_entry_value, first_value);
                node_array[_entry_value] = an;

                for(unordered_map<string, linked_list_entry>::iterator _ajcn_list_entry = _top_list_entry.ajcn_list.begin();
                    _ajcn_list_entry != _top_list_entry.ajcn_list.end(); _ajcn_list_entry++){
                    string ajcn_value = _ajcn_list_entry->first;
                    unordered_map<string, linked_list_entry>::iterator _ajcn_entry = graph_entry->find(ajcn_value);
                    if(_ajcn_entry != graph_entry->end()){
                        //record where this node pointed from
                        _ajcn_entry->second.from = _entry_value;
                        int color = _ajcn_entry->second.color;
                        graph_Edge edge;
                        if (_entry_value < ajcn_value) {
                            edge.endp1 = _entry_value;
                            edge.endp2 = ajcn_value;
                        }
                        else {
                            edge.endp1 = ajcn_value;
                            edge.endp2 = _entry_value;
                        }
                        if(color == WHITE){
                            //reinsert its origin into the stack to build up the euler tour sequence
                            _top_list_entry.reinsert++;
                            _entry_stack->push(_top_list_entry);

                            all_edges.insert(unordered_map<graph_Edge, int, hash_Edge>::value_type(edge, 0));
                            unordered_map<int, unordered_set<graph_Edge, hash_Edge> >::iterator _tree_edge_finder = tree_edge.find(0);
                            if(_tree_edge_finder != tree_edge.end()){
                                _tree_edge_finder->second.insert(edge);
                            }
                            else{
                                unordered_set<graph_Edge, hash_Edge> tempset;
                                tempset.insert(edge);
                                tree_edge.insert(unordered_map<int, unordered_set<graph_Edge, hash_Edge> >::value_type(0, tempset));
                            }
//                            mpMap.insert(multimap<string, string>::value_type(_entry_value, ajcn_value));
                            _ajcn_entry->second.color = GRAY;
                            _entry_stack->push(_ajcn_entry->second);
                        }
                        else if(color == GRAY){
                            string origin = _top_list_entry.from;
                            if (_ajcn_entry->second.value != origin){
                                all_edges.insert(unordered_map<graph_Edge, int, hash_Edge>::value_type(edge, 0));
                                unordered_map<int, unordered_set<graph_Edge, hash_Edge> >::iterator non_tree_edge_finder = non_tree_edge.find(0);
                                if(non_tree_edge_finder != non_tree_edge.end()){
                                    non_tree_edge_finder->second.insert(edge);
                                }
                                else{
                                    unordered_set<graph_Edge, hash_Edge> tempset;
                                    tempset.insert(edge);
                                    non_tree_edge.insert(unordered_map<int, unordered_set<graph_Edge, hash_Edge> >::value_type(0, tempset));
                                }

                                graph_Edge edge1;
                                edge1.endp1 = edge.endp2;
                                edge1.endp2 = edge.endp1;
                                unordered_map<int, unordered_map<string, unordered_set<graph_Edge, hash_Edge> > >::iterator edge_finder = non_tree_edge_query_table.find(0);
                                if(edge_finder != non_tree_edge_query_table.end()){
                                    unordered_map<string, unordered_set<graph_Edge, hash_Edge>>::iterator edge_entry = edge_finder->second.find(edge.endp1);
                                    if(edge_entry != edge_finder->second.end()){
                                        edge_entry->second.insert(edge);
                                    }
                                    else{
                                        unordered_set<graph_Edge, hash_Edge> tempset;
                                        tempset.insert(edge);
                                        edge_finder->second.insert(unordered_map<string, unordered_set<graph_Edge, hash_Edge> >::value_type(edge.endp1, tempset));
                                    }

                                    edge_entry = edge_finder->second.find(edge.endp2);
                                    if(edge_entry != edge_finder->second.end()){
                                        edge_entry->second.insert(edge1);
                                    }
                                    else{
                                        unordered_set<graph_Edge, hash_Edge> tempset;
                                        tempset.insert(edge1);
                                        edge_finder->second.insert(unordered_map<string, unordered_set<graph_Edge, hash_Edge> >::value_type(edge.endp2, tempset));
                                    }
                                }
                                else{
                                    unordered_set<graph_Edge, hash_Edge> tempset;
                                    tempset.insert(edge);
                                    unordered_map<string, unordered_set<graph_Edge, hash_Edge> > tempmap;
                                    tempmap.insert(unordered_map<string, unordered_set<graph_Edge, hash_Edge> >::value_type(edge.endp1, tempset));
                                    unordered_set<graph_Edge, hash_Edge> tempset1;
                                    tempset1.insert(edge1);
                                    unordered_map<string, unordered_set<graph_Edge, hash_Edge> > tempmap1;
                                    tempmap1.insert(unordered_map<string, unordered_set<graph_Edge, hash_Edge> >::value_type(edge.endp2, tempset1));
                                    non_tree_edge_query_table.insert(unordered_map<int, unordered_map<string, unordered_set<graph_Edge, hash_Edge> > >::value_type(0, tempmap));
                                    non_tree_edge_query_table.insert(unordered_map<int, unordered_map<string, unordered_set<graph_Edge, hash_Edge> > >::value_type(0, tempmap1));
                                }
                            }
                        }
                    }
                }
                if(_top_list_entry.reinsert == 0){
                    _top_list_entry.color = BLACK;
                }
            }
            else{
                _top_list_entry.reinsert--;
                occ_Node occ1(OCC_NUMBER_COUNTER,_top_list_entry.value, false);
                et_tour_sequence.push_back(occ1);
                OCC_NUMBER_COUNTER++;
            }
        }
    }
    delete _entry_stack;
}

void SpanningForest::generate_spanning_forest_withNoColor(unordered_map<string, linked_list_entry> _hpu) {
    unordered_map<string, bool> *visitMap = new unordered_map<string, bool>();
    unordered_set<string> *elementSet = new unordered_set<string>();
    unordered_map<string, string> *fromMap = new unordered_map<string, string>();
    stack<string> *dfsStack = new stack<string>();
    unordered_map<string, int> *countMap = new unordered_map<string, int>();

    for(unordered_map<string, linked_list_entry>::iterator adjIter = _hpu.begin(); adjIter != _hpu.end();
        adjIter++){
        visitMap->insert(unordered_map<string, bool>::value_type(adjIter->first, false));
        elementSet->insert(adjIter->first);
        countMap->insert(unordered_map<string, int>::value_type(adjIter->first, 0));
    }

    string first_value = *(elementSet->begin());

    while(!elementSet->empty()){
        dfsStack->push(*(elementSet->begin()));
        while(!dfsStack->empty()){
            string tmpStr = dfsStack->top();
            dfsStack->pop();

            unordered_map<string, bool>::iterator visIter = visitMap->find(tmpStr);
            if(visIter != visitMap->end()){
                visIter->second = true;

                unordered_map<string, int>::iterator countIter = countMap->find(tmpStr);
                if(countIter != countMap->end()){
                    int tmpCount = countIter->second + 1;
                    countIter->second = tmpCount;
                    if(tmpCount > 1){
                        occ_Node occ1(OCC_NUMBER_COUNTER, tmpStr, false);
                        et_tour_sequence.push_back(occ1);
                        OCC_NUMBER_COUNTER++;
                    }
                    else{
                        occ_Node occ1(OCC_NUMBER_COUNTER, tmpStr, true);
                        et_tour_sequence.push_back(occ1);
                        OCC_NUMBER_COUNTER++;

                        array_Node an(tmpStr, first_value);
                        node_array[tmpStr] = an;
                    }
                }

                unordered_set<string>::iterator eleIter = elementSet->find(visIter->first);
                if(eleIter != elementSet->end()){
                    elementSet->erase(eleIter);
                }

                bool all_empty = false;
                unordered_map<string, linked_list_entry>::iterator adjIter = _hpu.find(tmpStr);
                if(adjIter != _hpu.end()){
                    for(unordered_map<string, linked_list_entry>::iterator listIter = adjIter->second.ajcn_list.begin();
                    listIter != adjIter->second.ajcn_list.end(); listIter++){
                        visIter = visitMap->find(listIter->first);
                        if(visIter != visitMap->end()){
                            if(!visIter->second){
                                all_empty = true;
                                dfsStack->push(listIter->first);
                                fromMap->insert(unordered_map<string, string>::value_type(listIter->first, tmpStr));

                                //put this edge in the tree edge list
                                graph_Edge edge("","");
                                if(listIter->first > tmpStr){
                                    edge.endp1 = tmpStr;
                                    edge.endp2 = listIter->first;
                                }
                                else{
                                    edge.endp1 = listIter->first;
                                    edge.endp2 = tmpStr;
                                }

                                unordered_map<int, unordered_set<graph_Edge, hash_Edge> >::iterator treeIter = tree_edge.find(0);
                                if(treeIter != tree_edge.end()){
                                    treeIter->second.insert(edge);
                                }
                                else{
                                    unordered_set<graph_Edge, hash_Edge> tempSet;
                                    tempSet.insert(edge);
                                    tree_edge.insert(unordered_map<int, unordered_set<graph_Edge, hash_Edge> >::value_type(0, tempSet));
                                }

                                all_edges.insert(unordered_map<graph_Edge, int, hash_Edge>::value_type(edge, 0));

                                break;
                            }
                            else{
                                //put this edge in the non-tree edge list
                                graph_Edge edge("","");
                                if(listIter->first > tmpStr){
                                    edge.endp1 = tmpStr;
                                    edge.endp2 = listIter->first;
                                }
                                else{
                                    edge.endp1 = listIter->first;
                                    edge.endp2 = tmpStr;
                                }

                                unordered_map<int, unordered_set<graph_Edge, hash_Edge> >::iterator treeIter = tree_edge.find(0);
                                if(treeIter != tree_edge.end()){
                                    unordered_set<graph_Edge, hash_Edge>::iterator setIt = treeIter->second.find(edge);
                                    if(setIt == treeIter->second.end()){
                                        //push into nontree list and alledge list
                                        all_edges.insert(unordered_map<graph_Edge, int, hash_Edge>::value_type(edge, 0));

                                        unordered_map<int, unordered_set<graph_Edge, hash_Edge> >::iterator non_tree_edge_finder = non_tree_edge.find(0);
                                        if(non_tree_edge_finder != non_tree_edge.end()){
                                            non_tree_edge_finder->second.insert(edge);
                                        }
                                        else{
                                            unordered_set<graph_Edge, hash_Edge> tempset;
                                            tempset.insert(edge);
                                            non_tree_edge.insert(unordered_map<int, unordered_set<graph_Edge, hash_Edge> >::value_type(0, tempset));
                                        }

                                        graph_Edge edge1;
                                        edge1.endp1 = edge.endp2;
                                        edge1.endp2 = edge.endp1;
                                        unordered_map<int, unordered_map<string, unordered_set<graph_Edge, hash_Edge> > >::iterator edge_finder = non_tree_edge_query_table.find(0);
                                        if(edge_finder != non_tree_edge_query_table.end()){
                                            unordered_map<string, unordered_set<graph_Edge, hash_Edge>>::iterator edge_entry = edge_finder->second.find(edge.endp1);
                                            if(edge_entry != edge_finder->second.end()){
                                                edge_entry->second.insert(edge);
                                            }
                                            else{
                                                unordered_set<graph_Edge, hash_Edge> tempset;
                                                tempset.insert(edge);
                                                edge_finder->second.insert(unordered_map<string, unordered_set<graph_Edge, hash_Edge> >::value_type(edge.endp1, tempset));
                                            }

                                            edge_entry = edge_finder->second.find(edge.endp2);
                                            if(edge_entry != edge_finder->second.end()){
                                                edge_entry->second.insert(edge1);
                                            }
                                            else{
                                                unordered_set<graph_Edge, hash_Edge> tempset;
                                                tempset.insert(edge1);
                                                edge_finder->second.insert(unordered_map<string, unordered_set<graph_Edge, hash_Edge> >::value_type(edge.endp2, tempset));
                                            }
                                        }
                                        else{
                                            unordered_set<graph_Edge, hash_Edge> tempset;
                                            tempset.insert(edge);
                                            unordered_map<string, unordered_set<graph_Edge, hash_Edge> > tempmap;
                                            tempmap.insert(unordered_map<string, unordered_set<graph_Edge, hash_Edge> >::value_type(edge.endp1, tempset));
                                            unordered_set<graph_Edge, hash_Edge> tempset1;
                                            tempset1.insert(edge1);
                                            unordered_map<string, unordered_set<graph_Edge, hash_Edge> > tempmap1;
                                            tempmap1.insert(unordered_map<string, unordered_set<graph_Edge, hash_Edge> >::value_type(edge.endp2, tempset1));
                                            non_tree_edge_query_table.insert(unordered_map<int, unordered_map<string, unordered_set<graph_Edge, hash_Edge> > >::value_type(0, tempmap));
                                            non_tree_edge_query_table.insert(unordered_map<int, unordered_map<string, unordered_set<graph_Edge, hash_Edge> > >::value_type(0, tempmap1));
                                        }
                                    }
                                }
                            }
                        }
                    }
                }

                if(!all_empty){
                    unordered_map<string, string>::iterator fromIter = fromMap->find(tmpStr);
                    if(fromIter != fromMap->end()){
                        dfsStack->push(fromIter->second);
                    }
                }
            }
        }
    }

    delete visitMap;
    delete elementSet;
    delete fromMap;
    delete dfsStack;
    delete countMap;
}