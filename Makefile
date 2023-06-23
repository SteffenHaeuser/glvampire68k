CC = m68k-amigaos-gcc
AR = m68k-amigaos-ar
CFLAGS = -Wall -Wextra -fomit-frame-pointer -noixemul -mcpu=68030 -m68881 -O2 -ffast-math
LIBS = -lm -lstdc++ -lamiga

LIBS_GL4VAMPIRE = -lgl4vampire
LIBS_GL = -lGL

OBJS_GL4VAMPIRE = glvampirelib.o
OBJS_GL = glErrors.o glSetup.o glColors.o glMatrix.o glDraw.o glVertex.o glZBuffer.o glTextures.o
OBJS_EXE = immediate2.o

EXE = immediate2

OTHER = 

%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o : %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

all: libgl4vampire.a libGL.a $(EXE)

libgl4vampire.a: $(OBJS_GL4VAMPIRE)
	$(AR) rcs $@ $(OBJS_GL4VAMPIRE)

libGL.a: $(OBJS_GL)
	$(AR) rcs $@ $(OBJS_GL)

$(EXE) : $(OBJS_EXE) libgl4vampire.a libGL.a
	$(CC) $(OBJS_EXE) -L. -lgl4vampire -L. -lGL $(LIBS) -o $@

depend : 
	$(CC) $(CFLAGS) -MM $(OBJS_GL:.o=.c) >depend

clean :
	rm -f $(EXE) libgl4vampire.a libGL.a $(OBJS_GL4VAMPIRE) $(OBJS_GL) $(OBJS_EXE) $(ASMS) $(OTHER) depend
