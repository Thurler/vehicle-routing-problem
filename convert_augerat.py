#!/usr/bin/env python

import os

for dirname, subdirlist, filelist in os.walk("./instances/A-VRP"):
    for filename in filelist:
        with open("./instances/A-VRP/"+filename, 'r') as inp:
            lines = inp.readlines()
            n_v = int(lines[3].split(':')[1][1:-2])
            capacity = int(lines[5].split(':')[1][1:-2])
            coords = []
            for i in range(n_v):
                line = lines[7+i][:-2].split(' ')[-2:]
                coords.append((int(line[0]), int(line[1])))
            distances = [[] for i in range(n_v)]
            for i in range(n_v):
                for j in range(n_v):
                    if i == j:
                        continue
                    start = coords[i]
                    end = coords[j]
                    dist = ((start[0]-end[0])**2 + (start[1]-end[1])**2)**0.5
                    distances[i].append(dist)
            demands = []
            for i in range(n_v):
                demand = int(lines[7+n_v+1+i][:-2].split(' ')[1])
                demands.append(demand)
            with open("./instances/"+filename+'t', 'w') as outp:
                outp.write(str(18)+'\n')
                for demand in demands[:18]:
                    outp.write(str(demand)+'\n')
                for dist_v in distances[:18]:
                    for dist in dist_v[:17]:
                        outp.write(str(dist)+'\n')
                for i in range(18):
                    outp.write(str(capacity)+'\n')
