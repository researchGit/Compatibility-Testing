#!/usr/bin/env python

files = ['A96fig_withnames.tre',
         'B98fig_withnames.tre',
         'H98fig_withnames.tre',
         'I85fig_withnames.tre',
         'N98fig_withnames.tre',
         'P95fig_withnames.tre',
         'S90fig_withnames.tre']

trees = []

prefix = '../original data files/'

for file in files:
    f = open (prefix + file, 'r')
    for line in f.xreadlines():
        if line.startswith('tree '):
            (_, _, line) = line.partition('[&R] ')
            trees.append(line)
        else:
            continue
    f.close()

g = open ('kennedy.source_trees', 'w')

for tree in trees:
    g.write(tree)

g.close()
