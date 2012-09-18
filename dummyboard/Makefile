all: dummyboard

dummyboard: dummyboard.o
	gcc -o $@ $^ -lusb

dummyboard.o: dummyboard.c
	gcc -c $^

clean:
	rm -rf dummyboard
	rm -rf dummyboard.o

.PHONY: all clean
