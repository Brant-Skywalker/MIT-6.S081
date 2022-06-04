#include "../kernel/types.h"
#include "../kernel/stat.h"
#include "../user/user.h"

void pass(int p_r) {
    int p[2];
    if (-1 == pipe(p)) {
        fprintf(2, "Failed to create a pipe.\n");
        exit(1);
    }
    int div;  // The divisor of the current process.
    if (0 == read(p_r, &div, sizeof(int))) {
        close(p[0]);
        close(p[1]);
        exit(0);  // All file descriptors referring to the write end are closed. No need to recurse.
    }
    printf("prime %d\n", div);  // Print the divisor here!
    if (0 == fork()) {  // Child process: read numbers from pipe and recurse.
        pass(p[0]);  // Simply pass the read end - the recursive call will do the rest.
    } else {  // Current process: sieve composite numbers out and pass prime numbers.
        int buf;
        while (0 != read(p[0], &buf, sizeof(int))) {
            if (0 != buf % div) {
                write(p[1], &buf, sizeof(int));
            }
        }
    }
    close(p[0]);
    close(p[1]);
    exit(0);
}

int main(void) {
    int p[2];
    int status;
    if (-1 == pipe(p)) {
        fprintf(2, "Failed to create a pipe.\n");
        exit(1);
    }
    if (0 == fork()) {  // Child process.
        close(p[1]);  // We don't write here.
        pass(p[0]);  // Pass the pipe.
        close(p[0]);
        exit(0);
    } else {
        close(p[0]);  // We don't read here.
        int i = 1;
        while (++i < 36) {
            write(p[1], &i, sizeof(int));
        }
        close(p[1]);
        wait(&status);
        exit(status);
    }
}