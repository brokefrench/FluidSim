# Compiler and flags
CXX     := g++
CXXFLAGS := -Wall -Wextra -std=c++17 -g

# Directories
SRC_DIR := src
OBJ_DIR := obj
BIN_DIR := exec

# Files
SRC     := $(wildcard $(SRC_DIR)/*.cpp)
OBJ     := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRC))
TARGET  := $(BIN_DIR)/main   # Final executable

#Default

all:
	@echo "Run commands 'make build', 'make run', 'make clean', 'make memory'"


# Build 

build: $(TARGET)

$(TARGET): $(OBJ)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@


# Run / Clean / Check

run: build
	@echo ">> Running executable"
	@$(TARGET)

clean:
	@echo ">> Cleaning up"
	@rm -rf $(OBJ_DIR)/* $(BIN_DIR)/*

check:
	@echo ">> Running Valgrind"
	valgrind --leak-check=full --show-leak-kinds=all $(TARGET)


.PHONY: all build run clean check
