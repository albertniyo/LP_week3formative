#include <stdio.h>
#include <string.h>
#include <dirent.h>

/* scan dir and list file names*/
int main(int argc, char *argv[]) {
    const char *dir = (argc > 1) ? argv[1] : ".";
    DIR *d = opendir(dir);

    if (!d) {
        perror("opendir");
        return 1;
    }

    printf("scanning directory %s\n", dir);

    struct dirent *entry;
    int n = 0;

    while ((entry = readdir(d)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 ||
            strcmp(entry->d_name, "..") == 0)
            continue;

        printf("found: %s\n", entry->d_name);
        n++;
    }

    closedir(d);
    printf("total files %d\n", n);

    return 0;
}
