#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <time.h>
#include <errno.h>

/* append one timestamped line to the log file */
static int log_msg(const char *msg) {
    FILE *f = fopen("data_sync_v2.log", "a");
    if (!f)
        return -1;

    time_t t = time(NULL);
    struct tm *tm = localtime(&t);

    fprintf(f, "[%04d-%02d-%02d %02d:%02d:%02d] %s\n",
            tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday,
            tm->tm_hour, tm->tm_min, tm->tm_sec, msg);

    fclose(f);
    return 0;
}

/* scan dir, print size via stat(), and log start/end */
int main(int argc, char *argv[]) {
    const char *dir = (argc > 1) ? argv[1] : ".";
    DIR *d = opendir(dir);

    if (!d) {
        perror("opendir");
        return 1;
    }

    printf("scanning directory %s\n", dir);
    log_msg("scan started");

    struct dirent *entry;
    int n = 0;

    while ((entry = readdir(d)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 ||
            strcmp(entry->d_name, "..") == 0)
            continue;

        char path[1024];
        snprintf(path, sizeof(path), "%s/%s", dir, entry->d_name);

        struct stat st;
        if (stat(path, &st) == 0)
            printf("found %s (%ld bytes)\n", entry->d_name, (long)st.st_size);
        else
            printf("found %s (stat failed: %s)\n",
                   entry->d_name, strerror(errno));
        n++;
    }

    closedir(d);
    printf("total files %d\n", n);
    log_msg("scan complete");
    
    return 0;
}
