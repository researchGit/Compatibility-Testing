#!/usr/bin/env python

f = open ('../original data file/Beck_et_al._complete_analysis_source_trees.nex', 'r')
g = open ('beck.sources', 'w')

for line in f.xreadlines():
    # ignore blank lines
    if line.strip() == '':
        continue

    # ignore Nexus-specific lines
    if line.startswith('#NEXUS') or line.startswith('begin') or line.startswith('end'):
        continue

    # strip weight, name information 
    (_, _, line) = line.partition('.tre = ')

    # remove errant comma, as suggested by authors (personal communication)
    if line.startswith('(((Leporidae,((Muridae,Real_OG,)'):
        line = line.replace('(((Leporidae,((Muridae,Real_OG,)', '(((Leporidae,((Muridae,Real_OG)')

    #write Newick tree string to output file
    g.write(line)

f.close()
g.close()
