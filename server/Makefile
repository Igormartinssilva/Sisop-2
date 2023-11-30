CC = g++
CFLAGS = -std=c++11 -Wall
BIN_DIR = ../bin
HEADER_DIR = ../header
COMMON_DIR = ../common

# Lista de arquivos fonte
SOURCES = server2.cpp
COMMON_SOURCES = $(COMMON_DIR)/data.cpp
OBJECTS = $(SOURCES:%.cpp=$(BIN_DIR)/%.o) $(COMMON_SOURCES:$(COMMON_DIR)/%.cpp=$(BIN_DIR)/%.o)

# Nome do executável
EXECUTABLE = $(BIN_DIR)/server2

all: $(BIN_DIR) $(HEADER_DIR) $(EXECUTABLE)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(HEADER_DIR):
	mkdir -p $(HEADER_DIR)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

$(BIN_DIR)/%.o: %.cpp
	$(CC) $(CFLAGS) -c -o $@ $<

$(BIN_DIR)/%.o: $(COMMON_DIR)/%.cpp
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -rf $(BIN_DIR) $(HEADER_DIR) $(EXECUTABLE)
