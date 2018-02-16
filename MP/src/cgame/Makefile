# makefile for win32 using mingw on linux (debian unstable)
# based on (DJ Arts) Thanos Kyritsis' makefiles
# http://members.hellug.gr/djart/wolfsource

#CC = gcc
CC = /usr/bin/i586-mingw32msvc-gcc
CFLAGS = -pipe -fsigned-char -DNDEBUG -O6 -march=i586 \
-fomit-frame-pointer -ffast-math -falign-loops=2 -falign-jumps=2 \
-falign-functions=2 -fno-strict-aliasing -fstrength-reduce \
-DWIN32 -D_WINDOWS -DC_ONLY -DCGAMEDLL

SOURCES = $(shell ls wolf*.c cg_*.c ../game/bg_animation.c ../game/bg_misc.c ../game/bg_pmove.c ../game/bg_slidemove.c ../game/q_math.c ../game/q_shared.c ../ui/ui_shared.c)
MODULES = $(SOURCES:.c=.o)

#all: clean gameclean cgame_mp_x86.dll

all: cgame_mp_x86.dll

gameclean:
	rm -rf ../game/*.o ../ui/*.o

cgame_mp_x86.dll: $(MODULES)
	$(CC) -shared -lm -o $@ $?

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o *.dll ../game/*.o ../ui/*.o

