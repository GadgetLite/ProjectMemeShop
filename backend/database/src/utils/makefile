CXX = g++
CXXFLAGS = -g -std=c++17 -Iinclude -I $(IDIR) -pthread
IDIR = ../../../../include
LIB = ../../../lib/backendUtilsLib.a
SRCS = $(wildcard *.cpp)
OBJS = $(SRCS:.cpp=.o)

$(LIB): $(OBJS)
	ar rcs $@ $^

%.o: %.cpp $(IDIR)/database.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(LIB)
