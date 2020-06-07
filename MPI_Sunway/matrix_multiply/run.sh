
for cores in {4,16,128,256,512,1024}
do
  for test_time in {2,4,6,8,10}
  do
    # shellcheck disable=SC2219
    let sizes=$cores*$test_time
    cur_out=out/core"$cores"_size"$sizes".txt

    bsub -o $cur_out -J core"$cores"_size"$sizes" -q q_sw_share -N $cores  -host_stack 256 -share_size 4096 ./matrix_multiply $cores $sizes
    #echo core"$cores"_size"$sizes"
  done
done

#test
#bsub -I  -q q_sw_expr -N 4  -host_stack 256 -share_size 4096 ./matrix_multiply 4 16