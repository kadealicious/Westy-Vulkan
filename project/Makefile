CCFLAGS = -Wall -std=c99 -O2
CC = gcc
CCLIBS = -lmingw32 -lglfw3dll c:\Windows\System32\vulkan-1.dll --enable-stdcall-fixup -lfreetype6

SRCDIR = code/src
HEADDIR = code/src/h
OBJDIR = code/obj

_OBJS = main.o window.o input.o
OBJS = $(patsubst %, $(OBJDIR)/%,$(_OBJS))
_SRCS = main.c window.c input.c
SRCS = $(patsubst %, $(SRCDIR)/%,$(_SRCS))
_HEADS = window.h input.h
HEADS = $(patsubst %, $(HEADDIR)/%,$(_HEADS))

all: app

code/obj/main.o: code/src/main.c
	$(CC) $(CCFLAGS) -c $^ -o $@
code/obj/window.o: code/src/window.c
	$(CC) $(CCFLAGS) -c $^ -o $@
code/obj/input.o: code/src/input.c
	$(CC) $(CCFLAGS) -c $^ -o $@

app: $(OBJS)
	$(CC) $(CCFLAGS) -o $@ $^ $(CCLIBS)

.PHONY: clean
clean: 
	-del edit $(OBJS)