CCFLAGS = -Wall -MMD -m64 -std=c99
CC = gcc
CCLIBS = -lmingw32 glfw3.dll vulkan-1.dll

SRCDIR = code/src
HEADDIR = code/src/h
OBJDIR = code/obj
CLEANDIR = code\obj

_OBJS = main.o window.o input.o graphics.o mesh.o texture.o shader.o camera.o game.o mafs.o
OBJS = $(patsubst %, $(OBJDIR)/%,$(_OBJS))
CLEANOBJS = $(patsubst %, $(CLEANDIR)\\%,$(_OBJS))
_SRCS = main.c window.c input.c graphics.c mesh.c texture.c shader.c camera.c game.c mafs.c
SRCS = $(patsubst %, $(SRCDIR)/%,$(_SRCS))
_HEADS = stb_image.h window.h input.h graphics.h mesh.h texture.h shader.h camera.h game.h mafs.h
HEADS = $(patsubst %, $(HEADDIR)/%,$(_HEADS))

all: app

code/obj/main.o: code/src/main.c
	$(CC) $(CCFLAGS) -c $^ -o $@
code/obj/window.o: code/src/window.c
	$(CC) $(CCFLAGS) -c $^ -o $@
code/obj/input.o: code/src/input.c
	$(CC) $(CCFLAGS) -c $^ -o $@
code/obj/graphics.o: code/src/graphics.c
	$(CC) $(CCFLAGS) -c $^ -o $@
code/obj/mesh.o: code/src/mesh.c
	$(CC) $(CCFLAGS) -c $^ -o $@
code/obj/texture.o: code/src/texture.c
	$(CC) $(CCFLAGS) -c $^ -o $@
code/obj/shader.o: code/src/shader.c
	$(CC) $(CCFLAGS) -c $^ -o $@
code/obj/camera.o: code/src/camera.c
	$(CC) $(CCFLAGS) -c $^ -o $@
code/obj/game.o: code/src/game.c
	$(CC) $(CCFLAGS) -c $^ -o $@
code/obj/mafs.o: code/src/mafs.c
	$(CC) $(CCFLAGS) -c $^ -o $@

shaders: 
	compile_shaders.bat

app: $(OBJS)
	$(CC) $(CCFLAGS) -o $@ $^ $(CCLIBS)

.PHONY: app shaders clean
clean: 
	-del $(CLEANOBJS)