# Linux makefile
#

# Target and source files
TARGET = lab6
SOURCES = $(wildcard *.cpp)
OBJECTS = $(SOURCES:.cpp=.o)

# Always compile 32-bit code
ARCH=-march=i686 -m32

# Include sources from the glutil "library"
GLUTIL = ../glutil
GLUTIL_SRC = $(wildcard $(GLUTIL)/*.cpp)
GLUTIL_OBJ = $(GLUTIL_SRC:.cpp=.o)

# Setup paths (LPATH = linker input, RPATH = run-time path)
LPATH = -L../linux/lib -L../linux/lib64 
RPATH = -Wl,-rpath,../linux/lib,-rpath,../linux/lib64
IPATH = -I../linux/include -I$(GLUTIL)

# Libraries 
LIBS = -lIL -lILU -lILUT -lGL -lglut -lGLEW  

# C++ Compiler flags
CXXDEFS = -DILUT_USE_OPENGL
CXXFLAGS = $(ARCH) -g3 -Wall -Wextra $(CXXDEFS) $(IPATH) 

# Build targets
all : $(TARGET)

$(TARGET) : $(OBJECTS) $(GLUTIL_OBJ)
	g++ $(ARCH) $^ -o $@ $(LPATH) $(RPATH) $(LIBS)

# "Pseudo" target 'clean' (remove clutter)
clean: 
	rm -f $(OBJECTS)
	rm -f $(TARGET)

#EOF
