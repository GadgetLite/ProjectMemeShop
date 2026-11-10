CXX = g++
CXXFLAGS = -g -std=c++17 -Iinclude -I../../../../include -pthread
LIB = ../../lib/frontendUtilsLib.a
SRCS = $(wildcard *.cpp)
OBJS = $(SRCS:.cpp=.o)

$(LIB): $(OBJS)
	$(AR) rv $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(LIB)
