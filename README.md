# Demo metrics Linux zero-copy API

## Envrionment

CPU: E5-2680v4
Mem: 48GB
OS: AlmaLinux 9
Host: KVM

## Send 1GB file to Unix Socket Domain

| API         | Clocks | Seconds  |
| ----------- | ------ | -------- |
| read/send   | 907329 | 0.907329 |
| mmap/send   | 265564 | 0.265564 |
| sendfile    | 131007 | 0.131007 |
| splice/pipe | 158149 | 0.158149 |

> [!NOTE]
> API splice/pipe use a pipe to connect filefd and sockfd, according to `man 2 spclie`,
> the splice function requires one of file descriptor to be pipe, result in:
> splice(filefd, pipefd[1]) and splice(pipefd[1], sockfd).
