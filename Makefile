CXX = g++
CXXFLAGS = -std=c++17 -I/opt/homebrew/include -Wno-deprecated-declarations
LDFLAGS = -L/opt/homebrew/lib -lraylib


SRC = main.cpp Vendor/tinyfiledialogs.c
TARGET = cpu_sim

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(SRC) -o $(TARGET) $(CXXFLAGS) $(LDFLAGS)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)