CC = sw5cc
CXX = sw5CC
MPI = mpicc

SWLU = /home/export/online1/swmore/opensource/swlu
SWLU_INC = $(SWLU)/include/
SWLU_LIB = $(SWLU)/lib/
MPI_INC = /usr/sw-mpp/mpi2/include

TARGET = scatter_test
CFLAGS = -g -Wall -O0 -host -std=gnu99
INCLUDE = -I$(SWLU_INC) -I$(MPI_INC)
LB_LIB = -L$(SWLU_LIB)
SOURCES = scatter_test.c
OBJ = scatter_test.o
SEQ_LDFLAGS = -lswlu
MPI_LDFLAGS = -lswlu_mpi


$(TARGET):$(OBJ)
	$(MPI)   $(OBJ)  $(LB_LIB) $(SEQ_LDFLAGS) -o $(TARGET)
$(OBJ):$(SOURCES)
	$(CC)  $(INCLUDE)  $(CFLAGS) -c  $(SOURCES)


clean:
	rm -f $(TARGET)
	rm -f $(OBJ)

