import pandas as pd
import copy
import os
import re
from operator import itemgetter
from itertools import groupby
import numpy as np
import matplotlib.pyplot as plt
import random

path = r'.'
file_names = os.listdir(path)
data_dict_list = []

out_put_file_name = 'prefix.xlsx'


blank_dict = {'core':0,'size':0,\
            'scatter cycle':0,\
            'First step cycle':0,\
            'gather last elem then scatter cycle':0,\
            'gather prev last value then add a constant to chunk cycle':0,
            'total cost cycle':0,\
            'sum formula cycle':0}
total_cost_cycle_str = 'total cost cycle'
output_name = 'prefix'
calculate_phase_str = 'scatter cycle'
x_label_str = 'array size'

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
        
        cur_file = open(name)
        cur_file_all_lines = cur_file.readlines()

        for line in cur_file_all_lines:
            idx = line.find('cycle')
            if(idx==-1):
                continue
            line_list = re.findall(r'[0-9]+|[a-zA-Z]+',line)
            cur_cycle = int(line_list.pop())
            cur_str = ' '.join(line_list)

            if cur_str in cur_dict:
                cur_dict[cur_str] = cur_cycle


        data_dict_list.append(cur_dict)
        
# 计算一次 formula cycle

for one_dict in data_dict_list:
    sum = 0
    total_exper = 0
    for key,value in one_dict.items():
        if(key=='core' or key=='size' ):
            continue
        if(key==total_cost_cycle_str):
            total_exper = value
            continue
        if(key==calculate_phase_str):
            value *= 0.1
        sum += int(value)
    one_dict['overhead expriment'] = one_dict.pop(total_cost_cycle_str)
    one_dict['overhead formula'] = sum
   
    div = (abs(total_exper - sum))/(total_exper)
    if total_exper != 0:
        one_dict['deviation'] =div
    else:
        one_dict['deviation'] = 0

    
# 对list dict 排序

data_dict_list = sorted(data_dict_list,key=itemgetter('core','size','overhead formula'))


# 对 list dict 拆分
it = groupby(data_dict_list,key = lambda x:(x['core']))
groups = []
keys = []

def get_values_by_one_key_in_list_of_dict(to_get:list,key:str) ->list:
    return_values = []
    for one_dict in to_get:
        #print(one_dict)
        cut_val = one_dict.get(key)
        return_values.append(cut_val)
    return return_values



#保存size,y1,y2
points=[]
# 误差
deviation_list = []
for k,g in it:
    cur_list_of_dict = list(g)
    groups.append(list(cur_list_of_dict))
    x_data_cur_size = get_values_by_one_key_in_list_of_dict(cur_list_of_dict,'size')
    y_data1_overhead_expriment  = get_values_by_one_key_in_list_of_dict(cur_list_of_dict,'overhead expriment')
    y_data2_overhead_formula = get_values_by_one_key_in_list_of_dict(cur_list_of_dict,'overhead formula')
    cur_deviation_list = get_values_by_one_key_in_list_of_dict(cur_list_of_dict,'deviation')

    average_dev = np.mean(cur_deviation_list)
    deviation_list.append(average_dev)

 
    cur_points = []
    cur_points.append(x_data_cur_size)
    cur_points.append(y_data1_overhead_expriment)
    cur_points.append(y_data2_overhead_formula)

    points.append(cur_points)
    #print(average_dev)
    keys.append(k)

#print(points)





#开始画子图

# plot something
fig, axs = plt.subplots(nrows=3,ncols=2, sharex=False, sharey=False)
fig.set_size_inches(10, 15, forward=True)
for i, ax in enumerate(axs.flat):
    cur_points = points[i]
    ax.plot(cur_points[0],cur_points[1],c='b')
    ax.plot(cur_points[0],cur_points[2],c='g')

    cur_dev = deviation_list[i]
    cur_dev = '%.3f%%' % (cur_dev*100)
    first_title = 'the whole number of nodes is ' + str(keys[i]) + '\n' + 'deviation = ' + str(cur_dev)
    ax.set_title(first_title)
    ax.legend(['overhead expriment','overhead formula'],loc= 'upper left')


   
print(np.mean(deviation_list))
# set labels
plt.setp(axs[-1, :], xlabel=x_label_str)
plt.setp(axs[:, 0], ylabel='Cycles')
output_name_png = output_name +'.png'
if(os.path.exists(output_name_png)):
    os.remove(output_name_png)
plt.savefig(output_name_png,format='png')
#plt.show()


#输出excel
output_xlsx_name = output_name+'.xlsx'
if(os.path.exists(output_xlsx_name)):
    os.remove(output_xlsx_name)
output_df = pd.DataFrame.from_dict(data_dict_list)
writer = pd.ExcelWriter(output_xlsx_name,engine='xlsxwriter')
output_df.to_excel(writer,index=False)
writer.save()

