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
// Detailed implementation of ScoreAnalyzer.h
//

#include "ScoreAnalyzer.h"
#include <queue>
#include <fstream>
#include <sstream>

using namespace std;

//compute the distance between super tree and input trees by using the number of the clusters lost as the quantity
int ScoreAnalyzer::compute_score(dis_tree input, dis_tree super, unordered_map<string, string> mapping) {

    //initialize the sparse table structire
    SparseTable st;
    st.dfs(super, 0);
    vector<string> euler_tour_seq = st.get_euler_tour_seq();
    unordered_map<string, dis_tree> node_array_location = st.get_node_array_location();


    //construct rmq structure
    int _euler_tour_sequence_size = (int)euler_tour_seq.size();
    st.rmq_preprocess(_euler_tour_sequence_size);

    //go through all the non-leaf node in the input tree
    //get the leaf set of that node
    //find the lca of leaves in the leaf set
    //compute the difference between this node and the lca node
    set<string> setX = input.cluster;
    queue<dis_tree> queue1;
    queue1.push(input);
    int score_num = 0;
    while(!queue1.empty()){
        dis_tree tree = queue1.front();
        queue1.pop();
        string lca = "";
        if(tree.cluster.size() > 0){
            lca = *(tree.cluster.begin());
            unordered_map<string, string>::iterator _label_mapping_entry = mapping.find(lca);
            if(_label_mapping_entry != mapping.end()){
                lca = _label_mapping_entry->second;
            }
        }
        for(set<string>::iterator _clusters_set_entry = tree.cluster.begin(); _clusters_set_entry != tree.cluster.end();
            _clusters_set_entry++){
            string value = *_clusters_set_entry;
            unordered_map<string, string>::iterator iterator1 = mapping.find(value);
            if(iterator1 != mapping.end()){
                value = iterator1->second;
            }
            if(_clusters_set_entry != tree.cluster.begin()){
                lca = st.compute_LCA(lca,value);
                if(lca == "can not find the corresponding node"){
                    return -1;
                }
            }
        }
        cout << "resulting lca: " << lca << endl;
        //lca should be the lca of the leaves
        unordered_map<string, dis_tree>::iterator _node_array_location_entry = node_array_location.find(lca);
        if(_node_array_location_entry != node_array_location.end()){
            //find it
            set<string> input_cluster = tree.cluster;
            set<string> super_cluster = _node_array_location_entry->second.cluster;

            set<string> restriction;

            set_intersection(setX.begin(), setX.end(), super_cluster.begin(), super_cluster.end(),
                             inserter(restriction, restriction.begin()));

            if(input_cluster.size() != restriction.size()){
                cout << "cluster lost: {";
                for(set<string>::iterator _input_cluster_set_entry = input_cluster.begin(); _input_cluster_set_entry != input_cluster.end();){
                    cout << *_input_cluster_set_entry;
                    _input_cluster_set_entry++;
                    if(_input_cluster_set_entry != input_cluster.end()){
                        cout << ",";
                    }
                }

                cout << "}" <<endl;
                score_num++;
            }

        }
        else{
            cout << "ERROR! No location of LCA is found!" << endl;
        }
        for(unordered_set<dis_tree*>::iterator iterator4 = tree.chd.begin(); iterator4 != tree.chd.end(); iterator4++){
            queue1.push(*(*iterator4));
        }
    }
    st.free_rmq_array(_euler_tour_sequence_size);
    return score_num;
}

//Given path of a source file as the input, this method will read all the newick format strings in
//the file, and then prepares all the strings for parsing
unordered_map<int, dis_tree> ScoreAnalyzer::read_parse_from_file(string path) {
    ifstream input_file(path);
    string str;

    int tree_count = 1;//count the number of the trees
    unordered_map<int, dis_tree> tree_list;

    Parser* parser = new Parser();
    while(getline(input_file, str)){
        dis_tree tree = parser->parse_label(str);
        tree_list.insert(unordered_map<int, dis_tree>::value_type(tree_count, tree));
        tree_count ++;
    }
    delete(parser);
    return tree_list;
}