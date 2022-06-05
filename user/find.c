#include "../kernel/types.h"
#include "../kernel/stat.h"
#include "../user/user.h"
#include "../kernel/fs.h"

void checker(const char* path, const char* pattern) {
    const char* p;
    for (p = path + strlen(path); p >= path && *p != '/'; --p);
    ++p;
    return 0 == strcmp(p, pattern);
}

void find(const char* path, const char* pattern) {
    char buf[512];
    char* p;
    int fd;
    struct dirent de;
    struct stat st;

    if (0 > (fd = open(path, 0))) {
        fprintf(2, "find: cannot open %s\n", path);
        return;
    }

    if (0 > fstat(fd, &st)) {
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        return;
    }

    switch (st.type) {
        case T_FILE:
            if (checker(path, pattern)) {
                printf("%s\n", path);
            }
            break;
        case T_DIR:
            if (sizeof(buf) < strlen(path) + 1 + DIRSIZ + 1) {
                printf("find: path too long\n");
                break;
            }
            strcpy(buf, path);  // Save the path.
            p = buf + strlen(buf);   // Move `p` to the end of path.
            *p++ = '/';  // Attach a slash to it.
            while (sizeof(de) == read(fd, &de, sizeof(de))) {  // Dirent read successfully.
                if (0 == de.inum || 0 == strcmp(de.name, ".") || 0 == strcmp(de.name, "..")) {
                    continue;
                }
                memmove(p, de.name, DIRSIZ);
                p[DIRSIZ] = 0;
                if (0 > stat(buf, &st)) {
                    fprintf(2, "find: cannot stat %s\n", buf);
                    continue;
                }
                find(buf, pattern);
            }
    }
    close(fd);
}

int main(int argc, char* argv[]) {
    if (3 != argc) {
        fprintf(2, "usage: find [starting directory] [pattern]\n");
        exit(1);
    }
    find(argv[1], argv[2]);
    exit(0);
}
