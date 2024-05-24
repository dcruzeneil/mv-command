CFLAGS=-Wall -pedantic

mv : mv.c
	gcc $(CFLAGS) -o mv mv.c 

.PHONY: clean
clean: 
	rm -f mv