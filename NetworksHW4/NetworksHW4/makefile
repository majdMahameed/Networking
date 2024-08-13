CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++11
TARGET = simulator

SRCS = simulator.cpp
OBJS = $(SRCS:.cpp=.o)

all:$(TARGET)
	chmod +x $(TARGET) 

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

%.o: %.cpp $(HDRS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

.PHONY: all