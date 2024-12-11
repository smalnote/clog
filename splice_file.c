// For include splice
#define _GNU_SOURCE

#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int splice_file(int filefd, int sockfd, int *sys_call_count) {
  // According to: man 2 splice
  // Splice requires one of the fd must refer to a pipe, so we can't splice a
  // file directly into a socket.
  // So the actual sequence of operations is to splice the file into a pipe,
  // then to connect the pipe to the socket with a second splice() call.
  // filefd -> piped[0] piped[1] -> sockfd
  int pipefd[2];
  off_t offset = 0;
  ssize_t bytes_sent;

  (*sys_call_count)++;
  if (pipe(pipefd) == -1) {
    perror("Failed to create pipe");
    return -1;
  }

  (*sys_call_count)++;
  // Get file size
  struct stat file_stat;
  if (fstat(filefd, &file_stat) == -1) {
    perror("fstat failed");
    close(pipefd[0]);
    close(pipefd[1]);
    return -1;
  }

  ssize_t pipe_len, read_len, write_len;
  pipe_len = file_stat.st_size;

  for (;;) {
    (*sys_call_count)++;
    // Splice file to pipe write piped[1]
    read_len =
        splice(filefd, &offset, pipefd[1], NULL, pipe_len, SPLICE_F_NONBLOCK);
    if (read_len < 0) {
      if (errno == EAGAIN) {
        continue;
      }
      perror("Failed to splice filefd to pipefd");
      close(pipefd[0]);
      close(pipefd[1]);
      return -1;
    }
    if (read_len == 0) {
      break;
    }
    pipe_len -= read_len;

    // Send the file data
    while (read_len > 0) {
      (*sys_call_count)++;
      write_len =
          splice(pipefd[0], NULL, sockfd, NULL, read_len, SPLICE_F_MOVE);
      if (write_len < 0) {
        perror("Failed to splice pipefd to sockfd");
        close(pipefd[0]);
        close(pipefd[1]);
        return -1;
      }
      read_len -= write_len;
    }
  }

  return 0;
}
