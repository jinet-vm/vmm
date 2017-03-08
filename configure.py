#!/bin/python3
import json as j
import argparse as ag
from collections import OrderedDict

parser = ag.ArgumentParser(description='demos configure tool')
parser.add_argument('--lang',type=str,default='c',help='Language for const to be defined in: c, fasm, ld')

f = open('config.json','r')
configc = f.read()
conf = j.loads(configc, object_pairs_hook=OrderedDict)

args = parser.parse_args()

if args.lang == 'c':
	for name, i in conf.items():
		print('#define '+name+" "+i)
else:
	for name, i in conf.items():
		print(name+' = '+i+';')