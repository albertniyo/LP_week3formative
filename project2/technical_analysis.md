# System Call Technical Analysis

## Summary table of observed calls

| System Call | Purpose | Relevant Arguments | Return value or error | Role in the program |
|---|---|---|---|---|
| `mkdir` | Create backup directory | `"backup", 0755` | `0` (success) | Ensures destination directory exists before copy/log |
| `openat` | Open/create files | `"source.txt" O_RDONLY`; `"backup/destination.txt" O_CREAT\|O_TRUNC`; log with `O_APPEND` | fd ≥ 0, or `-1 ENOENT` on missing source | Source read, dest write, logging |
| `read` | Read file bytes | fd of source, 4096-byte buffer | `21` then `0` (EOF) on success | Copies source content in the child |
| `write` | Write file/stdout bytes | dest fd plus payload; log fd plus timestamp line; stdout status | byte count written | Persist backup plus log plus user message |
| `close` | Release descriptors | source/dest/log fds | `0` | Avoid descriptor leaks after I/O |
| `clone` | Create child process | `flags=...\|SIGCHLD` (glibc `fork()`) | child PID | Child performs the file copy |
| `wait4` | Wait for child | child PID | PID; status `WEXITSTATUS` 0 or 1 | Parent synchronizes and checks result |
| `exit_group` | Terminate process | `0` success / `1` failure | does not return | Ends child and parent with correct status |
| `access` | Loader preload check | `"/etc/ld.so.preload"` | `-1 ENOENT` | Runtime/loader init — not application logic |

## File interaction with the kernel

Application path starts with `mkdir` > `openat` log >`write`/`close` >child `openat` source plus dest >`read`/`write` loop >`close`. Timestamps trigger incidental `openat`/`read` of `/etc/localtime` inside `localtime()` as library behaviour, not backup logic. Dynamic linker `openat` of `libc.so.6` is process startup.

## Process creation and synchronization

`fork()` appears as `clone(... SIGCHLD ...)`. Parent immediately `wait4`s as child either completes the copy and `exit_group(0)`, or fails open and `exit_group(1)` then parent resumes `wait4`, inspects `WEXITSTATUS`, logs, prints, then `exit_group` with the same outcome.

## Success vs failure

| Aspect | Success | Failure |
|---|---|---|
| Source `openat` | `= 3` | `= -1 ENOENT` |
| Dest create / `read`/`write` copy | Present | Absent |
| Child exit | `exit_group(0)` | `exit_group(1)` |
| Parent `wait4` status | exit status 0 | exit status 1 |
| User-visible result | `Backup succeeded` | `Backup failed` |

Trace proves the failure cause is missing source (`ENOENT`), and that the parent correctly propagates the child’s non-zero exit.

## Unexpected / unnecessary calls

- Loader `access("/etc/ld.so.preload")` and libc mapping are normal CRT startup.
- `localtime()` timezone file reads, kinda side effect of timestamp formatting.

## One reliability improvement

Validating the source with `access`/`stat` (or open it) **before** `fork`, so the parent can fail fast without spawning the child that only reports `ENOENT` and create the destination directory under a unique name and `fsync` before reporting success.
