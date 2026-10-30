#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

/* where normal = 0, maintainance = 1 */
static volatile sig_atomic_t state = 0;

/* only set flags (async-signal-safe) */
static volatile sig_atomic_t want_status = 0;
static volatile sig_atomic_t want_toggle = 0;
static volatile sig_atomic_t want_graceful = 0;
static volatile sig_atomic_t want_emergency = 0;

static void on_usr1(int sig) { (void)sig; want_status = 1; }
static void on_usr2(int sig) { (void)sig; want_toggle = 1; }
static void on_int(int sig)  { (void)sig; want_graceful = 1; }
static void on_term(int sig) { (void)sig; want_emergency = 1; }

static void set_handler(int signo, void (*fn)(int)) {
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = fn;
    sigemptyset(&sa.sa_mask);
    if (sigaction(signo, &sa, NULL) < 0) {
        perror("sigaction");
        exit(1);
    }
}

int main() {
    set_handler(SIGUSR1, on_usr1);
    set_handler(SIGUSR2, on_usr2);
    set_handler(SIGINT,  on_int);
    set_handler(SIGTERM, on_term);

    printf("[Server Controller] PID %d — monitoring started\n", (int)getpid());
    fflush(stdout);

    for (;;) {
        /* Handle flags in the main loop (safe to printf here). */
        if (want_emergency) {
            puts("[Server Controller] EMERGENCY TERMINATION (SIGTERM)");
            fflush(stdout);
            _exit(1);
        }

        if (want_graceful) {
            puts("[Server Controller] graceful shutdown (SIGINT) — cleaning up...");
            puts("[Server Controller] cleanup done. Exiting");
            fflush(stdout);
            break;
        }

        if (want_toggle) {
            want_toggle = 0;
            state = state ? 0 : 1;
            printf("[Server Controller] Mode switched to %s\n",
                   state ? "MAINTENANCE" : "NORMAL");
            fflush(stdout);
        }

        if (want_status) {
            want_status = 0;
            printf("[Server Controller] STATUS: state=%s pid=%d\n",
                   state ? "MAINTENANCE" : "NORMAL", (int)getpid());
            fflush(stdout);
        }

        if (state == 0)
            puts("[Server Controller] Service running normally...");
        else
            puts("[Server Controller] Service in MAINTENANCE mode...");
        fflush(stdout);

        sleep(5);
    }

    return 0;
}
