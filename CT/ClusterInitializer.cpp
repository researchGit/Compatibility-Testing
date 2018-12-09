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
// Detailed implementation of ClusterInitialzer.h
//

#include "ClusterInitializer.h"

using namespace std;

//compute the clusters for each node in the input tree
set<string> ClusterInitializer::compute_cluster(dis_tree *tree) {
    string label = tree->label;
    if(!(tree->chd.empty())){
        //for each child of a node in the tree, compute its cluster and then add them into the cluster set of this tree node
        for(unordered_set<dis_tree *>::iterator _node_child_entry = tree->chd.begin(); _node_child_entry != tree->chd.end(); _node_child_entry++){
            set<string> _cluster_set = compute_cluster(*_node_child_entry);
            if(!(_cluster_set.empty())){
                //insert cluster
                for(set<string>::iterator _children_set_entry = _cluster_set.begin(); _children_set_entry != _cluster_set.end();
                    _children_set_entry++){
                    tree->cluster.insert(*_children_set_entry);
                }
            }
        }
        //for those input trees, we assume that no nodes are named with prefix "temp_"
        if(label.find("temp_") == string::npos){
            tree->cluster.insert(label);
        }
    }
    else{
        //the node has no children
        //this node is the leaf node
        tree->cluster.insert(label);
    }
    return tree->cluster;
}

//compute the clusters for each node in the super tree, the difference between compute_cluster4super and compute_cluster is
//that in the super tree, there will be some tree node containing a bunch of labels, which means that several labels will be mapped
//to a single tree node, therefore, compute_cluster4super will use structure called "mapping" to construct the mapping relation between
//super node and labels, this structure will be later used for analyzing distance between input trees and super trees
set<string> ClusterInitializer::compute_cluster4super(dis_tree *tree) {
    string label = tree->label;
    if(!(tree->chd.empty())){
        //has children, then compute cluster for each node
        for(unordered_set<dis_tree *>::iterator _node_child_entry = tree->chd.begin(); _node_child_entry != tree->chd.end(); _node_child_entry++){
            set<string> _cluster_set = compute_cluster4super(*_node_child_entry);
            if(!(_cluster_set.empty())){
                //insert the clusters into the cluster set of this node
                for(set<string>::iterator _children_set_entry = _cluster_set.begin(); _children_set_entry != _cluster_set.end();
                    _children_set_entry++){
                    tree->cluster.insert(*_children_set_entry);
                }
            }
        }
        //for those node containing several labels
        //parse current label to get internal labels and mappings
        int length = (int)label.length();
        string string1;
        for(int i = 0; i < length; i++){
            char c = label[i];
            if(c != ' '){
                string1 += c;
            }
            else{
                if(string1.find("temp_") != string::npos){
                    string1 = "";
                    continue;
                }
                else{
                    mapping.insert(unordered_map<string, string>::value_type(string1, label));
                    tree->cluster.insert(string1);
                    string1 = "";
                }
            }
        }
        if(string1.find("temp_") != string::npos){
            string1 = "";
        }
        else{
            mapping.insert(unordered_map<string, string>::value_type(string1, label));
            tree->cluster.insert(string1);
            string1 = "";
        }
    }
    else{
        //no children
        //this node is the leaf node
        tree->cluster.insert(label);
        mapping.insert(unordered_map<string, string>::value_type(label, label));
    }
    return tree->cluster;
}