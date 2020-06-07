
bsub -b -I -perf -q q_sw_share -n 1 -cgsp 64 -host_stack 256 -share_size 4096 ./test_program