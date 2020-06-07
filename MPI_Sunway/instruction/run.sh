for i in {1..10}
do
  bsub -b -I  -q q_sw_share -N 2 -cgsp 64 -host_stack 5000 -share_size 4096 ./ins_test
done
bsub -b -I -pr -q q_sw_expr -n 2  -allmaster -host_stack0 256 -cross_size0 10000 -host_stack 256 -cross_size 2500 ./ins_test