# Experiment A — Successful backup

## Setup

```sh
gcc -Wall -Wextra backup_tool.c -o backup_tool 
echo "hello backup payload" > source.txt

rm -rf backup
strace -f -o success_trace.txt ./backup_tool source.txt
```

## Result

Program printed `backup succeeded` (exit 0). Created `backup/backup.log` and `backup/destination.txt` with the source payload.

## Trace evidence (selected)

- Parent (`10719`) creates the directory and starts the log

```text
mkdir("backup", 0755) = 0
openat(AT_FDCWD, "backup/backup.log", O_WRONLY|O_CREAT|O_APPEND, 0644) = 3
write(3, "[2026-10-30 09:49:06] Backup sta"..., 37) = 37
clone(... SIGCHLD ...) = 10720
wait4(10720, ...
```

- and the child (`10720`) copies the file successfully

```text
openat(AT_FDCWD, "source.txt", O_RDONLY) = 3
openat(AT_FDCWD, "backup/destination.txt", O_WRONLY|O_CREAT|O_TRUNC, 0644) = 4
read(3, "hello backup payload\n", 4096) = 21
write(4, "hello backup payload\n", 21) = 21
read(3, "", 4096) = 0
close(3) = 0
close(4) = 0
exit_group(0)
```
- Parent sees a clean child exit and logs success

```text
wait4 resumed>, [{WIFEXITED(s) && WEXITSTATUS(s) == 0}], ...) = 10720
write(1, "Backup succeeded\n", 17) = 17
exit_group(0)
```

For full raw output, check [success_trace.txt](success_trace.txt).
