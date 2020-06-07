import pandas as pd
import copy
import os
import re
from operator import itemgetter
from itertools import groupby
import numpy as np
import matplotlib.pyplot as plt


path = r'.'
file_names = os.listdir(path)
data_dict_list = []



blank_dict = {'core':0,'size':0,\
                    'board and scatter cycle':0,\
                    'multiply cycle':0,\
                    'gather cycle':0,\
                    'totally cost cycle':0}
total_cost_cycle_str = 'totally cost cycle'
output_name = 'matrix'
calculate_phase_str = 'multiply cycle'


#print(total_cost_cycle_str)
for name in file_names:
    name_split = re.split('\_|\.',name)
    if(len(name_split)!=3):
        continue
    cur_core_list = name_split[0].split('core')
    cur_size_list = name_split[1].split('size')
    # 清空 dict
    cur_dict = copy.deepcopy(blank_dict)
    if(len(cur_core_list)==2 and len(cur_size_list)==2):
        cur_core = cur_core_list[1]
        cur_size = cur_size_list[1]
        #print(cur_core,cur_size)
        cur_dict['core'] = int(cur_core)
        cur_dict['size'] = int(cur_size)
        
        cur_file = open(name,'a+')
        cur_file_all_lines = cur_file.readlines()


        root_node_add_times = 0
        root_node_multiply_times = 0
        root_node_mem_times = 0
        

        for line in cur_file_all_lines:
            idx = line.find('cycle')
            if(idx==-1):
                continue
            line_list = re.findall(r'[0-9]+|[a-zA-Z]+',line)
            cur_cycle = int(line_list.pop())
            cur_str = ' '.join(line_list)

            if cur_str in cur_dict:
                cur_dict[cur_str] = cur_cycle

        str1 = "--------------------BEGIN COUNT-----------------------------\n\n"
        str2 = "root node add times \t\t\t\t"+ root_node_add_times +"\n"
        str3 = "root node multiply times \t\t\t\t" + root_node_multiply_times + "\n"
        str4 = "root node mem times \t\t\t" +  root_node_mem_times + '\n'
        str5 = "--------------------END COUNT------------------------------\n\n"
        cur_file.write(str1)
        cur_file.write(str2)
        cur_file.write(str3)
        cur_file.write(str4)
        cur_file.write(str5)

