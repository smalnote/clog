#include <arpa/inet.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>

int mmap_send(int filefd, int sockfd, int *sys_call_count) {
  void *file_data;
  size_t file_size, total_sent = 0;
  ssize_t bytes_sent;

  // Get file size
  struct stat file_stat;
  if (fstat(filefd, &file_stat) == -1) {
    perror("fstat failed");
    return -1;
  }
  file_size = file_stat.st_size;

  (*sys_call_count)++;
  // Memory-map the file, return a pointer to mapped memory buffer
  file_data = mmap(NULL, file_size, PROT_READ, MAP_PRIVATE, filefd, 0);
  if (file_data == MAP_FAILED) {
    perror("mmap failed");
    return -1;
  }

  // Send the file data
  while (total_sent < file_size) {
    (*sys_call_count)++;
    bytes_sent =
        send(sockfd, file_data + total_sent, file_size - total_sent, 0);
    if (bytes_sent == -1) {
      perror("Send failed");
      munmap(file_data, file_size);
      return -1;
    }
    total_sent += bytes_sent;
  }

  // Cleanup
  munmap(file_data, file_size);

  return 0;
}
