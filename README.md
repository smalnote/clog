# Demo metrics Linux zero-copy API

## Envrionment

- CPU: E5-2680v4
- Mem: 48GB
- OS: AlmaLinux 9
- Host: KVM

## Send 1GB file to Unix Socket Domain

| API                         | Time  | Diff  |
| --------------------------- | ----- | ----- |
| read/send with buffer (4KB) | 907ms | 100%  |
| read/send with buffer(8KB)  | 598ms | 65.9% |
| read/send with buffer(16KB) | 423ms | 46.6% |
| read/send with buffer(32KB) | 354ms | 39.0% |
| mmap/send                   | 265ms | 29.3% |
| sendfile                    | 131ms | 14.4% |
| splice/pipe                 | 158ms | 17.4% |

> [!NOTE]
> API splice/pipe use a pipe to connect filefd and sockfd, according to `man 2 spclie`,
> the splice function requires one of file descriptor to be pipe, result in:
> splice(filefd, pipefd[1]) and splice(pipefd[1], sockfd).
