# Demo metrics Linux zero-copy API

## Environment

- CPU: E5-2680v4
- Mem: 48GB
- OS: AlmaLinux 9
- Host: KVM

## Send 1GB file to Unix Socket Domain

| API                          | Time  | Diff  | Time Perf                                                       |
| ---------------------------- | ----- | ----- | --------------------------------------------------------------- |
| read/send with buffer (4KB)  | 907ms | 100%  | 0.17user 0.68system 0:00.86elapsed 99%CPU (1440maxresident)k    |
| read/send with buffer (32KB) | 331ms | 34.1% | 0.01user 0.31system 0:00.40elapsed 80%CPU (1440maxresident)k    |
| mmap/send                    | 265ms | 29.3% | 0.00user 0.26system 0:00.44elapsed 60%CPU (1049904maxresident)k |
| sendfile                     | 131ms | 14.4% | 0.00user 0.13system 0:00.50elapsed 26%CPU (1440maxresident)k    |
| splice/pipe                  | 158ms | 17.4% | 0.01user 0.13system 0:00.50elapsed 30%CPU (1440maxresident)k    |

> [!NOTE]
> API splice/pipe use a pipe to connect filefd and sockfd, according to `man 2 spclie`,
> the splice function requires one of file descriptor to be pipe, result in:
> splice(filefd, pipefd[1]) and splice(pipefd[1], sockfd).
