override CFLAGS=-Wall -Wextra -fanalyzer -g -O0 -fsanitize=address,undefined

ifdef CI
override CFLAGS=-Wall -Wextra -Werror
endif

.PHONY: clean all

all: sop-dika

sop-dika: sop-dika.c	
	gcc $(CFLAGS) -o sop-dika sop-dika.c

clean:
	rm sop-dika
