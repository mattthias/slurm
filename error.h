/* $Id: error.h,v 1.1 2003/05/25 14:45:23 hscholz Exp $ */
#ifndef _ERROR_H
#define _ERROR_H

/* error type definition
 *
 * lower Byte identifies the error
 * upper Byte is reserved for future use
 *       (i.e. & 0x01ff to enable syslog logging)
 */

#define ERR_DEBUG	0x0001
#define ERR_NOTICE	0x0003
#define ERR_WARNING 0x0004
#define ERR_ERROR	0x0005
#define ERR_FATAL	0x0006
#define ERRBUF BUFSIZ

#endif
