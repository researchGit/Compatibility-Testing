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
// Detailed implementation of Linked_list_Builder.h
//

#include "Linked_List_Builder.h"

using namespace std;

//Merge all of the input trees into a big linked-list structure
//at the same time, compute valid position and semi-universal nodes
void Linked_List_Builder::merge_input_trees(
        multimap<string, unordered_map<string, linked_list_entry> > input_trees,
        unordered_map<string, linked_list_entry> global_hpu) {
    //record initial valid position
    unordered_map<int, unordered_set<string> > _valid_pos;
    //record initial semi-universal nodes
    unordered_map<string, int> _semi_nodes;
    //record HPU
    unordered_map<string, linked_list_entry> _hpu;

    for(multimap<string, unordered_map<string, linked_list_entry> >::iterator input_trees_iterator = input_trees.begin();
        input_trees_iterator != input_trees.end(); input_trees_iterator++){

        //compute valid position
        //get the label
        string rootval = input_trees_iterator->first;
        //record which tree a label is contained
        unordered_set<int> belongs;
        unordered_map<string, linked_list_entry>::iterator list_entry_iterator = input_trees_iterator->second.find(rootval);
        if(list_entry_iterator != input_trees_iterator->second.end()){
            belongs = list_entry_iterator->second.belongs;
        }
        unordered_set<string> tempset1;
        tempset1.insert(rootval);
        pair<int, unordered_set<string> > pa(*belongs.begin(), tempset1);
        _valid_pos.insert(pa);
    }

    set_valid_position(_valid_pos);
    //set_graph_hpu(global_hpu);
    set_nodes_number((int)global_hpu.size());
    sum_kl = NODES_NUMBER;

    //compute semi universal nodes
    for(unordered_map<int, unordered_set<string> >::iterator valid_pos_iterator = initial_valid_pos.begin();
        valid_pos_iterator != initial_valid_pos.end(); valid_pos_iterator++){
        string str = *(valid_pos_iterator->second.begin());
        unordered_map<string, linked_list_entry>::iterator hpu_entry_finder = global_hpu.find(str);
        if(hpu_entry_finder != global_hpu.end()){
            string string1 = hpu_entry_finder->first;
            linked_list_entry n = hpu_entry_finder->second;
            n.count += 1;
            global_hpu.erase(hpu_entry_finder);
            global_hpu.insert(unordered_map<string, linked_list_entry>::value_type(string1, n));
        }
    }

    for(unordered_map<int, unordered_set<string> >::iterator _valid_position_entry = initial_valid_pos.begin();
        _valid_position_entry != initial_valid_pos.end(); _valid_position_entry++){
        string str = (*_valid_position_entry->second.begin());
        unordered_map<string, linked_list_entry>::iterator _hpu_entry = global_hpu.find(str);
        if(_hpu_entry != global_hpu.end()){
            if(_hpu_entry->second.count == _hpu_entry->second.kl){
                _semi_nodes.insert(unordered_map<string, int>::value_type(str, _valid_position_entry->first));
            }
        }
    }

    set_semi_universal_nodes(_semi_nodes);
}