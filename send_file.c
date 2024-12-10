#include <stdio.h>
#include <sys/mman.h>
#include <sys/sendfile.h>
#include <sys/stat.h>

int send_file(int filefd, int sockfd) {
  off_t offset = 0;
  size_t file_size;
  ssize_t bytes_sent;

  // Get file size
  struct stat file_stat;
  if (fstat(filefd, &file_stat) == -1) {
    perror("fstat failed");
    return -1;
  }
  file_size = file_stat.st_size;

  // Send the file data
  bytes_sent = sendfile(sockfd, filefd, &offset, file_size);
  if (bytes_sent == -1) {
    perror("Failed to send file");
    return -1;
  }

  return 0;
}
