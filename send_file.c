#include <stdio.h>
#include <sys/mman.h>
#include <sys/sendfile.h>
#include <sys/stat.h>

int send_file(int filefd, int sockfd) {
  off_t offset = 0;
  ssize_t bytes_sent;

  // Get file size
  struct stat file_stat;
  if (fstat(filefd, &file_stat) == -1) {
    perror("fstat failed");
    return -1;
  }

  int sys_call_count = 0;

  size_t len;
  len = file_stat.st_size;
  // Send the file data
  while (len > 0) {
    sys_call_count++;
    bytes_sent = sendfile(sockfd, filefd, &offset, len);
    if (bytes_sent == -1) {
      perror("Failed to send file");
      return -1;
    }
    len -= bytes_sent;
  }

  printf("Number of system call to sendfile: %d\n", sys_call_count);

  return 0;
}
