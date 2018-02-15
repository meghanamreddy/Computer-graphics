CC = g++
CXX = $(CC)
DEBUG = -g -Wno-deprecated -fPIC

TARGETS = assign5
C_SRCS = model.cpp controller.cpp view.cpp main.cpp
#OBJS = ${C_SRCS:.c=.o}

# release settings
#CFLAGS = -O2 -I/usr/include
#LFLAGS = -s -L/usr/lib

# debug settings
CFLAGS = $(DEBUG) $(DEFINES) -I/usr/include 
LFLAGS = -g -L/usr/lib -lglut -lglui -lGL -lGLU -lSOIL -lXi -lXmu -lm

CXXFLAGS = $(CFLAGS)

all: ${TARGETS}
	mv ${TARGETS} bin/


assign5: source/model.cpp source/controller.cpp source/view.cpp source/main.cpp
	g++ source/trackball.cpp source/model.cpp source/controller.cpp source/view.cpp source/main.cpp -o assign5 ${LFLAGS} -std=c++11

clean:
	rm -f bin/*



