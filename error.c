/*****************************************************************************
 *
 * error.c - generic error handling function
 *
 *****************************************************************************
 * $Id: error.c,v 1.1 2003/05/25 14:45:23 hscholz Exp $
 ****************************************************************************/

#ifndef _ERROR_C
#define _ERROR_C

#include "error.h"

/*****************************************************************************
 *
 * error()
 *
 * print error message and exit if asked for
 *
 ****************************************************************************/

void error(int level, char *msg, ...)
{
    va_list arguments;
    int debug = 1;
    va_start(arguments, msg);

    switch (level) {
    case ERR_DEBUG:
        if (debug != 0)
            fprintf(stderr, "DEBUG: ");
        break;
    case ERR_NOTICE:
        fprintf(stderr, "NOTICE: ");
        break;
    case ERR_WARNING:
        fprintf(stderr, "WARNING: ");
        break;
    case ERR_ERROR:
        fprintf(stderr, "ERROR: ");
        break;
    case ERR_FATAL:
        fprintf(stderr, "FATAL: ");
        break;
    }
    /* print the remaining arguments */
    vfprintf(stderr, msg, arguments);
    va_end(arguments);
    fprintf(stderr, "\n");
    if (level == ERR_FATAL)
        exit(1);
}
#endif
