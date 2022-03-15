CC=gcc
CFLAGS=-O2

DNS.out: main.o lprintf.o log.o hlist.o net.o config.o
	gcc main.c config.c hlist.c net.c lprintf.c log.c -o DNS.out -lm

test : test.o lprintf.o log.o hlist.o net.o
	gcc test.o net.o lprintf.o log.o hlist.o -o test.out -lm



.PHONY:clean
clean:
		@echo "=======clean project========="
		rm   *.o
		rm	 *.out
		rm   *.log
		@echo "=======clean completed========="t