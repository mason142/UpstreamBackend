# Makefile for a C++ project

# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -std=c++11 -g -lPocoFoundation -lPocoNet -lPocoUtil

# Source files
SRCS = main.cpp CustomHTTPReq.cpp CustomHTTPRes.cpp CustomSocket.cpp StreamVideoBuffer.cpp SlowStreamCircularBuffer.cpp

TARGET = myprogram

# Build rule
all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Clean rule
clean:
	rm -f $(TARGET)