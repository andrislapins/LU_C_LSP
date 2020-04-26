#include "common.h"

char *bin2hex(const unsigned char *input, size_t len) {
    char *result;
    char *hexits = "0123456789ABCDEF";

    if (input == NULL || len <= 0) {
        return NULL;
    }

    // (2 hexits+space)/chr + NULL
    int resultlength = (len*3)+1;

    result = malloc(resultlength);
    bzero(result, resultlength);

    for (int i = 0; i < len; i++) {
        result[i*3] =       hexits[input[i] >> 4];
        result[(i*3)+1] =   hexits[input[i] & 0x0F];
        result[(i*3)+2] = ' '; // for readability.
    }

    return result;
}


void str_overwrite_stdout() {
    printf("\r%s", "> ");
    fflush(stdout);
}

void str_trim_lf(char *arr, int length) {
    for (int i = 0; i < length; i++) {
        if (arr[i] == '\n') {
            arr[i] = '\0';
            break;
        }
    }
}

void err_n_die(const char *fmt, ...)
{
    int errno_save;
    va_list ap;

    // Any system or library call can set errno, so we need to save it now.
    errno_save = errno;

    // Print out the fmt+args to standard out.
    va_start(ap, fmt);
    vfprintf(stdout, fmt, ap);
    fprintf(stdout, "\n");
    fflush(stdout);

    // Print out error message is errno was set.
    if (errno_save != 0) {
        fprintf(stdout, "(errno = %d): %s\n", errno_save, strerror(errno_save));
        fprintf(stdout, "\n");
        fflush(stdout);
    }

    va_end(ap);

    // This is the ..and_die part. Terminate with an error.
    exit(1);
}