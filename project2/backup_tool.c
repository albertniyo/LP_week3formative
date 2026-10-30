#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <errno.h>
#include <time.h>

#define BUF_SIZE   4096
#define BACKUP_DIR "backup"
#define LOG_FILE   "backup/backup.log"
#define DEST_FILE  "backup/destination.txt"

/* one timestamped line to the log */
static void log_entry(const char *msg) {
    int fd = open(LOG_FILE, O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (fd < 0)
        return;

    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    char buf[256];
    int n = snprintf(buf, sizeof(buf),
                     "[%04d-%02d-%02d %02d:%02d:%02d] %s\n",
                     tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday,
                     tm->tm_hour, tm->tm_min, tm->tm_sec, msg);
    if (n > 0)
        write(fd, buf, (size_t)n);
    close(fd);
}

/* cp source file into DEST_FILE, Returns 0 on success */
static int do_backup(const char *src) {
    int in = open(src, O_RDONLY);
    if (in < 0) {
        perror("open source");
        return -1;
    }

    int out = open(DEST_FILE, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (out < 0) {
        perror("open dest");
        close(in);
        return -1;
    }

    char buf[BUF_SIZE];
    ssize_t nread;

    while ((nread = read(in, buf, sizeof(buf))) > 0) {
        ssize_t off = 0;
        while (off < nread) {
            ssize_t nwrite = write(out, buf + off, (size_t)(nread - off));
            if (nwrite < 0) {
                perror("write");
                close(in);
                close(out);
                return -1;
            }
            off += nwrite;
        }
    }

    if (nread < 0) {
        perror("read");
        close(in);
        close(out);
        return -1;
    }

    close(in);
    close(out);
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "usage: %s <source>\n", argv[0]);
        return 1;
    }

    /* 1) make backup dir */
    if (mkdir(BACKUP_DIR, 0755) < 0 && errno != EEXIST) {
        perror("mkdir");
        return 1;
    }
    log_entry("backup started");

    /* 2) child does the copy */
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        return 1;
    }

    if (pid == 0) {
        int rc = do_backup(argv[1]);
        log_entry(rc == 0 ? "child: complete" : "child: failed");
        _exit(rc == 0 ? 0 : 1);
    }

    /* 3) parent waits */
    int status;
    if (waitpid(pid, &status, 0) < 0) {
        perror("waitpid");
        return 1;
    }

    int ok = WIFEXITED(status) && WEXITSTATUS(status) == 0;
    printf("backup %s\n", ok ? "succeeded" : "failed");
    log_entry(ok ? "parent: success" : "parent: failure");

    return ok ? 0 : 1;
}
