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

//main access to the program, provide a command line interface
//the result is written to a file under the same directory as this exe file
#include <iostream>
#include "InputGenerator.h"
#include "SpanningForest.h"
#include "FastCompatibilityTest.h"
#include "CompatibilityAnalyzer.h"
#include <ctime>
#include <sstream>
#include <fstream>
#include <string>

//this method will write the results to a file located the same place as the executable file
void write2file(string result_string, int count){
    if(count == 0){
        ofstream file("result.txt");
        if(file.is_open()){
            file << result_string;
            file << "\n";
            file.close();
        }
    }
    else{
        ofstream file("result.txt", ios::app);
        if(file.is_open()){
            file << result_string;
            file << "\n";
            file.close();
        }
    }

}

//input is a tuple of parameters (N, K, D, M) or a path of a file that contains a list of phylogenetic trees
//N: integer, number of labeled nodes
//K: integer, number of phylogenetic trees
//D: integer, out-degree
//M: boolean variable, if M = true, then edge promotions are allowed
int main(int argc, char *argv[]) {
    srand(time(NULL));
    //specify the method of inputting phylogenetic trees
    //-i: phylogenetic trees are from a file in Newick format
    //-s: simulate the phylogenetic trees
    //-v: solve IDPP instances
    if(argc <= 1){
        cout << "Error: Please specify input type!" << endl;
        return 0;
    }

    string input_method = argv[1];

    if(input_method == "-i"){
        //inputs are from a file
        if(argc == 3){
            bool edge_promotion = false;
            cout << "Warning: HDT disables edge promotions by default";

            string file_path = argv[2];
            cout << file_path << endl;

            //initialization(including parsing the newick format, constructing data structures
            //computing initial valid positions, computing initial semi-universal nodes,
            //building up spanning trees and forest, etc)
            cout << "Initialization --> start" << endl;

            CompatibilityAnalyzer ca;
            //parse newick strings
            //construct HPU, initial valid position, initial semi-universal nodes
            ca.initializer(file_path);
            unordered_map<string, linked_list_entry> hpu = ca.get_HPU();
            int sum_kl = ca.get_sum_kl();
            unordered_map<int, unordered_set<string> > valpos = ca.get_valid_position();
            unordered_map<string, int> semi = ca.get_semi_universal_label();
            //construct spanning forest
            SpanningForest spanningForest;
            spanningForest.generate_spanning_forest_withNoColor(hpu);
            vector<occ_Node> euler_tour_sequence = spanningForest.get_euler_tour_sequence();
            unordered_map<string, array_Node> node_array = spanningForest.get_Node_Array();
            unordered_map<int, unordered_set<graph_Edge, hash_Edge> > tree_edge = spanningForest.get_tree_edge();
            unordered_map<int, unordered_set<graph_Edge, hash_Edge> > non_tree_edge = spanningForest.get_non_tree_edge();
            unordered_map<graph_Edge, int, hash_Edge> all_edges = spanningForest.get_edges();
            Treap treap;
            treap.build_Treap(euler_tour_sequence, &node_array);
            //initialize fct structure
            FastCompatibilityTest fct(hpu, valpos, semi, sum_kl, euler_tour_sequence, &node_array, tree_edge, non_tree_edge,
                                      all_edges, treap.tree_root, ca.get_father_children(), spanningForest.get_query_table(), edge_promotion);
            cout << "Initialization --> finished" << endl;
            cout << "Compatibility Test --> start" << endl;
            time_t start = clock();
            //execution
            int success = fct.build(false);
            time_t end = clock();
            time_t runtime = end - start;

            if(success == 0){
                cout << "Compatibility Test --> ternimate" << endl;
                cout << "Input phylogenetic trees are INCOMPATIBLE!" << endl;
                //print the runtime
                cout << "Compatibility Test --> duration(" << (double)(runtime)/CLOCKS_PER_SEC << " s)" << endl;
            }
            else if(success == 1){
                cout << "Compatibility Test --> finished" << endl;
                cout << "Output result --> start" << endl;
                string result_newick;
                result_newick = fct.convert2newick(fct.get_display_graph(), result_newick);
                result_newick = result_newick.substr(1, result_newick.length() - 12) + ";";
                write2file(result_newick,0);
                cout << "Output result --> finished" << endl;
                //print the runtime
                cout << "Compatibility Test --> duration(" << (double)(runtime)/CLOCKS_PER_SEC << " s)" << endl;
            }
            return 0;
        }
        else if(argc == 5){
            string edge_promotion_str = argv[2];
            bool edge_promotion = false;
            if(edge_promotion_str == "--p"){
                edge_promotion_str = argv[3];
                if(edge_promotion_str == "0"){
                    edge_promotion = false;
                }
                else if(edge_promotion_str == "1"){
                    edge_promotion = true;
                }
                else{
                    cout << "Command \"" << edge_promotion_str << "\" should be either 0 or 1!" << endl;
                    return 0;
                }
            }
            else{
                cout << "Command \"" << edge_promotion_str << "\" is not specified!" << endl;
                return 0;
            }

            string file_path = argv[4];
            cout << file_path << endl;

            //initialization(including parsing the newick format, constructing data structures
            //computing initial valid positions, computing initial semi-universal nodes,
            //building up spanning trees and forest, etc)
            cout << "Initialization --> start" << endl;

            CompatibilityAnalyzer ca;
            //parse newick strings
            //construct HPU, initial valid position, initial semi-universal nodes
            ca.initializer(file_path);
            time_t all_start = clock();
            unordered_map<string, linked_list_entry> hpu = ca.get_HPU();
            int sum_kl = ca.get_sum_kl();
            unordered_map<int, unordered_set<string> > valpos = ca.get_valid_position();
            unordered_map<string, int> semi = ca.get_semi_universal_label();
            //construct spanning forest
            SpanningForest spanningForest;
            spanningForest.generate_spanning_forest_withNoColor(hpu);
            vector<occ_Node> euler_tour_sequence = spanningForest.get_euler_tour_sequence();
            unordered_map<string, array_Node> node_array = spanningForest.get_Node_Array();
            unordered_map<int, unordered_set<graph_Edge, hash_Edge> > tree_edge = spanningForest.get_tree_edge();
            unordered_map<int, unordered_set<graph_Edge, hash_Edge> > non_tree_edge = spanningForest.get_non_tree_edge();
            unordered_map<graph_Edge, int, hash_Edge> all_edges = spanningForest.get_edges();
            Treap treap;
            treap.build_Treap(euler_tour_sequence, &node_array);
            //initialize fct structure
            FastCompatibilityTest fct(hpu, valpos, semi, sum_kl, euler_tour_sequence, &node_array, tree_edge, non_tree_edge,
                                      all_edges, treap.tree_root, ca.get_father_children(), spanningForest.get_query_table(), edge_promotion);
            cout << "Initialization --> finished" << endl;
            cout << "Compatibility Test --> start" << endl;
            time_t start = clock();
            //execution
            int success = fct.build(false);
            time_t end = clock();
            time_t runtime = end - start;
            time_t all_end = clock();
            time_t all_runtime = all_end - all_start;

            if(success == 0){
                cout << "Compatibility Test --> ternimate" << endl;
                cout << "Input phylogenetic trees are INCOMPATIBLE!" << endl;
                //print the runtime
                cout << "Compatibility Test --> duration(" << (double)(runtime)/CLOCKS_PER_SEC << " s)" << endl;
            }
            else if(success == 1){
                cout << "Compatibility Test --> finished" << endl;
                cout << "Output result --> start" << endl;
                string result_newick;
                result_newick = fct.convert2newick(fct.get_display_graph(), result_newick);
                result_newick = result_newick.substr(1, result_newick.length() - 12) + ";";
                write2file(result_newick,0);
                cout << "Output result --> finished" << endl;
                //print the runtime
                cout << "Compatibility Test --> duration(" << (double)(runtime)/CLOCKS_PER_SEC << " s)" << endl;
            }
            return 0;
        }
        else{
            cout << "Parameters are not correct!" << endl;
            return 0;
        }
    }
    else if(input_method == "-s"){
        //simulate phylogenetic trees with input generator
        //-t: simulate all triplets from an arbitrary binary tree
        //-b: simulate binary phylogenetic trees
        //-m: simulate non-binary phylogenetic trees
        //-tp: simulate triplets from a specified binary tree
        if(argc <= 2){
            cout << "Error: Phylogenetic tree type is not specified!" << endl;
            return 0;
        }
        string tree_type = argv[2];

        string number_of_labels_str = "0";
        string number_of_phylogenetic_trees_str = "0";
        string out_degree_str = "0";
        string edge_promotion_str = "0";
        string test_frequency_str = "0";
        string left_percentage_str = "0";
        string portion_str = "0";

        int number_of_labels = 0;
        int number_of_phylogenetic_trees = 0;
        int out_degree = 0;
        bool edge_promotion = false;
        int test_frequency = 30;
        double left_percentage = 0;
        double portion_value = 0;

        //receive other parameters
        if(tree_type == "-tp"){
            //simulate triples
            //must specify: --n the number of labeled nodes
            //must specify: --a the portion of triplets from all triplets
            //must specify: --r the portion of left children, 20 means the amount of left children among all children in this subtree is 20%
            //optional: --p if edge promotion is allowed
            //optional: --f how many test cases will be executed
            if(argc <= 3){
                cout << "Error: Parameters are not specified!" << endl;
                return 0;
            }
            number_of_labels_str = argv[3];
            if(number_of_labels_str == "--n"){
                if(argc <= 4){
                    cout << "Error: Number of labels is not specified!" << endl;
                    return 0;
                }
                number_of_labels_str = argv[4];
                stringstream labels(number_of_labels_str);
                labels >> number_of_labels;

                if(argc <= 5){
                    cout << "Error: Portion of triplets is not specified!" << endl;
                    return 0;
                }
                portion_str = argv[5];
                if(portion_str == "--a"){
                    if(argc <= 6){
                        cout << "Error: The portion value is not specified!" << endl;
                        return 0;
                    }
                    portion_str = argv[6];
                    stringstream portion(portion_str);
                    portion >> portion_value;

                    if(argc <= 7){
                        cout << "Error: Percentage is not specified!" << endl;
                        return 0;
                    }
                    left_percentage_str = argv[7];
                    if(left_percentage_str == "--r"){
                        if(argc <= 8){
                            cout << "Error: The percentage value is not specified!" << endl;
                            return 0;
                        }
                        left_percentage_str = argv[8];
                        stringstream percentage(left_percentage_str);
                        percentage >> left_percentage;

                        if(argc <= 9){
                            cout << "Warning: Edge promotion is prohibited by default!" << endl;
                            edge_promotion = false;
                            cout << "Warning: Default test frequency is 30!" << endl;
                            test_frequency = 30;
                        }
                        else{
                            edge_promotion_str = argv[9];
                            if(edge_promotion_str == "--p"){
                                if(argc <= 10){
                                    cout << "Warning: Edge promotion is prohibited by default!" << endl;
                                    edge_promotion = false;
                                }
                                else{
                                    edge_promotion_str = argv[10];
                                    if(edge_promotion_str == "1"){
                                        edge_promotion = true;
                                    }
                                    else if(edge_promotion_str == "0"){
                                        edge_promotion = false;
                                    }
                                    else{
                                        cout << "Error: Value of edge promotion should be binary(0 or 1)!" << endl;
                                        return 0;
                                    }
                                }
                            }
                            else{
                                cout << "Error: Command \"" << edge_promotion_str << "\" is not specified!" << endl;
                                return 0;
                            }

                            if(argc == 13){
                                //frequency is specified
                                test_frequency_str = argv[11];
                                if(test_frequency_str == "--f"){
                                    test_frequency_str = argv[12];
                                    stringstream frequency(test_frequency_str);
                                    frequency >> test_frequency;
                                }
                                else{
                                    cout << "Error: Command \"" << test_frequency_str << "\" is not specified!" << endl;
                                    return 0;
                                }
                            }
                            else if(argc == 12){
                                test_frequency_str = argv[11];
                                if(test_frequency_str == "--f"){
                                    cout << "Warning: Default test frequency is 30!" << endl;
                                    test_frequency = 30;
                                }
                                else{
                                    cout << "Error: Command is incorrect!" << endl;
                                    return 0;
                                }
                            }
                            else if(argc > 13){
                                cout << "Error: Too many parameters!" << endl;
                                return 0;
                            }
                        }
                    }
                }
            }
            else{
                cout << "Error: Command \"" << number_of_labels_str << "\" is not specified!" << endl;
                return 0;
            }

            time_t exec_acc_time = 0;
            time_t deletion_time = 0;
            time_t hdt_deletion = 0;

            cout << "Number of tests will be executed --> " << test_frequency << endl;
            for(int i = 0; i < test_frequency; i++) {

                cout << "Test " << i + 1 << ":" << endl;
                cout << "\tInitialization --> start" << endl;
                //initialization
                InputGenerator inputGenerator;

                bool internal_labeled = false;
                multimap<string, unordered_map<string, linked_list_entry> > input_trees = inputGenerator.construct_specifiedTriples(number_of_labels, portion_value, left_percentage);

                //construct HPU
                unordered_map<string, linked_list_entry> hpu = inputGenerator.get_global_hpu();

                //compute initial semi-universal nodes and valid position
                Linked_List_Builder list_builder;
                list_builder.merge_input_trees(input_trees, hpu);
                int sum_kl = list_builder.get_sum_kl();
                unordered_map<int, unordered_set<string> > valpos = list_builder.get_valid_position();
                unordered_map<string, int> semi = list_builder.get_semi_universal_nodes();

                //construct spanning forest
                SpanningForest spanningForest;
                //thinking about how to improve its efficiency
                spanningForest.generate_spanning_forest_withNoColor(hpu);
                vector<occ_Node> euler_tour_sequence = spanningForest.get_euler_tour_sequence();
                unordered_map<string, array_Node> node_array = spanningForest.get_Node_Array();
                unordered_map<int, unordered_set<graph_Edge, hash_Edge> > tree_edge = spanningForest.get_tree_edge();
                unordered_map<int, unordered_set<graph_Edge, hash_Edge> > non_tree_edge = spanningForest.get_non_tree_edge();
                unordered_map<int, unordered_map<string, unordered_set<graph_Edge, hash_Edge> > > query_table = spanningForest.get_query_table();
                unordered_map<graph_Edge, int, hash_Edge> all_edges = spanningForest.get_edges();
                Treap treap;
                treap.build_Treap(euler_tour_sequence, &node_array);

                //initialize FCT data structure
                FastCompatibilityTest fct(hpu, valpos, semi, sum_kl, euler_tour_sequence, &node_array, tree_edge, non_tree_edge,
                                          all_edges, treap.tree_root, inputGenerator.get_father_children(), query_table, edge_promotion);
                cout << "\tInitialization --> finished" << endl;
                cout << "\tCompatibility test --> start" << endl;

                time_t start = clock();
                int success = fct.build(internal_labeled);
                time_t end = clock();

                if(success == 0){
                    cout << "\tCompatibility test --> terminate" << endl;
                    cout << "\tThe simulated trees are INCOMPATIBLE" << endl;
                    break;
                }
                else if(success == 1){
                    cout << "\tCompatibility test --> finished" << endl;
                    cout << "\tOutput result --> start" << endl;
                    dis_Node* display_graph = fct.get_display_graph();
                    string newick_string;
                    newick_string = fct.convert2newick(display_graph, newick_string);
                    cout << "\tOutput result --> finished" << endl;
                    newick_string = newick_string.substr(1, newick_string.length() - 12) + ";";
                    write2file(newick_string, i);
                }

                cout << "Compatibility Test " << i + 1 << " --> duration(" << (double)(end - start)/CLOCKS_PER_SEC << " s)" << endl;
                exec_acc_time += end - start;

                multimap<string, unordered_map<string, linked_list_entry> > empty_input_tree;
                swap(empty_input_tree, input_trees);
                unordered_map<string, linked_list_entry> empty_hpu;
                swap(empty_hpu, hpu);
                unordered_map<int, unordered_set<string> > empty_valid_position;
                swap(empty_valid_position, valpos);
                unordered_map<string, int> empty_semi;
                swap(empty_semi, semi);
                vector<occ_Node> empty_euler;
                swap(empty_euler, euler_tour_sequence);
                unordered_map<string, array_Node> empty_node_array;
                swap(empty_node_array, node_array);
                unordered_map<int, unordered_set<graph_Edge, hash_Edge> > empty_tree_edge;
                unordered_map<int, unordered_set<graph_Edge, hash_Edge> > empty_non_tree_edge;
                swap(empty_tree_edge, tree_edge);
                swap(empty_non_tree_edge, non_tree_edge);
                unordered_map<int, unordered_map<string, unordered_set<graph_Edge, hash_Edge> > > empty_query_table;
                swap(empty_query_table, query_table);
                unordered_map<graph_Edge, int, hash_Edge> empty_all_edges;
                swap(empty_all_edges, all_edges);
            }
            cout << "Compatibility Test --> duration(" << (double)(exec_acc_time/test_frequency)/CLOCKS_PER_SEC << " s)" << endl;
            cout << "Deletion takes --> duration(" << (double)(deletion_time/test_frequency)/CLOCKS_PER_SEC << " s)" << endl;
            cout << "HDT Deletion takes --> duration(" << (double)(hdt_deletion/test_frequency)/CLOCKS_PER_SEC << " s)" << endl;

            //why fast
//            cout << "average actual scanned edges: " << wanna_know_actual_edge/test_frequency << endl;
//            cout << "average possible scanned edges: " << wanna_know_possible_edge/test_frequency << endl;
//            cout << "component size 1: " << size_1/test_frequency << endl;
//            cout << "component size 2: " << size_2/test_frequency << endl;
//            cout << "component size 3: " << size_3/test_frequency << endl;
//            cout << "component size 4: " << size_4/test_frequency << endl;
//            cout << "component size 5: " << size_5/test_frequency << endl;
//            cout << "component size 6: " << size_6/test_frequency << endl;
//            cout << "component size 7: " << size_7/test_frequency << endl;
//            cout << "component size 8: " << size_8/test_frequency << endl;
//            cout << "component size over 8: " << size_over_8/test_frequency << endl;
//            cout << "number of non-tree edges being deleted: " << (double)numberOfDeletedNonTreeEdge/test_frequency << endl;
//            cout << "number of tree edges being deleted: " << (double)numberOfDeletedTreeEdge/test_frequency << endl;
//            cout << "time spent on deleting non-tree edges: " << (double)deletedNonTreeTime/test_frequency/CLOCKS_PER_SEC << endl;
//            cout << "double check all edges: " << (double)doubleCheckEdge/test_frequency << endl;

            return 0;
        }
        else if(tree_type == "-t"){
            //simulate triples
            //must specify: --n the number of labeled nodes
            //optional: --p if edge promotion is allowed
            //optional: --f how many test cases will be executed
            if(argc <= 3){
                cout << "Error: Parameters are not specified!" << endl;
                return 0;
            }
            number_of_labels_str = argv[3];
            if(number_of_labels_str == "--n"){
                if(argc <= 4){
                    cout << "Error: Number of labels is not specified!" << endl;
                    return 0;
                }
                number_of_labels_str = argv[4];
                stringstream labels(number_of_labels_str);
                labels >> number_of_labels;

                if(argc <= 5){
                    cout << "Warning: Edge promotion is prohibited by default!" << endl;
                    edge_promotion = false;
                    cout << "Warning: Default test frequency is 30!" << endl;
                    test_frequency = 30;
                }
                else{
                    edge_promotion_str = argv[5];
                    if(edge_promotion_str == "--p"){
                        if(argc <= 6){
                            cout << "Warning: Edge promotion is prohibited by default!" << endl;
                            edge_promotion = false;
                        }
                        else{
                            edge_promotion_str = argv[6];
                            if(edge_promotion_str == "1"){
                                edge_promotion = true;
                            }
                            else if(edge_promotion_str == "0"){
                                edge_promotion = false;
                            }
                            else{
                                cout << "Error: Value of edge promotion should be binary(0 or 1)!" << endl;
                                return 0;
                            }
                        }
                    }
                    else{
                        cout << "Error: Command \"" << edge_promotion_str << "\" is not specified!" << endl;
                        return 0;
                    }

                    if(argc == 9){
                        //frequency is specified
                        test_frequency_str = argv[7];
                        if(test_frequency_str == "--f"){
                            test_frequency_str = argv[8];
                            stringstream frequency(test_frequency_str);
                            frequency >> test_frequency;
                        }
                        else{
                            cout << "Error: Command \"" << test_frequency_str << "\" is not specified!" << endl;
                            return 0;
                        }
                    }
                    else if(argc == 8){
                        test_frequency_str = argv[7];
                        if(test_frequency_str == "--f"){
                            cout << "Warning: Default test frequency is 30!" << endl;
                            test_frequency = 30;
                        }
                    }
                    else if(argc > 9){
                        cout << "Error: Too many parameters!" << endl;
                        return 0;
                    }
                }
            }
            else{
                cout << "Error: Command \"" << number_of_labels_str << "\" is not specified!" << endl;
                return 0;
            }

            time_t exec_acc_time = 0;
            time_t deletion_time = 0;
            time_t hdt_deletion = 0;

            long numberOfDeletedNonTreeEdge = 0;
            long numberOfDeletedTreeEdge = 0;
            long doubleCheckEdge = 0;
            time_t deletedNonTreeTime = 0;


            cout << "Number of tests will be executed --> " << test_frequency << endl;
            for(int i = 0 ;i < test_frequency; i++){

                cout << "Test " << i + 1 << ":" << endl;
                cout << "\tInitialization --> start" << endl;
                //initialization
                InputGenerator inputGenerator;

                //construct triples and resulting graphs
                multimap<string, unordered_map<string, linked_list_entry> > input_trees = inputGenerator.construct_triples(number_of_labels);
                bool internal_labeled = false;

                //construct HPU
                unordered_map<string, linked_list_entry> hpu = inputGenerator.get_global_hpu();

                //compute initial semi-universal nodes and valid position
                Linked_List_Builder list_builder;
                list_builder.merge_input_trees(input_trees, hpu);
                int sum_kl = list_builder.get_sum_kl();
                unordered_map<int, unordered_set<string> > valpos = list_builder.get_valid_position();
                unordered_map<string, int> semi = list_builder.get_semi_universal_nodes();

                //construct spanning forest
                SpanningForest spanningForest;
                spanningForest.generate_spanning_forest_withNoColor(hpu);
                vector<occ_Node> euler_tour_sequence = spanningForest.get_euler_tour_sequence();
                unordered_map<string, array_Node> node_array = spanningForest.get_Node_Array();
                unordered_map<int, unordered_set<graph_Edge, hash_Edge> > tree_edge = spanningForest.get_tree_edge();
                unordered_map<int, unordered_set<graph_Edge, hash_Edge> > non_tree_edge = spanningForest.get_non_tree_edge();
                unordered_map<int, unordered_map<string, unordered_set<graph_Edge, hash_Edge> > > query_table = spanningForest.get_query_table();
                unordered_map<graph_Edge, int, hash_Edge> all_edges = spanningForest.get_edges();
                Treap treap;
                treap.build_Treap(euler_tour_sequence, &node_array);

                //initialize FCT data structure
                FastCompatibilityTest fct(hpu, valpos, semi, sum_kl, euler_tour_sequence, &node_array, tree_edge, non_tree_edge,
                                          all_edges, treap.tree_root, inputGenerator.get_father_children(), query_table, edge_promotion);
                cout << "\tInitialization --> finished" << endl;
                cout << "\tCompatibility test --> start" << endl;

                time_t start = clock();
                int success = fct.build(internal_labeled);
                time_t end = clock();

                if(success == 0){
                    cout << "\tCompatibility test --> terminate" << endl;
                    cout << "\tThe simulated trees are INCOMPATIBLE" << endl;
                    break;
                }
                else if(success == 1){
                    cout << "\tCompatibility test --> finished" << endl;
                    cout << "\tOutput result --> start" << endl;
                    dis_Node* display_graph = fct.get_display_graph();
                    string newick_string;
                    newick_string = fct.convert2newick(display_graph, newick_string);
                    cout << "\tOutput result --> finished" << endl;
                    newick_string = newick_string.substr(1, newick_string.length() - 12) + ";";
                    write2file(newick_string, i);
                }

                cout << "Compatibility Test " << i + 1 << " --> duration(" << (double)(end - start)/CLOCKS_PER_SEC << " s)" << endl;
                exec_acc_time += end - start;

                multimap<string, unordered_map<string, linked_list_entry> > empty_input_tree;
                swap(empty_input_tree, input_trees);
                unordered_map<string, linked_list_entry> empty_hpu;
                swap(empty_hpu, hpu);
                unordered_map<int, unordered_set<string> > empty_valid_position;
                swap(empty_valid_position, valpos);
                unordered_map<string, int> empty_semi;
                swap(empty_semi, semi);
                vector<occ_Node> empty_euler;
                swap(empty_euler, euler_tour_sequence);
                unordered_map<string, array_Node> empty_node_array;
                swap(empty_node_array, node_array);
                unordered_map<int, unordered_set<graph_Edge, hash_Edge> > empty_tree_edge;
                unordered_map<int, unordered_set<graph_Edge, hash_Edge> > empty_non_tree_edge;
                swap(empty_tree_edge, tree_edge);
                swap(empty_non_tree_edge, non_tree_edge);
                unordered_map<int, unordered_map<string, unordered_set<graph_Edge, hash_Edge> > > empty_query_table;
                swap(empty_query_table, query_table);
                unordered_map<graph_Edge, int, hash_Edge> empty_all_edges;
                swap(empty_all_edges, all_edges);
            }
            cout << "Compatibility Test --> duration(" << (double)(exec_acc_time/test_frequency)/CLOCKS_PER_SEC << " s)" << endl;

            return 0;
        }
        else if(tree_type == "-b"){
            //simulate binary phylogenetic trees
            //must specify: --n the number of labeled nodes
            //must specify: --k the numebr of phylogenetic trees
            //must specify: --l internally labeled or not
            //optional: --p if edge promotion is allowed
            //optional: --f how many test cases will be executed
            string internal_label_str = "0";
            bool internal_label = false;

            if(argc <= 3){
                cout << "Error: Parameters are not specified!" << endl;
                return 0;
            }

            number_of_labels_str = argv[3];
            if(number_of_labels_str == "--n"){
                if(argc <= 4){
                    cout << "Error: Parameters are not specified!" << endl;
                    return 0;
                }

                number_of_labels_str = argv[4];
                stringstream labels(number_of_labels_str);
                labels >> number_of_labels;

                if(number_of_labels <= 2){
                    cout << "Number of labels is too small!" << endl;
                    return 0;
                }

                if(argc <= 5){
                    cout << "Error: Pararmeters are not specified!" << endl;
                    return 0;
                }

                number_of_phylogenetic_trees_str = argv[5];
                if(number_of_phylogenetic_trees_str == "--k"){
                    if(argc <= 6){
                        cout << "Error: Parameters are not specified!" << endl;
                        return 0;
                    }

                    number_of_phylogenetic_trees_str = argv[6];
                    stringstream trees(number_of_phylogenetic_trees_str);
                    trees >> number_of_phylogenetic_trees;

                    if(argc <= 7){
                        cout << "Error: Parameters are not specified!" << endl;
                        return 0;
                    }

                    internal_label_str = argv[7];
                    if(internal_label_str == "--l"){
                        if(argc <= 8){
                            cout << "Error: Parameters are not specified!" << endl;
                            return 0;
                        }

                        internal_label_str = argv[8];
                        if(internal_label_str == "0"){
                            internal_label = false;
                        }
                        else if(internal_label_str == "1"){
                            internal_label = true;
                        }
                        else{
                            cout << "Error: Parameters are not correct!" << endl;
                            return 0;
                        }

                        if(argc <= 9){
                            cout << "Warning: Edge promotion is prohibited by default!" << endl;
                            edge_promotion = false;
                            cout << "Warning: Default test frequency is 30!" << endl;
                            test_frequency = 30;
                        }
                        else{
                            edge_promotion_str = argv[9];
                            if(edge_promotion_str == "--p"){
                                if(argc <= 10){
                                    cout << "Warning: Edge promotion is prohibited by default!" << endl;
                                    edge_promotion = false;
                                }
                                else{
                                    edge_promotion_str = argv[10];
                                    if(edge_promotion_str == "1"){
                                        edge_promotion = true;
                                    }
                                    else if(edge_promotion_str == "0"){
                                        edge_promotion = false;
                                    }
                                    else{
                                        cout << "Error: Value of edge promotion should be binary(0 or 1)!" << endl;
                                        return 0;
                                    }
                                }
                            }
                            else{
                                cout << "Error: Command \"" << edge_promotion_str << "\" is not specified!" << endl;
                                return 0;
                            }

                            if(argc == 13){
                                //frequency is specified
                                test_frequency_str = argv[11];
                                if(test_frequency_str == "--f"){
                                    test_frequency_str = argv[12];
                                    stringstream frequency(test_frequency_str);
                                    frequency >> test_frequency;
                                }
                                else{
                                    cout << "Error: Command \"" << test_frequency_str << "\" is not specified!" << endl;
                                    return 0;
                                }
                            }
                            else if(argc == 12){
                                test_frequency_str = argv[11];
                                if(test_frequency_str == "--f"){
                                    cout << "Warning: Default test frequency is 30!" << endl;
                                    test_frequency = 30;
                                }
                            }
                            else if(argc > 13){
                                cout << "Error: Too many parameters!" << endl;
                                return 0;
                            }
                        }
                    }
                    else{
                        cout << "Command \"" << internal_label_str << "\" is not specified!" << endl;
                        return 0;
                    }
                }
                else{
                    cout << "Error: Command \"" << number_of_phylogenetic_trees_str << "\" is not specified" << endl;
                    return 0;
                }
            }
            else{
                cout << "Error: Command \"" << number_of_labels_str << "\" is not specified!" << endl;
                return 0;
            }

            time_t exec_acc_time = 0;
//            time_t deletion_time = 0;
            time_t hdt_deletion = 0;

            cout << "Number of tests will be executed --> " << test_frequency << endl;
            for(int i = 0 ;i < test_frequency; i++){

                cout << "Test " << i + 1 << ":" << endl;
                cout << "\tInitialization --> start" << endl;
                //initialization
                InputGenerator inputGenerator;

                //construct triples and resulting graphs
                multimap<string, unordered_map<string, linked_list_entry> > input_trees;
                bool internal_labeled;
                if(internal_label){
                    input_trees = inputGenerator.construct_tree_with_internal_label(number_of_labels,number_of_phylogenetic_trees);
                    internal_labeled = inputGenerator.get_internal_label();
                }
                else{
                    input_trees = inputGenerator.construct_tree_leaf_label_only(number_of_labels,number_of_phylogenetic_trees);
                    internal_labeled = false;
                }

                //construct HPU
                unordered_map<string, linked_list_entry> hpu = inputGenerator.get_global_hpu();

                //compute initial semi-universal nodes and valid position
                Linked_List_Builder list_builder;
                list_builder.merge_input_trees(input_trees, hpu);
                int sum_kl = list_builder.get_sum_kl();
                unordered_map<int, unordered_set<string> > valpos = list_builder.get_valid_position();
                unordered_map<string, int> semi = list_builder.get_semi_universal_nodes();

                //construct spanning forest
                SpanningForest spanningForest;
                spanningForest.generate_spanning_forest_withNoColor(hpu);
                vector<occ_Node> euler_tour_sequence = spanningForest.get_euler_tour_sequence();
                unordered_map<string, array_Node> node_array = spanningForest.get_Node_Array();
                unordered_map<int, unordered_set<graph_Edge, hash_Edge> > tree_edge = spanningForest.get_tree_edge();
                unordered_map<int, unordered_set<graph_Edge, hash_Edge> > non_tree_edge = spanningForest.get_non_tree_edge();
                unordered_map<int, unordered_map<string, unordered_set<graph_Edge, hash_Edge> > > query_table = spanningForest.get_query_table();
                unordered_map<graph_Edge, int, hash_Edge> all_edges = spanningForest.get_edges();
                Treap treap;
                treap.build_Treap(euler_tour_sequence, &node_array);

                //initialize FCT data structure
                FastCompatibilityTest fct(hpu, valpos, semi, sum_kl, euler_tour_sequence, &node_array, tree_edge, non_tree_edge,
                                          all_edges, treap.tree_root, inputGenerator.get_father_children(), query_table, edge_promotion);
                cout << "\tInitialization --> finished" << endl;
                cout << "\tCompatibility test --> start" << endl;

                time_t start = clock();
                int success = fct.build(internal_labeled);
                time_t end = clock();

                if(success == 0){
                    cout << "\tCompatibility test --> terminate" << endl;
                    cout << "\tThe simulated trees are INCOMPATIBLE" << endl;
                    break;
                }
                else if(success == 1){
                    cout << "\tCompatibility test --> finished" << endl;
                    cout << "\tOutput result --> start" << endl;
                    dis_Node* display_graph = fct.get_display_graph();
                    string newick_string;
                    newick_string = fct.convert2newick(display_graph, newick_string);
                    cout << "\tOutput result --> finished" << endl;
                    newick_string = newick_string.substr(1, newick_string.length() - 12) + ";";
                    write2file(newick_string, i);
                }

                exec_acc_time += end - start;

                multimap<string, unordered_map<string, linked_list_entry> > empty_input_tree;
                swap(empty_input_tree, input_trees);
                unordered_map<string, linked_list_entry> empty_hpu;
                swap(empty_hpu, hpu);
                unordered_map<int, unordered_set<string> > empty_valid_position;
                swap(empty_valid_position, valpos);
                unordered_map<string, int> empty_semi;
                swap(empty_semi, semi);
                vector<occ_Node> empty_euler;
                swap(empty_euler, euler_tour_sequence);
                unordered_map<string, array_Node> empty_node_array;
                swap(empty_node_array, node_array);
                unordered_map<int, unordered_set<graph_Edge, hash_Edge> > empty_tree_edge;
                unordered_map<int, unordered_set<graph_Edge, hash_Edge> > empty_non_tree_edge;
                swap(empty_tree_edge, tree_edge);
                swap(empty_non_tree_edge, non_tree_edge);
                unordered_map<int, unordered_map<string, unordered_set<graph_Edge, hash_Edge> > > empty_query_table;
                swap(empty_query_table, query_table);
                unordered_map<graph_Edge, int, hash_Edge> empty_all_edges;
                swap(empty_all_edges, all_edges);
            }
            cout << "Compatibility Test --> duration(" << (double)(exec_acc_time/test_frequency)/CLOCKS_PER_SEC << " s)" << endl;
            return 0;
        }
        else if(tree_type == "-m"){
            //simulate non-binary phylogenetic trees
            //must specify: --n the number of labeled nodes
            //must specify: --k the numebr of phylogenetic trees
            //must specify: --d the out-degree
            //optional: --p if edge promotion is allowed
            //optional: --f how many test cases will be executed
            if(argc <= 3){
                cout << "Error: Parameters are not specified!" << endl;
                return 0;
            }

            number_of_labels_str = argv[3];
            if(number_of_labels_str == "--n"){
                if(argc <= 4){
                    cout << "Error: Parameters are not specified!" << endl;
                    return 0;
                }

                number_of_labels_str = argv[4];
                stringstream labels(number_of_labels_str);
                labels >> number_of_labels;

                if(number_of_labels <= 2){
                    cout << "Numebr of nodes is not small!" << endl;
                    return 0;
                }

                if(argc <= 5){
                    cout << "Error: Parameters are not specified!" << endl;
                    return 0;
                }

                number_of_phylogenetic_trees_str = argv[5];
                if(number_of_phylogenetic_trees_str == "--k"){
                    if(argc <= 6){
                        cout << "Error: Parameters are not specified!" << endl;
                        return 0;
                    }

                    number_of_phylogenetic_trees_str = argv[6];
                    stringstream trees(number_of_phylogenetic_trees_str);
                    trees >> number_of_phylogenetic_trees;

                    if(argc <= 7){
                        cout <<"Error: Parameters are not specified!" << endl;
                        return 0;
                    }


                    out_degree_str = argv[7];
                    if(out_degree_str == "--d"){
                        if(argc <= 8){
                            cout << "Error: Parameters are not specified!" << endl;
                            return 0;
                        }

                        out_degree_str = argv[8];
                        stringstream degree(out_degree_str);
                        degree >> out_degree;

                        if(out_degree == 2){
                            cout << "Warning: Please use -b command to simulate binary phylogenetic trees" << endl;
                            return 0;
                        }
                        else if(out_degree < 2){
                            cout << "Warnning: Parameter is illegal!" << endl;
                            return 0;
                        }

                        if(number_of_labels < out_degree){
                            cout << "Error: out-degree is illegal!" << endl;
                            return 0;
                        }

                        if(argc <= 9){
                            cout << "Warning: Edge promotion is prohibited by default!" << endl;
                            edge_promotion = false;
                            cout << "Warning: Default test frequency is 30!" << endl;
                            test_frequency = 30;
                        }
                        else{
                            edge_promotion_str = argv[9];
                            if(edge_promotion_str == "--p"){
                                if(argc <= 10){
                                    cout << "Warning: Edge promotion is prohibited by default!" << endl;
                                    edge_promotion = false;
                                }
                                else{
                                    edge_promotion_str = argv[10];
                                    if(edge_promotion_str == "1"){
                                        edge_promotion = true;
                                    }
                                    else if(edge_promotion_str == "0"){
                                        edge_promotion = false;
                                    }
                                    else{
                                        cout << "Error: Value of edge promotion should be binary(0 or 1)!" << endl;
                                        return 0;
                                    }
                                }
                            }
                            else{
                                cout << "Error: Command \"" << edge_promotion_str << "\" is not specified!" << endl;
                                return 0;
                            }

                            if(argc == 13){
                                //frequency is specified
                                test_frequency_str = argv[11];
                                if(test_frequency_str == "--f"){
                                    test_frequency_str = argv[12];
                                    stringstream frequency(test_frequency_str);
                                    frequency >> test_frequency;
                                }
                                else{
                                    cout << "Error: Command \"" << test_frequency_str << "\" is not specified!" << endl;
                                    return 0;
                                }
                            }
                            else if(argc == 12){
                                test_frequency_str = argv[11];
                                if(test_frequency_str == "--f"){
                                    cout << "Warning: Default test frequency is 30!" << endl;
                                    test_frequency = 30;
                                }
                            }
                            else if(argc > 13){
                                cout << "Error: Too many parameters!" << endl;
                                return 0;
                            }
                        }
                    }
                    else{
                        cout << "Command \"" << out_degree_str << "\" is not specified!" << endl;
                        return 0;
                    }
                }
                else{
                    cout << "Error: Command \"" << number_of_phylogenetic_trees_str << "\" is not specified" << endl;
                    return 0;
                }
            }
            else{
                cout << "Error: Command \"" << number_of_labels_str << "\" is not specified!" << endl;
                return 0;
            }

            //execution
            time_t exec_acc_time = 0;
            time_t deletion_time = 0;
            time_t hdt_deletion = 0;
            time_t upgrade_time = 0;
            time_t update_semi = 0;

            cout << "Number of tests will be executed --> " << test_frequency << endl;
            for(int i = 0 ;i < test_frequency; i++){

                cout << "Test " << i + 1 << ":" << endl;
                cout << "\tInitialization --> start" << endl;
                //initialization
                InputGenerator inputGenerator;

                //construct triples and resulting graphs
                multimap<string, unordered_map<string, linked_list_entry> > input_trees = inputGenerator.construct_non_binary_degree_tree(number_of_labels,number_of_phylogenetic_trees,out_degree);
                bool internal_labeled = false;

                //construct HPU
                unordered_map<string, linked_list_entry> hpu = inputGenerator.get_global_hpu();

                //compute initial semi-universal nodes and valid position
                Linked_List_Builder list_builder;
                list_builder.merge_input_trees(input_trees, hpu);
                int sum_kl = list_builder.get_sum_kl();
                unordered_map<int, unordered_set<string> > valpos = list_builder.get_valid_position();
                unordered_map<string, int> semi = list_builder.get_semi_universal_nodes();

                //construct spanning forest
                SpanningForest spanningForest;
                spanningForest.generate_spanning_forest_withNoColor(hpu);
                vector<occ_Node> euler_tour_sequence = spanningForest.get_euler_tour_sequence();
                unordered_map<string, array_Node> node_array = spanningForest.get_Node_Array();
                unordered_map<int, unordered_set<graph_Edge, hash_Edge> > tree_edge = spanningForest.get_tree_edge();
                unordered_map<int, unordered_set<graph_Edge, hash_Edge> > non_tree_edge = spanningForest.get_non_tree_edge();
                unordered_map<int, unordered_map<string, unordered_set<graph_Edge, hash_Edge> > > query_table = spanningForest.get_query_table();
                unordered_map<graph_Edge, int, hash_Edge> all_edges = spanningForest.get_edges();
                Treap treap;
                treap.build_Treap(euler_tour_sequence, &node_array);

                //initialize FCT data structure
                FastCompatibilityTest fct(hpu, valpos, semi, sum_kl, euler_tour_sequence, &node_array, tree_edge, non_tree_edge,
                                          all_edges, treap.tree_root, inputGenerator.get_father_children(), query_table, edge_promotion);
                cout << "\tInitialization --> finished" << endl;
                cout << "\tCompatibility test --> start" << endl;

                time_t start = clock();
                int success = fct.build(internal_labeled);
                time_t end = clock();

                if(success == 0){
                    cout << "\tCompatibility test --> terminate" << endl;
                    cout << "\tThe simulated trees are INCOMPATIBLE" << endl;
                    break;
                }
                else if(success == 1){
                    cout << "\tCompatibility test --> finished" << endl;
                    cout << "\tOutput result --> start" << endl;
                    dis_Node* display_graph = fct.get_display_graph();
                    string newick_string;
                    newick_string = fct.convert2newick(display_graph, newick_string);
                    cout << "\tOutput result --> finished" << endl;
                    newick_string = newick_string.substr(1, newick_string.length() - 12) + ";";
                    write2file(newick_string, i);
                }

                exec_acc_time += end - start;

                multimap<string, unordered_map<string, linked_list_entry> > empty_input_tree;
                swap(empty_input_tree, input_trees);
                unordered_map<string, linked_list_entry> empty_hpu;
                swap(empty_hpu, hpu);
                unordered_map<int, unordered_set<string> > empty_valid_position;
                swap(empty_valid_position, valpos);
                unordered_map<string, int> empty_semi;
                swap(empty_semi, semi);
                vector<occ_Node> empty_euler;
                swap(empty_euler, euler_tour_sequence);
                unordered_map<string, array_Node> empty_node_array;
                swap(empty_node_array, node_array);
                unordered_map<int, unordered_set<graph_Edge, hash_Edge> > empty_tree_edge;
                unordered_map<int, unordered_set<graph_Edge, hash_Edge> > empty_non_tree_edge;
                swap(empty_tree_edge, tree_edge);
                swap(empty_non_tree_edge, non_tree_edge);
                unordered_map<int, unordered_map<string, unordered_set<graph_Edge, hash_Edge> > > empty_query_table;
                swap(empty_query_table, query_table);
                unordered_map<graph_Edge, int, hash_Edge> empty_all_edges;
                swap(empty_all_edges, all_edges);
            }
            cout << "Compatibility Test --> duration(" << ((double)exec_acc_time/test_frequency)/CLOCKS_PER_SEC << " s)" << endl;

            return 0;
        }
        else{
            cout << "Error: Command \""<< tree_type <<"\" is not specified!" << endl;
            return 0;
        }
    }
    else if(input_method == "-v"){
        //./CT -v filepath --p 0 --f 30
        if(argc == 7){
            ofstream file("result.txt");
            if(file.is_open()){
                file.close();
            }
            string baseFileName = argv[2];
            string edgePromotionStr = argv[4];
            string frequencyStr = argv[6];

            bool edgePromotion = edgePromotionStr == "1";
            stringstream freq(frequencyStr);
            int frequency = 0;
            freq >> frequency;

            time_t compatibilityExecution = 0;
            long numberOfEdges = 0;

            for(int i = 0; i < frequency; i++){
                string file_path = baseFileName + to_string(i+1);
                cout << file_path << endl;

                //initialization(including parsing the newick format, constructing data structures
                //computing initial valid positions, computing initial semi-universal nodes,
                //building up spanning trees and forest, etc)
                cout << "Initialization --> start" << endl;

                CompatibilityAnalyzer ca;
                //parse newick strings
                //construct HPU, initial valid position, initial semi-universal nodes
                ca.initializer(file_path);
                int sum_kl = ca.get_sum_kl();
                //construct spanning forest
                SpanningForest spanningForest;
                spanningForest.generate_spanning_forest_withNoColor(ca.get_HPU());
                unordered_map<string, array_Node> node_array = spanningForest.get_Node_Array();

                Treap treap;
                treap.build_Treap(spanningForest.get_euler_tour_sequence(), &node_array);

                //initialize fct structure
                FastCompatibilityTest fct(ca.get_HPU(), ca.get_valid_position(), ca.get_semi_universal_label(), sum_kl,
                        spanningForest.get_euler_tour_sequence(), &node_array,
                        spanningForest.get_tree_edge(), spanningForest.get_non_tree_edge(),
                                          spanningForest.get_edges(), treap.tree_root, ca.get_father_children(),
                                          spanningForest.get_query_table(), edgePromotion);

                cout << "Initialization --> finished" << endl;
                cout << "Compatibility Test --> start" << endl;
                time_t start = clock();
                //execution
                int success = fct.build(false);
                time_t end = clock();
                time_t runtime = end - start;
                compatibilityExecution += runtime;

                if(success == 0){
                    cout << "Compatibility Test --> ternimate" << endl;
                    cout << "Input phylogenetic trees are INCOMPATIBLE!" << endl;
                    //print the runtime
                    cout << "Compatibility Test --> duration(" << (double)(runtime)/CLOCKS_PER_SEC << " s)" << endl;
                }
                else if(success == 1){
                    cout << "Compatibility Test --> finished" << endl;
                    cout << "Output result --> start" << endl;
                    string result_newick;
                    result_newick = fct.convert2newick(fct.get_display_graph(), result_newick);
                    result_newick = result_newick.substr(1, result_newick.length() - 12) + ";";
                    write2file(result_newick,1);
                    cout << "Output result --> finished" << endl;
                    //print the runtime
                    cout << "Compatibility Test --> duration(" << (double)(runtime)/CLOCKS_PER_SEC << " s)" << endl;
                }
            }

            cout << "Compatibility Test takes " << (double)(compatibilityExecution)/frequency/CLOCKS_PER_SEC << " s" << endl;

            return 0;
        }
        else{
            cout << "Error: Command \"" << input_method << "\" is not specified!" << endl;
            return 0;
        }
    }
    else{
        cout << "Error: Command \""<< input_method <<"\" is not specified!" << endl;
        return 0;
    }

    return 0;
}