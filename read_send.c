#include <arpa/inet.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define BUFFER_SIZE 4096

// Function to send a file over TCP
int read_send(const char *file_path, const char *server_ip, int server_port) {
  int sockfd, filefd;
  struct sockaddr_in server_addr;
  char buffer[BUFFER_SIZE];
  ssize_t bytes_read, bytes_sent;

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

  // Read the file and send it over the TCP connection
  while ((bytes_read = read(filefd, buffer, BUFFER_SIZE)) > 0) {
    bytes_sent = send(sockfd, buffer, bytes_read, 0);
    if (bytes_sent == -1) {
      perror("Send failed");
      close(filefd);
      close(sockfd);
      return -1;
    }
  }

  if (bytes_read == -1) {
    perror("File reading failed");
  }

  // Close the file and socket
  close(filefd);
  close(sockfd);

  return 0;
}
