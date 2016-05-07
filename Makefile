.PHONY:all clean
PWD=$(shell pwd)
CC=gcc
INCLUDE=.
FLAGS=-Wall -g #-DDEBUG
LDFLAGS= -lpthread#-staticc
BIN=my_httpd  
LIB=
all:$(BIN)
%.o:%.c
	$(CC) $(FLAGS) -c $< -o $@ $(LDFLAGS) 
my_httpd:httpd.c commsocket.h
	$(CC) $(FLAGS)  $^ -o $@ $(LDFLAGS) 
clean:
	rm -rf  *.o $(BIN)
	


