#!/usr/bin/env python

from newick_modified import parse_tree
from spruce.unrooted import restrict, readMultipleTreesFromFile

trees = [parse_tree(tree) for tree in readMultipleTreesFromFile('kennedy.source_trees')]

taxaToRemove = set(['Pelecanus erythrorhynchos',
                    'Sula bassana',
                    'Phalacrocorax aristoteles',
                    'Larus dominicanus',
                    'Stictocarbo punctatus',
                    'Fregata minor',
                    'Fregata ariel',
                    'Fregata magnificens'])

for i in range(len(trees)):
    trees[i] = restrict(trees[i], set(trees[i].get_leaves_identifiers()) - taxaToRemove)

f = open ('kennedy.source_trees_manual', 'w')
for tree in trees:
    f.write(str(tree))
    f.write(';\n')
f.close()
