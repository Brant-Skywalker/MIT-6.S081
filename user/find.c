#include "../kernel/types.h"
#include "../kernel/stat.h"
#include "../user/user.h"
#include "../kernel/fs.h"

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
            break;
        case T_DIR:
            if (sizeof(buf) < strlen(path) + 1 + DIRSIZ + 1) {
                printf("find: path too long\n");
                break;
            }
            strcpy(buf, path);  // Save the path.
            p = buf + strlen(buf);   // Move `p` to the end of path.
            *p++ = '/';  // Attach a slash to it.
//void find_helper(char const *path, char const *target)
//{
//    char buf[512], *p;
//    int fd;
//    struct dirent de;
//    struct stat st;
//    if((fd = open(path, 0)) < 0){
//        fprintf(2, "find: cannot open %s\n", path);
//        exit(1);
//    }
//
//    if(fstat(fd, &st) < 0){
//        fprintf(2, "find: cannot stat %s\n", path);
//        exit(1);
//    }
//
//    switch(st.type){
//        case T_FILE:
//            fprintf(2, "Usage: find dir file\n");
//            exit(1);
//        case T_DIR:
//            if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
//                printf("find: path too long\n");
//                break;
//            }
//            strcpy(buf, path);
//            p = buf + strlen(buf);
//            *p++ = '/';
            while(read(fd, &de, sizeof(de)) == sizeof(de)){
                if (de.inum == 0 || strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
                    continue;
                memmove(p, de.name, DIRSIZ);
                p[DIRSIZ] = 0;
                if(stat(buf, &st) < 0){
                    printf("find: cannot stat %s\n", buf);
                    continue;
                }
                if(st.type == T_DIR){
                    find(buf, pattern);
                }else if (st.type == T_FILE){
                    if (strcmp(de.name, pattern) == 0)
                    {
                        printf("%s\n", buf);
                    }
                }
            }
            break;
    }
    close(fd);
}

//void find(const char* path, const char* pattern) {
//    char buf[512];
//    char* p;
//    int fd;
//    struct dirent de;
//    struct stat st;
//
//    if (0 > (fd = open(path, 0))) {
//        fprintf(2, "find: cannot open %s\n", path);
//        return;
//    }
//
//    if (0 > fstat(fd, &st)) {
//        fprintf(2, "find: cannot stat %s\n", path);
//        close(fd);
//        return;
//    }
//
//    switch (st.type) {
//        case T_FILE:
//            break;
//        case T_DIR:
//            if (sizeof(buf) < strlen(path) + 1 + DIRSIZ + 1) {
//                printf("find: path too long\n");
//                break;
//            }
//            strcpy(buf, path);  // Save the path.
//            p = buf + strlen(buf);   // Move `p` to the end of path.
//            *p++ = '/';  // Attach a slash to it.
//            while (sizeof(de) == read(fd, &de, sizeof(de))) {  // Check all files in the current directory.
//                if (0 == de.inum || 0 == strcmp(de.name, ".") || 0 == strcmp(de.name, "..")) {
//                    continue;
//                }
//                memmove(p, de.name, DIRSIZ);  // Concatenate the path.
//                p[DIRSIZ] = 0;
//                if (0 > stat(buf, &st)) {
//                    fprintf(2, "find: cannot stat %s\n", buf);
//                    continue;
//                }
//                switch (st.type) {
//                    case T_FILE:
//                        if (0 == strcmp(de.name, pattern)) {
//                            printf("%s\n", path);
//                        }
//                        break;
//                    case T_DIR:
//                        find(buf, pattern);  // Recurse on directory!
//                        break;
//                }
//            }
//            break;
//    }
//    close(fd);
//}

int main(int argc, char* argv[]) {
    if (3 != argc) {
        fprintf(2, "usage: find [path] [pattern]\n");
        exit(1);
    }
    find(argv[1], argv[2]);
    exit(0);
}
