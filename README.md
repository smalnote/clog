# Demo metrics Linux zero-copy API

## Environment

- CPU: E5-2680v4
- Mem: 48GB
- OS: AlmaLinux 9
- Host: KVM

## Send 1GB file to Unix Socket Domain

| API                  | Time  | Diff  | CPU(user/sys/total/%/maxresident) |
| -------------------- | ----- | ----- | --------------------------------- |
| read/send (buf 4KB)  | 907ms | 100%  | 0.17/0.68/0.86s/99%CPU/1440k      |
| read/send (buf 32KB) | 331ms | 34.1% | 0.01/0.31/0.40s/80%CPU/1440k      |
| mmap/send            | 265ms | 29.3% | 0.00/0.26/0.44s/60%CPU/1049904k   |
| sendfile             | 131ms | 14.4% | 0.00/0.13/0.50s/26%CPU/1440k      |
| splice/pipe          | 158ms | 17.4% | 0.01/0.13/0.50s/30%CPU/1440k      |

> [!NOTE]
> API splice/pipe use a pipe to connect filefd and sockfd, according to `man 2 splice`,
> the splice function requires one of file descriptor to be pipe, result in:
> splice(filefd, pipefd[1]) and splice(pipefd[1], sockfd).
