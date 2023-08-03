clean:
	rm -rf *.out *.o
	clear 

test_D:
	gcc -Wall test_utils.c utils.c -o test.o -DDEBUG
	./test.o

test:
	gcc -Wall test_utils.c utils.c -o test.o 
	./test.o
