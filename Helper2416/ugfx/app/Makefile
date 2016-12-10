# Compiler, optimizer and uGFX library location
GFXLIB  = ../
CC      = gcc
CCOPT   = -g -ggdb -O0 -fomit-frame-pointer -Wall -Wextra -Wstrict-prototypes
LDOPT   = -g 
 
 
# ******* Start: Your Project Settings **************
 
 
# Your project directories
UINCDIR =
ULIBDIR =
 
# Your project definitions and libraries
UDEFS   =
ULIBS   =
 
# Your project executable
PROJECT = uGFX.exe
 
# Your project C source files
SRC     = main.c
# Your project uGFX drivers
GFXINC +=   ./

#include $(GFXLIB)/boards/base/Win32/board.mk
#include $(GFXLIB)/demos/modules/gwin/widgets/demo.mk
include $(GFXLIB)/drivers/multiple/Win32/driver.mk
# ******* End: Your Project Settings **************
 
 
# The default directories, libraries and source $(MYCSRC)
include $(GFXLIB)/gfx.mk
DSRC    = $(GFXSRC) 
DINCDIR = $(GFXINC) $(MYFILES)
DLIBDIR = 
DLIBS   = ws2_32 gdi32
DDEFS   =
 
# Putting it all together - Object files and compiler flags
OBJS    = $(DSRC:.c=.o) $(SRC:.c=.o)
LDFLAGS = $(LDOPT) $(patsubst %,-L%, $(ULIBDIR) $(DLIBDIR)) $(patsubst %,-l%, $(ULIBS) $(DLIBS))
CCFLAGS = $(CCOPT) $(patsubst %,-D%, $(UDEFS) $(DDEFS)) $(patsubst %,-I%, . $(UINCDIR) $(DINCDIR)) -DGFX_USE_OS_WIN32=TRUE
 
# Makefile rules 
all: $(OBJS) $(PROJECT) 
 
%.o : %.c       
	$(CC) -DGFX_USE_OS_WIN32=TRUE -c $(CCFLAGS) $< -o $@ 
 
%.exe: $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) -o $@
 
run: $(PROJECT)
	./$(PROJECT)
 
clean:
	-rm -f $(OBJS)   
	-rm -f $(PROJECT)