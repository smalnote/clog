# Compiler and flags
CC = clang
# -O2 will optimize out C loop
CFLAGS = -Wall -Wextra -I$(DEVKIT)/lua5.1/include
LDFLAGS = -L$(DEVKIT)/lua5.1/lib
LIBS = -llua51 -lm

# Source files and object files
SRC = $(wildcard *.c)
OBJ = $(patsubst %.c, ./bin/%.o, $(SRC))

# Output directory and target executable
BIN_DIR = ./bin
TARGET = $(BIN_DIR)/lua_demo

# Rule to build the target, first target is the default target
$(TARGET): $(OBJ)
	@mkdir -p $(BIN_DIR)
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)

# Rule to compile .c files into .o files
./bin/%.o: %.c
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -c $< -o $@


run_lua: $(TARGET)
	time $(TARGET) ./hello.lua

run_c: $(TARGET)
	time $(TARGET)

lua_lua:
	time lua ./hello.lua

# Clean up generated files
clean:
	rm -rf $(BIN_DIR)

# Phony targets
.PHONY: clean run_lua run_c
