#include <arpa/inet.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>

// Function to send a file over TCP
int mmap_send(const char *file_path, const char *server_ip, int server_port) {
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

  // Get file size
  if (fstat(filefd, &file_stat) == -1) {
    perror("fstat failed");
    close(filefd);
    close(sockfd);
    return -1;
  }
  file_size = file_stat.st_size;

  // Memory-map the file, return a pointer to mapped memory buffer
  file_data = mmap(NULL, file_size, PROT_READ, MAP_PRIVATE, filefd, 0);
  if (file_data == MAP_FAILED) {
    perror("mmap failed");
    close(filefd);
    close(sockfd);
    return -1;
  }

  // Send the file data
  while (total_sent < file_size) {
    bytes_sent =
        send(sockfd, file_data + total_sent, file_size - total_sent, 0);
    if (bytes_sent == -1) {
      perror("Send failed");
      munmap(file_data, file_size);
      close(filefd);
      close(sockfd);
      return -1;
    }
    total_sent += bytes_sent;
  }

  // Cleanup
  munmap(file_data, file_size);
  close(filefd);
  close(sockfd);

  return 0;
}
