/* poor man's ulog */
#ifndef ULOG_H___
#define ULOG_H___

#include <stdio.h>

#define ULOG_DECLARE_TAG(tag)

#define ULOGE(args...) do { fprintf(stderr, "E: " args); fprintf(stderr, "\n"); } while (0)
#define ULOGI(args...) do { fprintf(stderr, "I: " args); fprintf(stderr, "\n"); } while (0)
#define ULOGW(args...) do { fprintf(stderr, "W: " args); fprintf(stderr, "\n"); } while (0)

#endif /* ULOG_H___ */
