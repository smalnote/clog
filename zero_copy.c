#include <arpa/inet.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <time.h>
#include <unistd.h>

#include "mmap_send.h"
#include "read_send.h"
#include "send_file.h"
#include "splice_file.h"

// Read whole file content from filefd and write it to sockfd.
typedef int (*CopyFunc)(int filefd, int sockfd, int *sys_call_count);

int copy_file(const char *method, int filefd, int sockfd, int *sys_call_count) {
  CopyFunc copy_func = NULL;
  if (strcmp(method, "read_send") == 0) {
    copy_func = read_send;
  } else if (strcmp(method, "mmap_send") == 0) {
    copy_func = mmap_send;
  } else if (strcmp(method, "send_file") == 0) {
    copy_func = send_file;
  } else if (strcmp(method, "splice_file") == 0) {
    copy_func = splice_file;
  } else {
    fprintf(stderr, "Method '%s' not supported\n", method);
    return EXIT_FAILURE;
  }

  if (copy_func(filefd, sockfd, sys_call_count) != 0) {
    printf("File copying failed.\n");
    return -1;
  }

  printf("File copied successfully.\n");
  return 0;
}

int send_to_tcp_socket(int argc, char *argv[], int *sys_call_count) {
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

  int sockfd, filefd;
  struct sockaddr_in server_addr;
  struct stat file_stat;
  void *file_data;
  size_t file_size;
  ssize_t bytes_sent, total_sent;

  // Create socket
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd == -1) {
    perror("Socket creation failed");
    return -1;
  }

  // Setup server address struct
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(server_port);
  if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0) {
    perror("Invalid server IP address");
    close(sockfd);
    return -1;
  }

  // Connect to the server
  if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) ==
      -1) {
    perror("Connection failed");
    close(sockfd);
    return -1;
  }

  // Open the file for reading
  filefd = open(file_path, O_RDONLY);
  if (filefd == -1) {
    perror("File opening failed");
    close(sockfd);
    return -1;
  }

  if (copy_file(method, filefd, sockfd, sys_call_count) != 0) {
    perror("Copy file failed");
    close(filefd);
    close(sockfd);
    return -1;
  }

  close(filefd);
  close(sockfd);
  return 0;
}

int send_to_unix_socket(int argc, char *argv[], int *sys_call_count) {
  if (argc != 4) {
    fprintf(stderr, "Usage: %s <method> <file_path> <socket_path>\n", argv[0]);
    return EXIT_FAILURE;
  }
  const char *method = argv[1];
  const char *file_path = argv[2];
  const char *socket_path = argv[3];

  printf("Sending file '%s' to '%s'\n", file_path, socket_path);

  int sockfd, filefd;
  struct sockaddr_un server_addr;

  // Create socket
  sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
  if (sockfd == -1) {
    perror("Socket creation failed");
    return -1;
  }

  // Setup server address struct
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sun_family = AF_UNIX;
  strncpy(server_addr.sun_path, socket_path, sizeof(server_addr.sun_path) - 1);

  // Connect to the server
  if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) ==
      -1) {
    perror("Connection failed");
    close(sockfd);
    return -1;
  }

  // Open the file for reading
  filefd = open(file_path, O_RDONLY);
  if (filefd == -1) {
    perror("File opening failed");
    close(sockfd);
    return -1;
  }

  if (copy_file(method, filefd, sockfd, sys_call_count) != 0) {
    perror("Copy file failed");
    close(filefd);
    close(sockfd);
    return -1;
  }

  close(filefd);
  close(sockfd);
  return 0;
}
int main(int argc, char *argv[]) {
  clock_t start, end;
  double cpu_time_used;

  start = clock();

  int ret_code;
  int sys_call_count = 0;
  if (argc == 5) {
    ret_code = send_to_tcp_socket(argc, argv, &sys_call_count);
  } else {
    ret_code = send_to_unix_socket(argc, argv, &sys_call_count);
  }

  end = clock();

  cpu_time_used = ((double)(end - start) / CLOCKS_PER_SEC);

  printf("***Metrics: %ld clocks, %f seconds, %d syscalls\n", (end - start),
         cpu_time_used, sys_call_count);

  return ret_code;
}
