.PHONY: all clean dirs
CXX = g++
CXXFLAGS = -g -std=c++17 -Iinclude -I../../../include -pthread
TARGET = ../exe/svrWeb
LIB = ../lib/frontendUtilsLib.a
SRC = webServer.cpp

all: dirs $(LIB) $(TARGET)

dirs:
	mkdir -p ../exe ../lib

$(LIB):
	make -C utils

$(TARGET): $(LIB) $(SRC) 
	$(CXX) $(CXXFLAGS) $(SRC) $(LIB) -o $@

clean:
	${RM} $(TARGET)
	make -C utils clean
