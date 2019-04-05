make: cube.o

cube: cube.o
	gcc -o cube cube.o

cube.o: cube.c
	gcc -g cube.c wizard.c -lreadline -lhistory -lncurses -lpthread -o cube

clean:
	rm -f cube
	rm -f cube.o
