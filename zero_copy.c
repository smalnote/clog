#include <arpa/inet.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "mmap_send.h"
#include "read_send.h"

#define BUFFER_SIZE 4096

typedef int (*SendFunc)(const char *file_path, const char *server_ip,
                        int server_port);

int main(int argc, char *argv[]) {
  if (argc != 5) {
    fprintf(stderr,
            "Usage: %s <method> <file_path> <server_ip> <server_port>\n",
            argv[0]);
    return EXIT_FAILURE;
  }
  const char *method = argv[1];
  const char *file_path = argv[2];
  const char *server_ip = argv[3];
  int server_port = atoi(argv[4]);

  if (server_port <= 0 || server_port >= (1 << 16)) {
    fprintf(stderr, "Invalid port number: %d\n", server_port);
    return EXIT_FAILURE;
  }

  printf("Sending file '%s' to '%s:%d'\n", file_path, server_ip, server_port);

  SendFunc send_func = NULL;
  if (strcmp(method, "read_send") == 0) {
    send_func = read_send;
  } else if (strcmp(method, "mmap_send") == 0) {
    send_func = mmap_send;
  } else {
    fprintf(stderr, "Method '%s' not supported\n", method);
    return EXIT_FAILURE;
  }

  if (send_func(file_path, server_ip, server_port) == 0) {
    printf("File sent successfully to %s:%d\n", server_ip, server_port);
  } else {
    printf("File sending failed.\n");
  }
  return EXIT_SUCCESS;
}
