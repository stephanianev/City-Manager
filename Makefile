CXX = g++
CXXFLAGS = -std=c++17 -Wall

SRC = src/main.cpp \
      src/core/CityManager.cpp \
      src/models/Citizen.cpp \
      src/models/Building.cpp \
      src/models/ResidentialBuilding.cpp \
      src/models/CommercialBuilding.cpp \
      src/models/IndustrialBuilding.cpp \
      src/models/ServiceBuilding.cpp \
      src/serialization/CitySerializer.cpp

TARGET = app

all:
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)

clean:
	rm -f $(TARGET) $(TARGET).exe