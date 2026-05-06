CXX = g++
CXXFLAGS = -std=c++17 -Wall

SRC = src/main.cpp \
      src/core/CityManager.cpp \
      src/models/Citizen.cpp \
      src/models/Building.cpp

TARGET = app

all:
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)

clean:
	rm -f $(TARGET) $(TARGET).exe