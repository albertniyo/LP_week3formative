# Project 2 — Linux System Call Forensics and Execution Analysis

`backup_tool.c` which creates `backup/`, writes a log, forks a child that copies a source file to `backup/destination.txt`, parent waits via `waitpid()`.

## Compilation

```sh
gcc -Wall -Wextra backup_tool.c -o backup_tool
```

## Tracing (with child follow)

```sh
# experiment A — success
echo "payload" > source.txt
strace -f -o success_trace.txt ./backup_tool source.txt

# experiment B — failure
strace -f -o failed_trace.txt ./backup_tool missing_source.txt
```

**Note:** `-f` follows the child; `-o` writes the full trace to simply show evidence of captured results.

## Deliverables

| File | Role |
|---|---|
| `backup_tool.c` | Source |
| `success_trace.txt` & `failed_trace.txt` | Raw strace output |
| `success_exec.md` & `failed_exec.md` | experiment notes |
| `technical_analysis.md` | syscall summary table and analysis |
