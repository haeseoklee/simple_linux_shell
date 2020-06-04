all: myshell.out sub.out

myshell.out: main.o
	gcc -o myshell.out main.o

sub.out: sub.o
	gcc -o sub.out sub.o

main.o: main.c
	gcc -c -o main.o main.c

sub.o: sub.c
	gcc -c -o sub.o sub.c

clean:
	rm -f *.o
	rm -f *.out