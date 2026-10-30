# Project 4 — Signal-Driven Linux Service Controller
Our task is to implement a C program that responds to these signals while continuing to perform periodic monitoring.

## Build

```sh
gcc -Wall -Wextra server_controller.c -o server_controller
```

## Manual test

```sh
./server_controller          # note the PID
# other terminal:
kill -SIGUSR1 <pid>          # status report
kill -SIGUSR2 <pid>          # toggle NORMAL <-> MAINTENANCE
kill -SIGINT  <pid>          # graceful shutdown
kill -SIGTERM <pid>          # emergency exit
```

## Deliverables

- `server_controller.c`
- `demo/terminal_demo.png` — terminal evidence
- `technical_report.md` - technical analysis
