# -*- coding: utf-8 -*-

import sys
from collections import OrderedDict

import numpy
from matplotlib import pyplot

algos = OrderedDict()

with open(sys.argv[1]) as fd:
    for line in fd:
        pair_type, dist, algo, seconds = line.split()
        algos.setdefault(algo, {})[pair_type] = int(seconds)

print(algos)

std_times = [algo['std'] for algo in algos.values()]
cruft_times = [algo['cruft'] for algo in algos.values()]

ind = numpy.arange(len(algos)) + 0.25
width = 0.3

pyplot.xkcd()

fig, ax = pyplot.subplots()
bars_std = ax.bar(ind, std_times, width, color='#1f77b4', edgecolor='#1f77b4')
bars_cruft = ax.bar(ind+width, cruft_times, width, color='#ff7f0e', edgecolor='#ff7f0e')

# add some text for labels, title and axes ticks
ax.set_ylabel('Cycles per element (fewer is better)')
ax.set_title(r'Sorting $10^6$ pairs of unsigned short')
ax.set_xticks(ind + width)
ax.set_xticklabels(list(algos.keys()))

ax.legend((bars_std[0], bars_cruft[0]), ('std::pair', 'cruft::tight_pair'))

pyplot.show()
