#include "../kernel/types.h"
#include "../kernel/stat.h"
#include "../user/user.h"
#include "../kernel/fs.h"
#include "../kernel/param.h"

/*!
 * @brief This function reads an individual line of input argument.
 * NOTE: This function reads arguments from file descriptor 0.
 * @param args is ptray of character strings (resulting argv).
 * @param i is the original number of arguments.
 * @return 1 if new argv added, 0 otherwise.
 */
int get_argv(char** args, int i) {
    const int BUFSIZE = 512;
    int n = 0;
    char buf[BUFSIZE];
    while (1 != read(0, buf, 1)) {  // Read until newline character met.
        if ('\n' == buf[n++]) { break; }
        if (BUFSIZE == n) {
            fprintf(2, "xargs: input arg too long\n");
            exit(1);
        }
    }
    if (0 == n) { return 0; }  // No argument read from fd 0.
    buf[n] = '\0';  // Terminate with nul.
    int p = 0;
    char* ptr = buf;
    while (p < n) {   // Now split the line by spaces.
        while (p < n && ' ' != ptr[p]) { ++p; }
        if (p < n) { break; }
        ptr[p] = '\0';  // Split the string.
        args[i] = malloc(strlen(ptr) + 1);
        strcpy(args[i++], ptr);  // Copy the new argument and increment argc.
        while (p < n && ' ' == ptr[p]) { ++p; }
        ptr = buf + p;  // Point `ptr` to the head of next argument.
    }
    args[i] = 0;  // End of argv.
    return 1;
}

int main(int argc, char* argv[]) {
    if (2 > argc) {
        fprintf(2, "usage: xargs [command [initial-arguments]]\n");
        exit(1);
    }
    char cmd[strlen(argv[1]) + 1];  // The passed-in command.
    strcpy(cmd, argv[1]);
    char* args[MAXARG];  // ptray of pointers.
    for (int i = 1; i < argc; ++i) {  // Save initial arguments.
        args[i - 1] = malloc(strlen(argv[i]) + 1);
        strcpy(args[i - 1], argv[i]);
    }
    while (get_argv(args, argc - 1)) {
        if (0 == fork()) {
            exec(cmd, args);
            fprintf(2, "exec %s failed\n", cmd);
            exit(1);
        } else {
            wait((int*) 0);
        }
    }
    for (int i = 0; i < MAXARG; ++i) {
        if (0 == args[i]) { break; }
        free(args[i]);
    }
    exit(0);
}