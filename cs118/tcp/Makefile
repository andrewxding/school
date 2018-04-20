CXX=g++
CXXOPTIMIZE= -O2
CXXFLAGS= -g -Wall -pthread -std=c++11 $(CXXOPTIMIZE)
CLASSES=packet.cpp
default: server
all: server client
server: *.cpp
	$(CXX) -o $@ $(CLASSES) $(CXXFLAGS) server.cpp TCP_connection.cpp TCP_server.cpp
client: *.cpp
	$(CXX) -o $@ $(CLASSES) $(CXXFLAGS) client.cpp TCP_connection.cpp 