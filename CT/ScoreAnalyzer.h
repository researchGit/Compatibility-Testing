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
// This class is implemented for computing the distance between super tree and its input trees
// The score is determined by the number of clusters that the super tree loses
// The higher the score is, the worse the super tree is
//

#ifndef FGO_SCOREANALYZER_H
#define FGO_SCOREANALYZER_H

#include "Parser.h"
#include "SparseTable.h"

using namespace std;

class ScoreAnalyzer {
public:
    int compute_score(dis_tree input, dis_tree super, unordered_map<string, string> mapping);
    unordered_map<int, dis_tree> read_parse_from_file(string path);
};


#endif //FGO_SCOREANALYZER_H
