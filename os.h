/******************************************************************************
 *
 * os.h - OS dependent headers 
 *
 *****************************************************************************
 * $Id: os.h,v 1.7 2004/10/12 19:55:58 hscholz Exp $
 *****************************************************************************
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301,
 * USA.
 *
 *****************************************************************************/

#ifndef _OS_H_
#define _OS_H_

/* set variables for extra features */
#if defined (__sun__) || defined (__sun)
# ifndef __Solaris__
#  define __Solaris__ 1
# endif
#endif

#if defined (__FreeBSD__) || defined (__OpenBSD__) || defined (__NetBSD__) \
		|| defined (__MicroBSD__) || defined (__APPLE__) || defined (__DragonFly__)
#define _HAVE_BSD
#define _HAVE_CHECKINTERFACE
#elif defined (__HPUX__)
#define _HAVE_CHECKINTERFACE
#elif defined (__Solaris__)
#define _HAVE_CHECKINTERFACE
#endif

#ifdef __HPUX__             /* H P U X */
#define _XOPEN_SOURCE_EXTENDED
#define NO_CURSES_E	1
#include <stdio.h>
#include <sys/param.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stropts.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <signal.h>
#include <curses.h>
#include <sys/stdsyms.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <net/if.h>
#include <sys/mib.h>
#include <arpa/inet.h>
#include <net/if.h>
#elif defined (__FreeBSD__)           /* F R E E B S D */
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <signal.h>
#include <curses.h>
#include <sys/param.h>
#include <sys/sysctl.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <net/if.h>
#include <net/if_media.h>
#include <net/if_mib.h>
#include <arpa/inet.h>
#ifndef __DragonFly__
#include <net/ppp_defs.h>
#endif
#elif defined (__NetBSD__)            /* N E T B S D */
#include <stdio.h>
#include <sys/param.h>
#include <sys/sysctl.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <signal.h>
#include <curses.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <net/if.h>
#include <net/if_dl.h>
#include <net/if_media.h>
#include <net/route.h>
#include <arpa/inet.h>
#include <net/ppp_defs.h>
#include <net/if_ppp.h>
#include <net/if.h>
#elif defined (__OpenBSD__) || defined (__MicroBSD__)           /* O P E N B S D */
#include <stdio.h>
#include <sys/param.h>
#include <sys/sysctl.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <signal.h>
#include <curses.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <net/if.h>
#include <net/if_media.h>
#include <net/if_dl.h>
#include <net/route.h>
#include <arpa/inet.h>
#include <net/ppp_defs.h>
#include <net/if_ppp.h>
#elif defined (__APPLE__)			/* Mac OS X */
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <signal.h>
#include <curses.h>
#include <ifaddrs.h>
#include <sys/param.h>
#include <sys/sysctl.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <net/route.h>
#include <net/if_dl.h>
#include <net/if.h>
#include <net/if_media.h>
#include <net/if_mib.h>
#include <arpa/inet.h>
#elif defined (__linux__)             /* L I N U X */
#include <stdio.h>
#include <sys/param.h>
#include <linux/sysctl.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <signal.h>
#include <curses.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <linux/ppp_defs.h>
#include <linux/if_ppp.h>
#elif defined (__Solaris__)           /* S O L A R I S */
#include <stdio.h>
#define NO_CURSES_E	1
#define _WIDEC_H
#include <sys/param.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include <time.h>
#include <ctype.h>
#include <signal.h>
#include <curses.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <net/if.h>
#include <stropts.h>
#include <sys/socket.h>
#include <sys/sockio.h>
#include <net/if.h>
#include <kstat.h>
#include <net/if.h>
#include <sys/sockio.h>
#include <arpa/inet.h>
#include <net/if.h>
#elif defined (__FreeBSD_kernel__)           /* G N U / k F r e e B S D */
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <signal.h>
#include <curses.h>
#include <sys/param.h>
#include <sys/sysctl.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <net/if.h>
#include <net/if_media.h>
#include <net/if_mib.h>
#include <arpa/inet.h>
#else
#error	"Your OS is not supported! Please report to hscholz@raisdorf.net"
#endif                      /* OS selection */

#endif
