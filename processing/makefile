.PHONY: all clean dirs
CXX = g++
CXXFLAGS = -g -std=c++17 -Iinclude -I../include -DCPPHTTPLIB_OPENSSL_SUPPORT 
LDFLAGS = -lssl -lcrypto -lpthread
TARGET = exe/svrWeb
LIB = lib/frontendUtilsLib.a
SRC = src/webServer.cpp

all: dirs $(LIB) $(TARGET)

dirs:
	mkdir -p exe lib

$(LIB):
	make -C src/utils

$(TARGET): $(LIB) $(SRC) 
	$(CXX) $(CXXFLAGS) $(SRC) $(LIB) -o $@ $(LDFLAGS)

clean:
	${RM} $(TARGET)
	make -C utils clean
