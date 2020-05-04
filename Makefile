CC=$(CXX)
CPPFLAGS = -Wall -O3 -funroll-loops

all: meta-ttt

clean:
	rm -rf *.o meta-ttt

meta-ttt: meta-ttt.o
