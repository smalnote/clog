#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>

#define BUFFER_SIZE 4096

int read_send(int filefd, int sockfd, int *sys_call_count) {
  char buffer[BUFFER_SIZE];
  ssize_t bytes_read, bytes_sent;

  // Read the file and send it over the TCP connection
  while ((bytes_read = read(filefd, buffer, BUFFER_SIZE)) > 0) {
    (*sys_call_count) += 2;
    bytes_sent = send(sockfd, buffer, bytes_read, 0);
    if (bytes_sent == -1) {
      perror("Send failed");
      return -1;
    }
  }

  if (bytes_read == -1) {
    perror("File reading failed");
    return -1;
  }

  return 0;
}
