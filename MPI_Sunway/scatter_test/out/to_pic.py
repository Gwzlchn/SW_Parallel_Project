import pandas as pd
import copy
import os
import re
from operator import itemgetter
from itertools import groupby
import numpy as np
import matplotlib.pyplot as plt
from scipy import stats

path = r'.'
file_names = os.listdir(path)
data_dict_list = []



blank_dict = {'core':0,'size':0,\
                    'cycles':0,\
                    'data size in str':0,
                    'real data size':0}
                   

output_name = 'matrix'



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
            idx = line.find('Bytes')
            if(idx==-1):
                continue
            line_list = re.findall(r'[0-9]+|[a-zA-Z]+',line)

            cur_dict['cycles'] = int(line_list[0])
            cur_dict['data size in str'] = line_list[-2] + line_list[-1]

            if line_list[-1]=='KBytes':
                cur_dict['real data size'] = int(line_list[-2])*1024
            if line_list[-1]=='MBytes':
                cur_dict['real data size'] = int(line_list[-2])*1024*1024
            if line_list[-1]=='GBytes':
                cur_dict['real data size'] = int(line_list[-2])*1024*1024*1024



        data_dict_list.append(cur_dict)
        

print(data_dict_list)
# 对list dict 排序

data_dict_list = sorted(data_dict_list,key=itemgetter('core','size','cycles'))


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
#斜率
slopes = []
for k,g in it:
    cur_list_of_dict = list(g)
    groups.append(list(cur_list_of_dict))
    x_data_cur_size = get_values_by_one_key_in_list_of_dict(cur_list_of_dict,'data size in str')
    y_data1_overhead_expriment  = get_values_by_one_key_in_list_of_dict(cur_list_of_dict,'cycles')

    real_data_size=   get_values_by_one_key_in_list_of_dict(cur_list_of_dict,'real data size')

    #print(real_data_size)

    slope, intercept, r_value, p_value, slope_std_error = stats.linregress(real_data_size, y_data1_overhead_expriment)

    slopes.append(slope)
   # y_data2_overhead_formula = get_values_by_one_key_in_list_of_dict(cur_list_of_dict,'overhead formula')
   # cur_deviation_list = get_values_by_one_key_in_list_of_dict(cur_list_of_dict,'deviation')

   
    cur_points = []
    cur_points.append(x_data_cur_size)
    cur_points.append(y_data1_overhead_expriment)
    #cur_points.append(y_data2_overhead_formula)

    points.append(cur_points)
    #print(average_dev)
    keys.append(k)

print(slopes)





#开始画子图

# plot something
fig, axs = plt.subplots(nrows=3,ncols=2, sharex=False, sharey=False)
fig.set_size_inches(10, 15, forward=True)
for i, ax in enumerate(axs.flat):
    cur_points = points[i]
    ax.plot(cur_points[0],cur_points[1],c='g')
    #ax.plot(cur_points[0],cur_points[2],c='g')
    for tick in ax.get_xticklabels():  # 将横坐标倾斜30度，纵坐标可用相同方法
        tick.set_rotation(30)

    cur_slope = format(slopes[i], '.3f')
    print(cur_slope)
    first_title = 'the whole number of nodes is ' + str(keys[i]) + '\n The bandwith is '+str(cur_slope)+' bytes/sec'
    ax.set_title(first_title)
   # ax.legend(['Cycles'],loc= 'upper left')


   

# set labels
plt.setp(axs[-1, :], xlabel='Total amount of scatter data/')
plt.setp(axs[:, 0], ylabel='Cycles')
plt.subplots_adjust(wspace=0.4, hspace=0.4) 
#plt.style.use(None) 
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





