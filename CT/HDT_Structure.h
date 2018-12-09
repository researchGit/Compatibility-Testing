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
// HDT_Structure.h is the file containing most data structure used by HDT algorithm
//

#ifndef FGO_HDT_STRUCTURE_H
#define FGO_HDT_STRUCTURE_H
#include <iostream>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <sstream>

using namespace std;

struct graph_Edge{
    string endp1; //smaller endpoint
    string endp2; //larger endpoint
    bool operator == (const graph_Edge &t) const {
        return  endp1==t.endp1 && endp2==t.endp2;
    }
    graph_Edge(){};
    graph_Edge(string str1, string str2):endp1(str1),endp2(str2){};
};

struct graph_Edge2{
    string endp1; //smaller endpoint
    string endp2; //larger endpoint
    int level;
    bool operator == (const graph_Edge2 &t) const {
        return  endp1==t.endp1 && endp2==t.endp2 && level == t.level;
    }
    graph_Edge2(){};
    graph_Edge2(string str1, string str2, int l):endp1(str1),endp2(str2), level(l){};
};

struct hash_Edge {
    size_t operator () (const graph_Edge &t) const {
        hash<string> hash;
        return  hash(t.endp1+t.endp2);
    }
};

struct hash_Edge2 {
    size_t operator () (const graph_Edge2 &t) const {
        hash<string> hash;
        string str;
        stringstream ss;
        ss<<t.level;
        str += t.endp1;
        str += t.endp2;
        str += ss.str();
        return  hash(str);
    }
};

struct com_edge{

    bool operator()(const graph_Edge& edge1,const graph_Edge& edge2) const{
        if(edge1.endp1 < edge2.endp1){
            return true;
        }
        else if(edge1.endp2 < edge2.endp2){
            return true;
        }
        return false;
    }
};


struct et_tree_Node{
    double number; //assign some number to this variable in order to construct the et-tree (treap)
    string value; //the name of the node
    int priority; //used for treap
    int node_weight;//number of active nodes in its subtree
    bool act_child; //flag variable, in order to do efficient searching on non_tree edges
    bool active; //if this node is the active node among its own occurance
    et_tree_Node* l_child;
    et_tree_Node* r_child;
    et_tree_Node* parent;
    et_tree_Node(){};
    et_tree_Node(double number, string value, int priority, bool active):
            number(number), value(value),priority(priority),active(active){
        node_weight = 1;
        act_child = false;
        l_child = NULL;
        r_child = NULL;
        parent = NULL;
    }
};

struct array_Node{
    string value; //name of the node
    unordered_map<int, et_tree_Node*> act_Node1; // points to the act node among its occurance
    string seq_root;//no longer useful. will be replaced by root
    unordered_map<int, unordered_map<string, unordered_set<et_tree_Node*> > > node_occr;//used for storing all the occrences of a certain node, replace act_Node1 act_Node2
    unordered_set<string> indegree;
    array_Node(){};
    array_Node(string str, string _seq_root):value(str), seq_root(_seq_root){};
};

struct occ_Node{
    double number; //assign some number to this variable in order to construct the et-tree (treap)
    string value; //name of the node
    bool active;
    unordered_map<int, unordered_set<graph_Edge, hash_Edge> > non_tree_edge;
    occ_Node(){};
    occ_Node(double num, string str, bool act):number(num),value(str), active(act){};
};

struct com_array_node{

    bool operator()(const array_Node& node1,const array_Node& node2) const{
        if(node1.value < node2.value){
            return true;
        }
        else if(node1.act_Node1 != node2.act_Node1){
            return true;
        }
        return false;
    }
};
#endif //FGO_HDT_STRUCTURE_H
