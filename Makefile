CC=cc
CFLAGS=-g
LFLAGS=-pthread

all: dining selectionSort.o selectionSortTest

dining : dining.c dining.h
	cc dining -o dining -lpthread

selectionSort.o : selectionSort.c
	$(CC) $(CFLAGS) $^ -c -o $@

selectionSortTest: selectionSort.o
	$(CC) $(CFLAGS) selectionSortTest.c -o $@ $^ $(LFLAGS)

clean:
	rm -rf *.o selectionSort.o selectionSortTest dining
