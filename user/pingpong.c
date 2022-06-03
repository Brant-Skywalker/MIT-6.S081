#include "../kernel/types.h"
#include "../kernel/stat.h"
#include "../user/user.h"

char buf[1];

int main(void) {
    int p[2];
    pipe(p);  // Create a new pipe.
    if (0 == fork()) {
        read(p[0], buf, 1);
        close(p[0]);
        fprintf(0, "%d: received ping\n", getpid());
        write(p[1], buf, 1);
        close(p[1]);
        exit(0);
    } else {
        write(p[1], "a", 1);  // Write to the pipe and close the write side.
        close(p[1]);
        wait((int*) 0);  // Wait for the child process to terminate.
        read(p[0], buf, 1);
        close(p[0]);
        fprintf(0, "%d: received pong\n", getpid());
        exit(0);
    }
}