.PHONY:all clean
PWD=$(shell pwd)
CC=gcc
INCLUDE=.
FLAGS=-Wall -g #-DDEBUG
LDFLAGS= -lpthread#-staticc
BIN=myhttpd  
LIB=
all:$(BIN)
%.o:%.c
	$(CC) $(FLAGS) -c $< -o $@ $(LDFLAGS) 
myhttpd:httpd.c commsocket.h
	$(CC) $(FLAGS)  $^ -o $@ $(LDFLAGS) 
clean:
	rm -rf  *.o $(BIN)
	


