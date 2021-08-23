#
#  Makefile 
#

include ../Makefile.general

INCDIRS = -I.

CC_SOURCES =  $(shell ls *.cc)
CC_SOURCES := $(notdir $(CC_SOURCES))
CC_OBJECTS =  $(CC_SOURCES:%.cc=%.o)

C_SOURCES  =  $(shell ls *.c)
C_SOURCES  := $(notdir $(C_SOURCES))
C_OBJECTS  =  $(C_SOURCES:%.c=%.o)

OBJECTS = $(CC_OBJECTS)  $(C_OBJECTS) 
SOURCES = $(CC_SOURCES)  $(C_SOURCES) 

all: $(OBJECTS)

include ../Makefile.rules

clean: rmobj rmdep

mrproper: clean rmbak







