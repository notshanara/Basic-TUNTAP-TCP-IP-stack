#include "syshead.h"
#include "utils.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>


int run_cmd(char *cmd, ...) {
    va_list ap;

    char buf[CMDBUFLEN];
    va_start(ap, cmd);
    vsprintf(buf, CMDBUFLEN, cmd, ap);

    va_end(ap);
     printf("%s\n", buf);

    return system(buf);

}

void print_hexdump(char *str, int len) {

    printf("Producing hexdump:\n");
    for (int i=0; i<len; i++) {
        if (i % 8==0) printf("\n");
        printf("%02x ", (unsigned char)str[i]);
    }
      printf("\n");

}

void print_error(char *str, ...) {
    va_list ap;
    char buf[200];
    va_start(ap, str);
    vsnprintf(buf, 200 , str, ap);

    va_end(ap);

    perror(buf);
}
