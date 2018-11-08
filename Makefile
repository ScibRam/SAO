#  Main Makefile for SAO

# Compilers and flags
CC=gcc
CCFLAGS := -Wall -MD -pipe


# Source paths
VPATH := src/core/ src/sys src/tools


# Libraries
core := $(patsubst src/core/%, %, $(wildcard src/core/*.c))
core := $(core:.c=.o)
corelib : $(core)
sys := $(patsubst src/sys/%, %, $(wildcard src/sys/*.c))
sys := $(sys:.c=.o)
syslib : $(sys)

# Objects
%.o : %.c
	mkdir -p bin
	mkdir -p lib/obj
	$(CC) -o $(addprefix lib/obj/, $@) -c $< $(CCFLAGS) -Ilib


# Tools

# Coordinated Universal Time (UTC) Convertion and Calculation Tool
utc : utc.o $(core)t
	$(CC) -o $(addprefix bin/, $@) $(addprefix lib/obj/, $^) -Ilib

# SAC file Information Tool
sacinfo : sacinfo.o $(core) $(sys)
	$(CC) -o $(addprefix bin/, $@) $(addprefix lib/obj/, $^) -Ilib


# Tests

# Coordinated Universal Time (UTC) Convertion and Calculation Tool
trace_sacio : trace_sacio.o $(core) $(sys)
	$(CC) -o $(addprefix tst/, $@) $(addprefix lib/obj/, $^) -Ilib



# Actions
.PHONY: clean delete

clean:
	rm -fr lib/obj

delete:
	rm -fr bin
	rm -fr lib/obj

include $(wildcard lib/obj/*.d)
