CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -Wpedantic

TARGET := rideshare
SOURCES := main.cpp RideSharing.cpp

$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(TARGET)

.PHONY: clean
clean:
	rm -f $(TARGET)
