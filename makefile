zero_copy: zero_copy.c read_send.c mmap_send.c send_file.c
	clang -o ./bin/zero_copy zero_copy.c read_send.c mmap_send.c send_file.c

# Read file and send to unix socket server
listen-unix:
	./listen_unix.sh /tmp/zero_copy.sock

test_read_unix: zero_copy
	time ./bin/zero_copy read_send ./bin/large_file /tmp/zero_copy.sock

test_mmap_unix: zero_copy
	time ./bin/zero_copy mmap_send ./bin/large_file /tmp/zero_copy.sock

test_send_file: zero_copy
	time ./bin/zero_copy send_file ./bin/large_file /tmp/zero_copy.sock

# Read file and send to TCP server
listen-tcp:
	nc -4 -l 8022 >/dev/null

test_read_tcp: zero_copy
	time ./bin/zero_copy read_send ./bin/large_file 192.168.31.10 8022

test_mmap_tcp: zero_copy
	time ./bin/zero_copy mmap_send ./bin/large_file 192.168.31.10 8022

large_file:
	dd if=/dev/random of=./bin/large_file bs=1M count=1024
