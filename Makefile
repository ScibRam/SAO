#  Main Makefile for SAO

# Compilers and flags
CC=gcc
CCFLAGS := -Wall -MD -pipe


# Source paths
VPATH := src/core/ src/tools


# Libraries
core := $(patsubst src/core/%, %, $(wildcard src/core/*.c))
core := $(core:.c=.o)
corelib : $(core)


# Objects
%.o : %.c
	mkdir -p bin
	mkdir -p lib/obj
	$(CC) -o $(addprefix lib/obj/, $@) -c $< $(CCFLAGS) -Ilib


# Tools

# Coordinated Universal Time (UTC) Convertion and Calculation Tool
utc : utc.o $(core)
	$(CC) -o $(addprefix bin/, $@) $(addprefix lib/obj/, $^) -Ilib




# Actions
.PHONY: clean delete

clean:
	rm -fr lib/obj

delete:
	rm -fr bin
	rm -fr lib/obj

include $(wildcard lib/obj/*.d)
