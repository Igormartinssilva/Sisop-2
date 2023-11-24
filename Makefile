CC=g++
CFLAGS=-std=c++11 -Wall
LDFLAGS=

SOURCES=message.cpp session.cpp user.cpp client.cpp run.cpp
HEADERS=message.hpp session.hpp user.hpp client.hpp

OBJECTS=$(SOURCES:.cpp=.o)

TARGET=twitter

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

%.o: %.cpp $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)

.PHONY: all clean
