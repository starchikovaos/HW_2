"""
Input data: XML-file contains electric chain's elements
Output data: CSV-file contains float values of resistances

Comand line arguments :
0. program name
1. input file name
2. output file name
"""
import xml.dom.minidom
from xml.dom.minidom import Node
import matrixops 
import copy
import time
import sys
if len(sys.argv) > 2:
    input_path = sys.argv[1]
    output_path = sys.argv[2]
else:
    print ("Enter name of input and(or) output file name")

dom = xml.dom.minidom.parse(input_path)
nets = dom.getElementsByTagName("net")
diodes = dom.getElementsByTagName("diode")
resistors = dom.getElementsByTagName("resistor")
capactors = dom.getElementsByTagName("capactor")
e = {}
for net in nets:
        net_id = net.getAttribute("id")
for diode in diodes:
    d_n_from = diode.getAttribute("net_from")
    d_n_to = diode.getAttribute("net_to")
    d_res = diode.getAttribute("resistance")
    d_rev_res = diode.getAttribute("reverse_resistance")
    e[d_n_from, d_n_to] = d_res
    e[d_n_to, d_n_from] = d_rev_res
for resistor in resistors:
    res_n_from = resistor.getAttribute("net_from")
    res_n_to = resistor.getAttribute("net_to")
    res_res = resistor.getAttribute("resistance")
    e[res_n_from, res_n_to] = res_res
    e[res_n_to, res_n_from] = res_res
for capactor in capactors:
    cap_n_from = capactor.getAttribute("net_from")
    cap_n_to = capactor.getAttribute("net_to")
    cap_res = capactor.getAttribute("resistance")
    e[cap_n_from, cap_n_to] = cap_res
    e[cap_n_to, cap_n_from] = cap_res
N = nets.length
d = []
for i in range(N):
    d.append([float('Inf')]*N)
for i in range(N):
    d[i][i] = 0
for key in sorted(e):
    i = int(key[0])-1
    j = int(key[1])-1
    value = e[key]
    try:
        val = 1/d[i][j]
    except:
        val = float('Inf')
    d[i][j] = 1/(val+1/float(value))
d_new = copy.deepcopy(d);
start_py = time.time()
for k in range(N):
    for i in range(N):
        for j in range(N):
            try:
                v1 = 1/d[i][j]
            except:
                v1 = float('Inf')
            try:
                v2 = 1/(d[i][k]+d[k][j])
            except:
                v2 = float('Inf')
            try:
                d[i][j] = 1/(v1 + v2)
            except:
                d[i][j] = float('Inf')
with open(output_path, "w") as fd:
    for i in range(N):
        for j in range(N-1):
            fd.write("%.6f" % (d[i][j])+",")
        fd.write("%.6f" % (d[i][j+1])+"\n")
fd.close()
finish_py = time.time()

start_cpp = time.time()
d_cpp = matrixops.faster_resistance(d_new)
finish_cpp = time.time()

if (finish_cpp - start_cpp == 0):
    print ("Time of Python working :", (finish_py - start_py), "Time of C++ working is very close to zero")
else:
    print ("Time of working:", (finish_py - start_py)/(finish_cpp - start_cpp))
