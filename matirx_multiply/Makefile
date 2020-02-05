program: master.o slave.o
	sw5cc -hybrid master.o slave.o -lswperf -Wl,-zmuldefs -o program
master.o:master.c
	sw5cc -host -cpp -I/usr/sz/swcc/sw3gcc-binary/include -c master.c
slave.o: slave.c
	sw5cc -slave -DTEST -DCHECK -c slave.c

clean:
	rm master.o slave.o program

run:
	sh ./run.sh
