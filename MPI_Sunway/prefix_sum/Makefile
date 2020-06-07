CC = sw5cc
CXX = sw5CC
MPI = mpicc

SWLU = /home/export/online1/swmore/opensource/swlu
SWLU_INC = $(SWLU)/include/
SWLU_LIB = $(SWLU)/lib/
MPI_INC = /usr/sw-mpp/mpi2/include

TARGET = prefix_sum
CFLAGS = -g -Wall -O2 -host -std=gnu99
INCLUDE = -I$(SWLU_INC) -I$(MPI_INC)
LB_LIB = -L$(SWLU_LIB)
SOURCES = prefix_sum.c
OBJ = prefix_sum.o
SEQ_LDFLAGS = -lswlu
MPI_LDFLAGS = -lswlu_mpi


$(TARGET):$(OBJ)
	$(MPI)   $(OBJ)  $(LB_LIB) $(SEQ_LDFLAGS) -o $(TARGET)
$(OBJ):$(SOURCES)
	$(CC)  $(INCLUDE)  $(CFLAGS) -c  $(SOURCES)


clean:
	rm -f $(TARGET)
	rm -f $(OBJ)

