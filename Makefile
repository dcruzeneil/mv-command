CFLAGS=-Wall -pedantic

rename : rename.c
	gcc $(CFLAGS) -o rename rename.c 

.PHONY: clean
clean: 
	rm -f rename