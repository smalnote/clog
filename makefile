zero_copy: zero_copy.c read_send.c mmap_send.c send_file.c splice_file.c
	clang -o ./bin/zero_copy zero_copy.c read_send.c mmap_send.c send_file.c splice_file.c

# Read file and send to unix socket server
listen_unix:
	./listen_unix.sh /tmp/zero_copy.sock &

wait:
	sleep 2

TEST_FILE = ./bin/small_file

test_read_unix: zero_copy listen_unix wait
	./bin/zero_copy read_send $(TEST_FILE) /tmp/zero_copy.sock

test_mmap_unix: zero_copy listen_unix wait
	./bin/zero_copy mmap_send $(TEST_FILE) /tmp/zero_copy.sock

test_send_unix: zero_copy listen_unix wait
	./bin/zero_copy send_file $(TEST_FILE) /tmp/zero_copy.sock

test_splice_unix: zero_copy listen_unix wait
	 ./bin/zero_copy splice_file $(TEST_FILE) /tmp/zero_copy.sock

# Read file and send to TCP server
listen_tcp:
	nc -4 -l 8022 >/dev/null

test_read_tcp: zero_copy
	./bin/zero_copy read_send $(TEST_FILE) 192.168.31.10 8022

test_mmap_tcp: zero_copy
	./bin/zero_copy mmap_send $(TEST_FILE) 192.168.31.10 8022

large_file:
	dd if=/dev/random of=./bin/large_file bs=1M count=1024

small_file:
	dd if=/dev/random of=./bin/small_file bs=1M count=1
