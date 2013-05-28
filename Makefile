
LDFLAGS += -framework Cocoa -framework IOKit -lstdc++
CXXFLAGS += -MMD

PROJECT = maclight

SOURCES = maclight.cpp lights_handle.cpp
OBJECTS = ${patsubst %.cpp, %.o, ${SOURCES}}

DEPS	= ${wildcard *.d}



${PROJECT}: ${OBJECTS}

run: ${PROJECT}
	./${PROJECT}


-include ${DEPS}