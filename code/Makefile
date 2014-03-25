PROJECT_NAME	= collision
#Path to shared directory, which contains some scripts
SHARED_PATH	= ./shared

#Add modules to project. Each module is a separate (sub)directory
MODULES := src
QT_MOC_DIR := 

#Define execuatable name and library name/location 
EXE		= collision

#Default rule
all: _main

#Include other paths
INCLUDES         = -I./include

#Include some other shared libraries
LIBS	+= -lpthread 

CUDA	= 1
DEBUG   = 0

ifeq ($(DEBUG),1)
STRIP_FLAGS		= 
COMPILATION_FLAGS       = -O0 -g -D_DEBUG
else
STRIP_FLAGS		= #-ffunction-sections -fdata-sections 
COMPILATION_FLAGS       = -O4 -DN_DEBUG
endif

ifeq ($(CUDA),1)
CUDA_LIBS		= -lcudart 
else
CUDA_LIBS		=
endif

#Compiler flags
# CCFLAGS := $(INCLUDES)  -Werror $(COMPILATION_FLAGS) $(STRIP_FLAGS) -DUNIX $(GPU) -D_REENTRANT -ansi -pedantic $(SSE2) $(BENCHMARK) -Wno-long-long -DNOISE

CCFLAGS := $(INCLUDES) --compiler-options '-Wall -Werror $(COMPILATION_FLAGS) $(STRIP_FLAGS) -DUNIX $(GPU) '


#CUDA Compiler flags
ifeq ($(CUDA),1)
NVCCFLAGS	+= -Xcompiler "" $(INCLUDES) -DUNIX -DCUDA $(COMPILATION_FLAGS) -arch sm_13 
#--compiler-bindir=/usr/bin/gcc-4.4 
#--device-emulation
CCFLAGS		+= -DCUDA	
else
NVCCFLAGS	=
endif

#Compilers
CC 	= nvcc
CPP	= nvcc
ifeq ($(CUDA),1)
NVCC	= nvcc
else
NVCC	=
endif

#Default seachpath for MAKE
VPATH 	= include:$(CGF_INCLUDE_PATH):$(CGF_LIB_PATH)

#Automatically lookup our sources. When a source file is in a module
#directory, it wil be find, compiled and linked automatically
CFILES	:= $(wildcard $(patsubst %, %/*.c, $(MODULES)))
CCFILES	:= $(wildcard $(patsubst %, %/*.cpp, $(MODULES)))
CUFILES	:= $(wildcard $(patsubst %, %/*.cu, $(MODULES)))
HPPFILES:= $(wildcard $(patsubst %, %/*.hpp, include))
MPPFILES:= $(wildcard $(patsubst %, %/*.hpp, $(QT_MOC_DIR)))

#Create a list of object ande dependency files
ifeq ($(CUDA),1)
OBJ	:= $(CFILES:.c=.o) $(CCFILES:.cpp=.o) $(CUFILES:.cu=.o)
else
OBJ	:= $(CFILES:.c=.o) $(CCFILES:.cpp=.o)
endif

DEPS 	:= $(OBJ:.o=.dep)
COBJ	+= $(OBJ)
ifeq ($(CUDA),1)
CUBIN	:= $(CUFILES:.cu=.cubin)
CUPTX 	:= $(CUFILES:.cu=.ptx)
else
CUBIN	:= 
CUPTX	:=
endif

MOCS	:= $(MPPFILES:.hpp=.moc)

#Temporary emacs files
TEMPFILES += $(CFILES:.c=.c~) $(CCFILES:.cpp=.cpp~) $(CUFILES:.cu=.cu~)
TEMPFILES += $(HPPFILES:.hpp=.hpp~)

#Rules for compiling different files
%.o:	%.c
	$(CC) $(CCFLAGS) -c $< -o $@ 

%.o:	%.cpp
	$(CPP) $(CCFLAGS) -c $< -o $@

%.s:	%.cpp
	$(CPP) $(CCFLAGS) -S $< -o $@

ifeq ($(CUDA),1)
%.o:	%.cu
	$(NVCC) -o $@ -c  $< $(NVCCFLAGS) 
else

endif

%.moc:	%.hpp
	$(MOC4) -o $@ $<
#	$(MOC4) -o $@ $<	

ifeq ($(CUDA),1)
%.cubin:	%.cu
	$(NVCC) -o $@ $< $(NVCCFLAGS) --cubin 

%.ptx:	%.cu
	$(NVCC) -o $@ $< $(NVCCFLAGS) --ptx 
else

endif


touchdep: $(DEPS) 
	@touch $(DEPS)

#Rule for linking main executable
# $(CUBIN) #$(CUPTX) #$(ASM)
ifeq ($(CUDA),1)
_main:	$(DEPS) $(OBJ) touchdep
	$(NVCC) -o $(EXE) $(COBJ) $(LIBS)
else
_main:	$(DEPS) $(OBJ) touchdep
	$(CC) -o $(EXE) $(COBJ) $(LIBS) 
#-Wl,-s -Wl,--gc-sections
endif

#Rules for creating dependencies
%.dep: 	%.c
	@./$(SHARED_PATH)/dep.sh `dirname $*.c` $(CCFLAGS) $*.c > $@	

%.dep: 	%.cpp
	@./$(SHARED_PATH)/dep.sh `dirname $*.cpp` $(CCFLAGS) $*.cpp > $@	

ifeq ($(CUDA),1)
%.dep: 	%.cu
	@./$(SHARED_PATH)/nvdep.sh `dirname $*.cu` $(NVCCFLAGS) $*.cu > $@	
else

endif

.PHONY: clean
#Clean up
clean:
	rm -f $(OBJ) *.linkinfo include/.version.hpp include/.lversion.hpp $(CUBIN) $(CUPTX) $(ASM) $(MOCS) $(TEMPFILES) Makefile~ 

ifneq ($(MAKECMDGOALS),clean_dep)
ifneq ($(MAKECMDGOALS),clean)
include $(DEPS)
endif
endif

clean_dep: clean
	rm -f $(DEPS)

