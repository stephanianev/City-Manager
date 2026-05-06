CXX = g++
CXXFLAGS = -std=c++17 -Wall

SRC = $(wildcard src/**/*.cpp) src/main.cpp

app:
	$(CXX) $(CXXFLAGS) $(SRC) -o app

clean:
	rm -f app