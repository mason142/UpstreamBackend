# Makefile for a C++ project

# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -std=c++11 -g

# Source files
SRCS = main.cpp Data/StreamDatabase.cpp Network/CustomHTTPReq.cpp Network/CustomHTTPRes.cpp Network/CustomSocket.cpp Data/StreamVideoBuffer.cpp Data/SlowStreamCircularBuffer.cpp Utilities/httputils.cpp

TARGET = server

# Build rule
all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) -o $@ $^ -lPocoFoundation -lPocoNet -lPocoUtil

# Clean rule
clean:
	rm -f $(TARGET)