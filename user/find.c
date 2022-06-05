#include "../kernel/types.h"
#include "../kernel/stat.h"
#include "../user/user.h"
#include "../kernel/fs.h"

void find(char* path, char* pattern) {
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
            fprintf(2, "usage: find [starting directory] [pattern]\n");
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
                    fprintf(2, "find: cannot stat %s\n", path);
                    close(fd);
                    continue;
                }
                switch (st.type) {
                    case T_FILE:
                        if (0 == strcmp(de.name, pattern)) {
                            printf("%s\n", buf);
                        }
                        break;
                    case T_DIR:
                        find(buf, pattern);
                        break;
                }
                break;
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