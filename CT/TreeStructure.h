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
// TreeStructure.h is designed to contain a tree structure that is used for computing distance between
// super trees and input trees. This structure is also used for constructing the trees that are used for
// compatibility test. "dis_tree" refers for "distance tree" and "tree_node" refers for "tree node"
//

#ifndef FGO_TREESTRUCTURE_H
#define FGO_TREESTRUCTURE_H

#endif //FGO_TREESTRUCTURE_H

#include <iostream>
#include <set>
#include <unordered_set>

using namespace std;

typedef struct dis_tree{
    dis_tree *parent;//the parent node of this node
    string label;//the label of this node
    string weight;//the weight follows the label in the input tree
    set<string> cluster;//cluster set
    unordered_set<dis_tree *> chd;//children nodes
    dis_tree(){};
    dis_tree(string str1):label(str1){};
}tree_node;

