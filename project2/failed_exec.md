# Experiment B — Failed backup (missing source)

## Setup

```sh
rm -rf backup
strace -f -o failed_trace.txt ./backup_tool missing_source.txt
```

## Result

Child could not open the source. Program printed `backup failed` (exit 1). `backup/` and the log still exist (mkdir plus start log happen before the copy).

## Trace evidence (selected)

- Directory creation still succeeds:

```text
mkdir("backup", 0755) = 0
clone(... SIGCHLD ...) = 10726
wait4(10726, ...
```

- Child open fails with `ENOENT`, this is the root cause in the trace (not only the printed message):

```text
openat(AT_FDCWD, "missing_source.txt", O_RDONLY) = -1 ENOENT (No such file or directory)
write(2/stderr via perror path..., "open source: No such file or dir"..., 39) = 39
exit_group(1)
```

- then parent observes non-zero child status and reports failure

```text
wait4 resumed>, [{WIFEXITED(s) && WEXITSTATUS(s) == 1}], ...) = 10726
write(1, "Backup failed\n", 14) = 14
exit_group(1)
```

No `openat` of `backup/destination.txt` appears in the failure child path as copy never starts after the source open fails.

For full raw output, check [failed_trace.txt](failed_trace.txt).
