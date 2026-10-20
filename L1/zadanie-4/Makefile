CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Werror -pedantic -O2

biblioteka: biblioteka.c etap-4.c etap-4.h
	$(CC) $(CFLAGS) biblioteka.c etap-4.c -o biblioteka

.PHONY: clean
clean:
	rm -f biblioteka
