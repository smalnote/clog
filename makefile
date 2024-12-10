zero_copy: zero_copy.c read_send.c mmap_send.c
	clang -o ./bin/zero_copy zero_copy.c read_send.c mmap_send.c

test_read_send: zero_copy
	time ./bin/zero_copy read_send ./bin/large_file 192.168.31.10 8022

test_mmap_send: zero_copy
	time ./bin/zero_copy mmap_send ./bin/large_file 192.168.31.10 8022

listen:
	nc -4 -l 8022 >/dev/null

large_file:
	dd if=/dev/random of=./bin/large_file bs=1M count=1024
