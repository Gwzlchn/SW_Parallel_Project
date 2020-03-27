program: master.o slave.o
	sw5cc -hybrid master.o slave.o  -o program
master.o:master.c
	sw5cc -host -c master.c
slave.o: slave.c
	sw5cc -slave -DTEST -DCHECK -c slave.c

clean:
	rm master.o slave.o program

run:
	sh ./run.sh
