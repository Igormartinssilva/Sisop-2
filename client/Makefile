CC = g++
CFLAGS = -std=c++11 -Wall -pthread
BIN_DIR = ../bin
HEADER_DIR = ../header
COMMON_DIR = ../common
DATABASE_DIR = ../database

# Lista de arquivos fonte
SOURCES = client.cpp client_interface.cpp message.cpp session.cpp
COMMON_SOURCES = $(COMMON_DIR)/data.cpp $(COMMON_DIR)/utils.cpp
DATABASE_SOURCES = $(DATABASE_DIR)/database.cpp
OBJECTS = $(SOURCES:%.cpp=$(BIN_DIR)/%.o) $(COMMON_SOURCES:$(COMMON_DIR)/%.cpp=$(BIN_DIR)/%.o) $(DATABASE_SOURCES:$(DATABASE_DIR)/%.cpp=$(BIN_DIR)/%.o)

# Nome do executável
EXECUTABLE = $(BIN_DIR)/client_executable

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

$(BIN_DIR)/%.o: $(DATABASE_DIR)/%.cpp
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -rf $(BIN_DIR) $(HEADER_DIR) $(EXECUTABLE)