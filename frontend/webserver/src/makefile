.PHONY: all clean
CXX = g++
CXXFLAGS = -g -std=c++17 -Iinclude -I../../../include -pthread
TARGET = ../exe/svrWeb
LIB = ../lib/frontendUtilsLib.a
SRC = webServer.cpp

all: $(LIB) $(TARGET)

$(LIB):
	make -C utils

$(TARGET): $(LIB) $(SRC) 
	$(CXX) $(CXXFLAGS) $(SRC) $(LIB) -o $@

clean:
	${RM} $(TARGET)
	make -C utils clean
