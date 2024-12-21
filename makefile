zero_copy: zero_copy.c read_send.c mmap_send.c send_file.c splice_file.c
	clang -o ./bin/zero_copy zero_copy.c read_send.c mmap_send.c send_file.c splice_file.c

# Read file and send to unix socket server
listen_unix:
	./listen_unix.sh /tmp/zero_copy.sock &

small_file:
	dd if=/dev/random of=./bin/small_file bs=1M count=1

TEST_FILE = ./bin/large_file

large_file:
	mkdir -p ./bin && dd if=/dev/urandom of=./bin/large_file bs=1M count=1024
METHOD = ""

nc_unix_listener:
	(nc -lU /tmp/zero_copy.sock >/dev/null && rm /tmp/zero_copy.sock &) && sleep 0.5

command_unix_socket: zero_copy
	time ./bin/zero_copy $(METHOD) $(TEST_FILE) /tmp/zero_copy.sock

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
