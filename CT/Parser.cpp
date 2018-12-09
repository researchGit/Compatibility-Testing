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
// Detailed implementation of Parser.h
//

#include "Parser.h"
#include <fstream>
#include <sstream>

using namespace std;

//Take a newick format string as input, and then convert it to a tree
//when encounter a new label, this method will first labels it with 'temp_xxx' like label
//if this node is labeled by the string, which means that the node has its own label
//this method will replace the temporary label with its own label
//the counter of the temporary label will not be corrected, it will not influence the correctness of the method
dis_tree Parser::parse_label(string newick_str) {
    int length = (int)newick_str.length();
    string str1 = "";
    dis_tree *new_node;

    dis_tree *ptr = new dis_tree();
    ptr->label = "dummy root";
    ptr->parent = NULL;

    dis_tree *pmark = ptr;//pmark is a pointer pointing to a parent node
    bool create_label = false;

    for(int i = 0; i < length; i++){
        char c = newick_str[i];
        cout << c << endl;
        if(c == '(')
        {
            str1 = "";
            new_node = new dis_tree();
            new_node->parent = pmark;
            //convert a int variable to a string variable
            stringstream ss;
            ss << label_counter++;
            new_node->label = "temp_" + ss.str();
            new_node->parent->chd.insert(new_node);
            pmark = new_node;
        }
        else if(c == ','){
            if(!create_label)
            {
                //if the parser finds a new label
                new_node = new dis_tree();
                //extract label and corresponding weight of that label
                vector<string> temp_vec = get_label_weight(str1);
                new_node->parent = pmark;
                new_node->label = temp_vec[0];
                pmark->chd.insert(new_node);
                str1 = "";
            }
            else{
                vector<string> temp_vec = get_label_weight(str1);
                pmark->label = temp_vec[0];
                pmark = pmark->parent;
                str1 = "";
                create_label = false;
            }
        }
        else if(c == ')'){
            if(create_label){
                vector<string> temp_vec = get_label_weight(str1);
                pmark->label = temp_vec[0];
                pmark = pmark->parent;
                str1 = "";
            }
            else{
                new_node = new dis_tree();
                vector<string> str = get_label_weight(str1);
                new_node->parent = pmark;
                new_node->label = str[0];
                pmark->chd.insert(new_node);
                str1 = "";
            }
            create_label = true;
        }
        else if(c == ';'){
            //end of a newick string
            vector<string> str = get_label_weight(str1);
            pmark->label = str[0];
            str1 = "";
            pmark = pmark->parent;
        }
        else if(c == '\'' || c == '\"'){
            continue;
        }
        else{
            str1 += c;
        }
    }
    ptr= *(ptr->chd.begin());
    ptr->parent = NULL;
    delete pmark;
    return *ptr;
}

tree_node* Parser::parse_label_weight(string newick_string){
    int length = (int)newick_string.length();

    tree_node *ptr = new tree_node();
    ptr->label = "dummy root";
    ptr->weight = "";
    ptr->parent = NULL;

    string new_label = "";
    tree_node *pmark = ptr;

    bool create_label = false;
    tree_node *new_node = NULL;

    for(int i = 0; i < length; i++){
        char c = newick_string[i];
        if(c == '(')
        {
            new_label = "";
            new_node = new tree_node();
            new_node->parent = pmark;
            stringstream ss;
            ss << label_counter++;
            new_node->label = "temp_" + ss.str();
            new_node->weight = "";
            new_node->parent->chd.insert(new_node);
            pmark = new_node;
        }
        else if(c == ','){
            if(!create_label)
            {
                new_node = new tree_node();
                vector<string> str = get_label_weight(new_label);
                new_node->parent = pmark;
                new_node->label = str[0];
                new_node->weight = str[1];
                pmark->chd.insert(new_node);
                new_label = "";
            }
            else{
                vector<string> str = get_label_weight(new_label);
                pmark->label = str[0];
                pmark->weight = str[1];
                pmark = pmark->parent;
                new_label = "";
                create_label = false;
            }
        }
        else if(c == ')'){
            if(create_label){
                vector<string> str = get_label_weight(new_label);
                pmark->label = str[0];
                pmark->weight = str[1];
                pmark = pmark->parent;
                new_label = "";
            }
            else{
                new_node = new tree_node();
                vector<string> str = get_label_weight(new_label);
                new_node->parent = pmark;
                new_node->label = str[0];
                new_node->weight = str[1];
                pmark->chd.insert(new_node);
                new_label = "";
            }
            create_label = true;
        }
        else if(c == ';'){
            vector<string> str = get_label_weight(new_label);
            pmark->label = str[0];
            pmark->weight = str[1];
            new_label = "";
            if(pmark->parent != NULL){
                pmark = pmark->parent;
            }
        }
        else{
            new_label += c;
        }
    }
    if(!(ptr->chd.empty())){
        ptr = *(ptr->chd.begin());
        ptr->parent = NULL;
    }

    if(pmark != nullptr){
//        cout << pmark->label << endl;
        if(pmark->label == "dummy root"){
            delete pmark;
        }
    }

    return ptr;
}

//a str could be formatted as "species:weight"
//this method will contract label of a species and its weight
vector<string> Parser::get_label_weight(string str) {
    string label = "";
    string weight = "";

    //if the parser finds out that a string has both label and weight
    //then process it
    unsigned long index = str.find(':');
    if(index != string::npos)
    {
        if(index > 0){
            label = str.substr(0,index);
            weight = str.substr(index+1,str.length()-1);
        }
        else{
            //if the node will not be assigned with the label
            //then create one
            stringstream ss;
            ss << label_counter++;
            label = "temp_" + ss.str();
            weight = str.substr(index+1,str.length()-1);
        }

    }
    else {
        //if the node will not be assigned with the weight
        //then just initialize the label
        if(str.length() > 0){
            label = str;
            if(label[0] == ' '){
                label = label.substr(1,label.length()-1);
            }
        }
        else{
            stringstream ss;
            ss << label_counter++;
            label = "temp_" + ss.str();
        }
    }
    vector<string> vec;
    vec.push_back(string_trim(label));
    vec.push_back(weight);

    return vec;
}

//delete all the extra spaces in a given string
string Parser::string_trim(string& str) {
    if (str.empty()){
        return str;
    }
    str.erase(0,str.find_first_not_of(" "));
    str.erase(str.find_last_not_of(" ") + 1);
    return str;
}