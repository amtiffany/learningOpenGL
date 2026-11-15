CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O0 -g
LDLIBS = -lGLEW -lglut -lGL

SOURCES = main.cpp mesh.cpp parseSimpleObj.cpp
TARGET = viewer

$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(TARGET) $(LDLIBS)

.PHONY: clean
clean:
	rm -f $(TARGET)