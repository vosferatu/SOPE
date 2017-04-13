#Makefile for Project sfind

CC = gcc

CFLAGS = -Wall

TARGET = sfind

DEPS = sfind.h

all: sfind

sfind: sfind.c 
	$(CC) $(CFLAGS) -o bin/sfind sfind.c

clean: $(RM) bin/* 