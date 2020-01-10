divide: master.o slave.o
	sw5cc -hybrid -o  divide -g master.o slave.o
master.o:master.c
	sw5cc -host -c master.c
slave.o: slave.c
	sw5cc -slave -c slave.c

clean:
	rm master.o slave.o divide
