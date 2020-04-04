#!/bin/bash
#SBATCH -A TG-SEE150001 #Account number
#SBATCH -n 32            #Number of tasks (cores)
#SBATCH -J sorting           #Job name
#SBATCH -o sorting.o%j       #Output file name
#SBATCH -p normal       #Queue to use
#SBATCH -t 00:15:00     #Run (wall) time 15min
ibrun SampleSort.exe 128000000

