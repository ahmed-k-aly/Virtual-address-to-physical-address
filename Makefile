CC            = gcc
SPECIAL_FLAGS = -ggdb -Wall
CFLAGS        = -std=gnu99 -fPIC

all: libvmsim iterative-walk random-hop

libvmsim: vmsim.o mmu.o
	$(CC) $(CFLAGS) -shared -o libvmsim.so vmsim.o mmu.o

mmu.o: mmu.h vmsim.h mmu.c
	$(CC) $(CFLAGS) $(DEBUG_FLAGS) -c mmu.c

iterative-walk: iterative-walk.c vmsim.h
	$(CC) $(CFLAGS) $(DEBUG_FLAGS) -L. -o iterative-walk iterative-walk.c -lvmsim

random-hop: random-hop.c vmsim.h
	$(CC) $(CFLAGS) $(DEBUG_FLAGS) -L. -o random-hop random-hop.c -lvmsim

clean:
	rm -rf mmu.o libvmsim.so iterative-walk random-hop
