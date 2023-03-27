CCFLAGS = -Wall -MMD -m64 -std=c99
CC = gcc
CCLIBS = -lmingw32 glfw3.dll C:\Windows\System32\vulkan-1.dll

SRCDIR = code/src
HEADDIR = code/src/h
OBJDIR = code/obj

_OBJS = main.o window.o input.o vulkan_interface.o shader.o mesh.o camera.o
OBJS = $(patsubst %, $(OBJDIR)/%,$(_OBJS))
_SRCS = main.c window.c input.c vulkan_interface.c shader.c mesh.c camera.c
SRCS = $(patsubst %, $(SRCDIR)/%,$(_SRCS))
_HEADS = stb_image.h window.h input.h vulkan_interface.h shader.h mesh.h camera.h
HEADS = $(patsubst %, $(HEADDIR)/%,$(_HEADS))

all: app

code/obj/main.o: code/src/main.c
	$(CC) $(CCFLAGS) -c $^ -o $@
code/obj/window.o: code/src/window.c
	$(CC) $(CCFLAGS) -c $^ -o $@
code/obj/input.o: code/src/input.c
	$(CC) $(CCFLAGS) -c $^ -o $@
code/obj/vulkan_interface.o: code/src/vulkan_interface.c
	$(CC) $(CCFLAGS) -c $^ -o $@
code/obj/shader.o: code/src/shader.c
	$(CC) $(CCFLAGS) -c $^ -o $@
code/obj/mesh.o: code/src/mesh.c
	$(CC) $(CCFLAGS) -c $^ -o $@
code/obj/camera.o: code/src/camera.c
	$(CC) $(CCFLAGS) -c $^ -o $@

shaders: 
	compile_shaders.bat

app: $(OBJS)
	$(CC) $(CCFLAGS) -o $@ $^ $(CCLIBS)

.PHONY: app shaders clean
clean: 
	-del edit $(OBJS)