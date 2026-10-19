all: prog1 prog1b

prog1: prog1.c	
	gcc -Wall -fsanitize=address,undefined -o prog1 prog1.c
prog1b: prog1b.c
	gcc -Wall -fsanitize=address,undefined -o prog1b prog1b.c


.PHONY: clean all

clean:
	rm prog1