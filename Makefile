OBJS = glvampire.o glvampirelib.o immediate2.o

EXE = immediate2.exe

OTHER = 

CCM68K = gcc -m68060 -fomit-frame-pointer -I/ade/include -noixemul -g -gstabs

CC603 = ppc-amigaos-gcc -V 2.95.1 -mcpu=603e -mmultiple -warpup
CC604 = ppc-amigaos-gcc -V 2.95.1 -mcpu=604e -mmultiple -warpup
CC750 = m68k-amigaos-gcc -fomit-frame-pointer -noixemul -mcpu=68030 -m68881

CC = $(CC750)

LD = m68k-amigaos-ld

CODEGEN = 

OPTIMIZE = -O2 -ffast-math

CFLAGS= $(CODEGEN) $(OPTIMIZE)
CFLAGS_NOOPT = $(CODEGEN)

LIBS = -lm -lamiga -lstdc++

%.o : %.c
	$(CC) $(CFLAGS) -S $< -o $*.s
	$(CC) $(CFLAGS) -c $*.s -o $*.o
	
%.o : %.cpp
	$(CC) $(CFLAGS) -S $< -o $*.s
	$(CC) $(CFLAGS) -c $*.s -o $*.o	

ASMS = $(OBJS:.o=.s)

$(EXE) : $(OBJS) 
	$(CC) $(OBJS) $(OBJSCPP) $(LIBS) -o immediate2.exe

depend : 
	$(CC) $(CFLAGS) -MM $(OBJS:.o=.c) >depend

clean :
	rm $(EXE) $(OBJS) $(ASMS) $(OTHER) depend

