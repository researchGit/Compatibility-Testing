# Fast Compatibility Testing(FastCT)
This project contains codes for solving Compatibility Testing problem.

## Introduction

  Research project **FastCT** implements the fast compatibility testing algorithm for a set of phylogenetic trees proposed by David Fernandez-Baca and Yun Deng[1,2].  

  This document will lead you to a better understanding of using the program on your local machine.

## Components

  * **CT** contains all source codes associated with this project. Under this directory, **cmake-build-debug** directory contains **Makefile** and a Unix executable file named as **CT**.  
  * **data** contains all three sets of real world data used in the project.  
  * **command** contains all commands used to run real data with this program. The command follows a set of rules which can be referred to *Execution* section. Other commands will be introduced later in the next few sections.

## Environment

  FastCT is implemented in C++ with respect to c++11 standard. For convenience, it is recommended to use a Unix-based device, such as a Mac. You can find a Unix executable file named **CT** under **cmake-build-debug** directory.  

  Intuitively, a simple command **./CT** can be used to execute the program. If this file is lost, you simply need to run **make** command in the command tool to re-compile the program and generate a unix executable file. To do this, the necessary **Makefile** can also be found under the this direectory. And this is the only prerequisite your machine needs to satisfy.  

  If there is any problem on *compiler* and *make*, please refer to the corresponding websites for help. Typically, we use *Clang* as our C++ compiler.

## Main Features

  FastCT is mainly used for testing compatibility upon a collection of ancestral phylogenetic trees. If the trees are compatible, it will construct a supertree(display graph) to display all information in the phylogenetic trees. Otherwise, INCOMPATIBLE is reported. The supertrees that are constructed for a set of phylogenetic trees are finally converted into NEWICK format and output into **result.txt** file. This file can be found under **cmake-build-debug** directory as well.  

  As for inputs, FastCT provides two main methods for inputting a set of trees.
  * From a file. The source file is a single text file containing a list of phylogenetic trees which are in NEWICK format. FastCT is able to parse these strings and convert them to the corresponding data structures for its own use. However, the parser implemented inside FastCT program only serves for the purpose of conducting our designed experimental researches. Therefore, there is no guarantee that any NEWICK string can be translated correctly by using our parser.
  * From a simulator. Together with FastCT, a phylogenetic tree generator is implemented to simulate a collection of compatible phylogenetic trees. The simulator is able to generate the phylogenetic trees for triples, leaf-labeled binary trees, internally labeled binary trees, and leaf-labeled non-binary trees.

## Execution

  FastCT uses command line as its interface. The Unix executable file **CT** can be found under the **cmake-build-debug** directory in the project. Download the project, use command console to direct to this directory on your device. The commands are as described below.

### Template

  Here are the command templates:
  * Run the code with real data, use command: ./CT -i --p (1/0) path (path is the file path of the data file, use either 0 or 1 to disable or enable edge promotions)
  * Run the code by generating a complete set of triples covering n labels: ./CT -s -t --n (integer) --p (1/0) --f (integer)
  * **-tp** is added for thorough tests on triples. The purpose of designing this feature is to test how fast compatibility algorithm performs on an incomplete set of triples with different alpha-balance that are generated from their complete sets. The command is: ./CT -s -tp --n (integer) --a (number between 6/n(n-1) and 100) --r (number between 0 and 50) [--p (1/0)] [--f (integer)]. --n tells the number of labels. --a tells how many percentage of the triples will be generated. --r specifies alpha-balance of the tree, for example, 50 means the initial tree is balanced.
  * Run the code by generating a set of k binary trees that cover n labels: ./CT -s -b --n (integer) --k (integer) --l 0 --p (1/0) --f (integer)
  * Run the code by generating a set of k non-binary trees that cover n labels: ./CT -s -m --n (integer) --k (integer) --d (integer) --p (1/0) --f (integer)
  * Run the code with transformed IDPP instances: ./CT -v path --p (1/0) --f (integer) (path is the file path of instance files, suppose they are named as instance1, instance2, ... , instance10, then the name of the file in the path is specified as instance, and the integer follows --f tag is 10 respectively)
### Example:
  * ./CT -i --p 0 /Users/name/tree
  * ./CT -i /Users/name/tree
  * ./CT -s -t --n 100 --p 1 --f 30
  * ./CT -s -b --n 100 --k 100 --l 0 --p 0 --f 30
  * ./CT -s -m --n 100 --k 100 --d 3 --p 0 --f 20
  * ./CT -v /Users/name/instance --p 0 --f 30
  * ./CT -s -tp --n 100 --a 50 --r 10 --p 0 --f 30
### Real data
  Tests were done on three sets of real data, seabirds[3], placental mammals[4] and legume[5]. They are all included under the directory **data**. With all my respects, I would like to appreciate all the efforts the researchers made. So we can take a chance to stand on the shoulders of the giants and present our own work.

  For your convenience, all commands used in the command line to run those three real datasets are also provided under **command** directory as well. You can direct copy a command to your command line and execute it for convenience.

### Note
  * (integer) means the value put here should be a positive integer.  
  * (1/0) means the value here should be either 1 or 0.
  * In our research study, we focus on reporting and studying the behaviors of fast compatibility algorithm. Therefore, the initialization part of the program was not specifically optimized. When you are trying to generate some large graphs, initialization part might take longer time because it would take a longer time to generate the graphs if the graphs you intend to produce are huge.

## Author
  This project is implemented by Lei Liu(lliu@iastate.edu)

## License
  This project is under [MIT License](LICENSE.md).

## Conclusion
  This software could be a useful tool for testing compatibility of a set of phylogenetic trees. Your comments, opinions and criticism are very welcome, and they may become new inspirations directing to huge improvements.  

  I would be very glad to see your citations to this work in your own work if you are going to use some ideas from this project.  

  If you have trouble executing the program, please feel free to contact me through email.

## References
  [1] Y. Deng and D. Fernandez-Baca. An efficient algorithm for testing the compatibility of phylogenies with nested taxa. Algorithms for Molecular Biology, 12:7, 2017.  
  [2] Y. Deng and D. Fernandez-Baca. Fast compatibility testing for rooted phylogenetic trees. Algorithmica, in press, 2017.  
  [3] M. Kennedy and R. D. M. Page. Seabird supertrees: combining partial estimates of procellariiform phylogeny. The Auk, 119(1):88–108, 2002  
  [4] R. M. D. Beck, O. R. P. Bininda-Emonds, M. Cardillo, F. G. R. Liu, and A. Purvis. A
higher-level MRP supertree of placental mammals. BMC Evol. Biol., 6:93, 2006  
  [5] M. Wojciechowski, M. Sanderson, K. Steele, and A. Liston. Molecular phylogeny of the
“Temperate Herbaceous Tribes” of Papilionoid legumes: a supertree approach. In P. Herendeen and A. Bruneau, editors, Advances in Legume Systematics, volume 9, pages 277–298. Royal Botanic Gardens, Kew, 2000.
