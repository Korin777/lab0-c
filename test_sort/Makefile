CC = gcc
CFLAGS = -O1 -g -Wall
CFILES=queue.c list_sort.c test_sort.c

all: merge_sort quick_sort ksort

merge_sort: $(CFILES)
	$(CC) -o $@ $(CFILES) $(CFLAGS) 
quick_sort: $(CFILES)
	$(CC) -o $@ $(CFILES) $(CFLAGS) -DQSORT 

ksort: $(CFILES)
	$(CC) -o $@ $(CFILES) $(CFLAGS) -DKSORT

data.txt: randstr.py
	python3 randstr.py

clean:
	rm -f *.o merge_sort quick_sort ksort