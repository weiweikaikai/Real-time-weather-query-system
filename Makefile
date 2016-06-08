.PHONY:all clean
PWD=$(shell pwd)
CC=gcc
INCLUDE=.
FLAGS= -g #-Wall -DDEBUG
LDFLAGS= -lpthread#-staticc
BIN=myhttpd  
LIB=
all:$(BIN)
%.o:%.c
	$(CC) $(FLAGS) -c $< -o $@ $(LDFLAGS) 
myhttpd:httpd.c commsocket.h commsocket.c epoll.c threadpool.c
	$(CC) $(FLAGS)  $^ -o $@ $(LDFLAGS) 
clean:
	rm -rf  *.o $(BIN)
	


