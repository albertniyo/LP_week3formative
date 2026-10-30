# Technical Explanation — Signal-Driven Server Controller

## Registration and handling

`server_controller` installs four handlers with `sigaction()` for `SIGUSR1`, `SIGUSR2`, `SIGINT`, and `SIGTERM`. Handlers are intentionally tiny: each only writes a `volatile sig_atomic_t` flag. No `printf`, heap use, or other non-async-signal-safe work runs inside a handler.

## Flag-driven main loop

The infinite loop sleeps ~5 seconds, then checks flags in priority order (emergency → graceful → toggle → status) before printing the periodic monitoring line. When a signal interrupts `sleep()`, the loop wakes, processes pending requests, and continues. This keeps I/O and state changes in normal context where libc output is safe.

## Operational state

State is a `sig_atomic_t` holding `NORMAL` or `MAINTENANCE`. `SIGUSR2` sets a toggle flag; the main loop flips the mode and prints the new state. Periodic messages reflect the current mode (`Service running normally...` vs `Service in MAINTENANCE mode...`). `SIGUSR1` requests a status line with state and PID.

## SIGINT vs SIGTERM

| Signal | Behaviour |
|---|---|
| `SIGINT` | Graceful path: print shutdown + cleanup messages, then leave the loop with `return 0`. |
| `SIGTERM` | Emergency path: print termination message and `_exit(1)` immediately (no cleanup sequence). |

## Demonstration evidence

Captured with `demo/run_demo.sh` (logs under `demo/`):

- **SIGUSR1:** `STATUS: state=NORMAL pid=11358` (and later `MAINTENANCE`).
- **SIGUSR2:** `Mode switched to MAINTENANCE` then back to `NORMAL`.
- **SIGINT:** `Graceful shutdown (SIGINT) — cleaning up...` / `Cleanup done. Exiting.`
- **SIGTERM:** `EMERGENCY TERMINATION (SIGTERM)` in `demo/demo_term.log`.

## Compilation

```sh
gcc -Wall -Wextra -o server_controller server_controller.c
```
