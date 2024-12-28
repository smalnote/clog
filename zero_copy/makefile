# Compiler and flags
CC = clang
CFLAGS = -Wall -Wextra -O2

# Source files and object files
SRC = $(wildcard *.c)
OBJ = $(patsubst %.c, ./bin/%.o, $(SRC))

# Output directory and target executable
BIN_DIR = ./bin
TARGET = $(BIN_DIR)/zero_copy

# Rule to build the target, first target is the default target
$(TARGET): $(OBJ)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^

# Rule to compile .c files into .o files
./bin/%.o: %.c
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up generated files
clean:
	rm -rf $(BIN_DIR)

# Phony targets
.PHONY: clean

# Read file and send to unix socket server
listen_unix:
	./listen_unix.sh /tmp/zero_copy.sock &

bin/small_file:
	dd if=/dev/random of=./bin/small_file bs=1M count=1

bin/large_file:
	mkdir -p ./bin && dd if=/dev/urandom of=./bin/large_file bs=1M count=1024

TEST_FILE = bin/large_file

nc_unix_listener:
	(nc -lU /tmp/zero_copy.sock >/dev/null && rm /tmp/zero_copy.sock &) && sleep 0.5

METHOD = ""

command_unix_socket: $(TARGET) $(TEST_FILE)
	time $(TARGET) $(METHOD) $(TEST_FILE) /tmp/zero_copy.sock

read_send:
	$(eval METHOD := read_send)
test_read_unix: nc_unix_listener read_send command_unix_socket

mmap_send:
	$(eval METHOD := mmap_send)
test_mmap_unix: nc_unix_listener mmap_send command_unix_socket

send_file:
	$(eval METHOD := send_file)
test_send_unix: nc_unix_listener send_file command_unix_socket

splice_file:
	$(eval METHOD := splice_file)
test_splice_unix: nc_unix_listener splice_file command_unix_socket

# Read file and send to TCP server
listen_tcp:
	nc -4 -l 8022 >/dev/null

test_read_tcp: zero_copy
	./bin/zero_copy read_send $(TEST_FILE) 192.168.31.10 8022

test_mmap_tcp: zero_copy
	./bin/zero_copy mmap_send $(TEST_FILE) 192.168.31.10 8022

test_time:
	time sleep 3
