#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

char buf[2] = "";

int main(void) {
    int p[2];
    pipe(p);  // Create a new pipe.
    if (0 == fork()) {
        if (1 != read(p[0], buf, 1)) {
            fprintf(2, "read error\n");  // 2 is stderr.
            exit(1);
        }
        close(p[0]);
        printf("%d: received ping\n", getpid());
        if (1 != write(p[1], buf, 1)) {
            fprintf(2, "write error\n");
            exit(1);
        }
        close(p[1]);
        exit(0);
    } else {
        if (1 != write(p[1], "\x01", 1)) { // Write to the pipe and close the write side.
            fprintf(2, "write error\n");
            exit(1);
        }
        close(p[1]);
        wait((int*) 0);  // Wait for the child process to terminate.
        if (1 != read(p[0], buf, 1)) {
            fprintf(2, "read error\n");
            exit(1);
        }
        close(p[0]);
        printf("%d: received pong\n", getpid());
        exit(0);
    }
}