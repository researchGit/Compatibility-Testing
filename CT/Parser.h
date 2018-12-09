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
// This class implements a parser transforming from Newick format to linked-list tree structure which
// is described in SparseTable.h in linear time
// This structure is mainly used for analyzing the quality of resulting super tree
// Details are implemented in Parser.cpp
//

#ifndef FGO_PARSER_H
#define FGO_PARSER_H

#include "SparseTable.h"

class Parser {

public:
    dis_tree parse_label(string newick_string);
    tree_node* parse_label_weight(string newick_string);
    vector<string> get_label_weight(string str);


private:
    int label_counter = 1;//used for labeling those nodes which do not have any label
                          // the number will be increased without fixing its correctness
    string string_trim(string& s);
};


#endif //FGO_PARSER_H
