.PHONY: all clean

CXX = g++
CXXFLAGS = -g -std=c++17 -Iinclude -I $(IDIR) -pthread
IDIR = ../../../include
LIB = ../../lib/backendUtilsLib.a
TARGET = ../../exe/svrBackend
SRC = main.cpp

all: $(TARGET)

$(LIB):
	$(MAKE) -C utils

$(TARGET): $(SRC) $(LIB) $(IDIR)/database.h
	$(CXX) $(CXXFLAGS) $(SRC) $(LIB) -o $(TARGET)

clean:
	rm -f $(TARGET) *.bin
