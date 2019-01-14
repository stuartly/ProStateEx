CC=clang -flto

CFLAGS = -m32 -g -ansi -Wall -O0
LDFLAGS = -s

GOLDLDFLAGS = -use-gold-plugin -Wl,-plugin-opt=emit-llvm

all:	miditest   mozart   mfc120   mididump  m2rtttl

miditest:   miditest.c   midifile.o	
	$(CC) $(CFLAGS) $(LFLAGS) midifile.o miditest.c -o miditest
	$(CC) $(CFLAGS) $(LFLAGS) ${GOLDLDFLAGS} midifile.o miditest.c -o miditest.bc 

mozart: mozmain.c   mozart.c   midifile.o	
	$(CC) $(CFLAGS) $(LFLAGS) midifile.o mozart.c mozmain.c -o mozart
	$(CC) $(CFLAGS) $(LFLAGS) ${GOLDLDFLAGS} midifile.o mozart.c mozmain.c -o mozart.bc 

mfc120: mfcmain.c   mfc120.c   midifile.o
	$(CC) $(CFLAGS) $(LFLAGS) midifile.o mfc120.c mfcmain.c -o mfc120
	$(CC) $(CFLAGS) $(LFLAGS) ${GOLDLDFLAGS} midifile.o mfc120.c mfcmain.c -o mfc120.bc

mididump: mididump.c midiutil.o midifile.o
	$(CC) $(CFLAGS) $(LFLAGS) midifile.o midiutil.o mididump.c -o mididump
	$(CC) $(CFLAGS) $(LFLAGS) ${GOLDLDFLAGS} midifile.o midiutil.o mididump.c -o mididump.bc


m2rtttl: m2rtttl.c midifile.o midiutil.o
	$(CC) $(CFLAGS) $(LFLAGS) midifile.o midiutil.o m2rtttl.c -o m2rtttl
	$(CC) $(CFLAGS) $(LFLAGS) ${GOLDLDFLAGS} midifile.o midiutil.o m2rtttl.c -o m2rtttl.bc

midifile.o:	midifile.c	midifile.h
midiutil.o:	midiutil.c	midiutil.h


install:
	@echo Just copy the files somewhere useful!

clean:
	rm -f *.o *.bc *.resolution.txt *.ll *.opt *.id
	rm -f miditest mozart mfc120 mididump m2rtttl

