/* $Id: openbsd.c,v 1.7 2003/02/23 17:26:02 hscholz Exp $ */

/*****************************************************************************
 *
 * checkinterface()
 *
 * check if a given interface exists, return TRUE if it does and FALSE if not
 *
 ****************************************************************************/
int checkinterface()
{
    int validinterface = FALSE;
    static int mib_name[] = { CTL_NET, PF_ROUTE, 0, 0, NET_RT_IFLIST, 0 };
    static char *buf;
    static int alloc;
    char *lim, *next;
    struct if_msghdr *ifm, *nextifm;
    struct sockaddr_dl *sdl;
    size_t needed;
    char s[32];

    if (sysctl(mib_name, 6, NULL, &needed, NULL, 0) < 0)
        return FALSE;
    if (alloc < (signed long) needed) {
        if (buf != NULL)
            free(buf);
        buf = malloc(needed);
        if (buf == NULL)
            return FALSE;
        alloc = needed;
    }

    if (sysctl(mib_name, 6, buf, &needed, NULL, 0) < 0)
        return FALSE;

    lim = buf + needed;
    next = buf;
    while ((next < lim) && (validinterface == 0)) {
        ifm = (struct if_msghdr *) next;
        if (ifm->ifm_type != RTM_IFINFO)
            return FALSE;
        next += ifm->ifm_msglen;

        while (next < lim) {
            nextifm = (struct if_msghdr *) next;
            if (nextifm->ifm_type != RTM_NEWADDR)
                break;
            next += nextifm->ifm_msglen;
        }

        if (ifm->ifm_flags & IFF_UP) {
            sdl = (struct sockaddr_dl *) (ifm + 1);
            strncpy(s, sdl->sdl_data, sdl->sdl_nlen);
            s[sdl->sdl_nlen] = '\0';
            /* search for the right network interface */
            if (sdl->sdl_family != AF_LINK)
                continue;
            if (strcmp(s, ifdata.if_name) != 0)
                continue;
            else {
                validinterface = TRUE;
                break;          /* stop searching */
            }
        }
    }
    return validinterface;
}

/*****************************************************************************
 *
 * get_stat()
 *
 * this code is based on gkrellm code (thanks guys!)
 *
 ****************************************************************************/

int get_stat(void)
{
    static int mib_name[] = { CTL_NET, PF_ROUTE, 0, 0, NET_RT_IFLIST, 0 };
    static char *buf;
    static int alloc;
    char *lim, *next;
    struct if_msghdr *ifm, *nextifm;
    struct sockaddr_dl *sdl;
    char s[32];
    size_t needed;
    unsigned long rx_o, tx_o;

    if (sysctl(mib_name, 6, NULL, &needed, NULL, 0) < 0)
        return 1;
    if (alloc < (signed long) needed) {
        if (buf != NULL)
            free(buf);
        buf = malloc(needed);
        if (buf == NULL)
            return 1;
        alloc = needed;
    }

    if (sysctl(mib_name, 6, buf, &needed, NULL, 0) < 0)
        return 1;
    lim = buf + needed;
    next = buf;
    while (next < lim) {
        ifm = (struct if_msghdr *) next;
        if (ifm->ifm_type != RTM_IFINFO)
            return 1;
        next += ifm->ifm_msglen;

        while (next < lim) {
            nextifm = (struct if_msghdr *) next;
            if (nextifm->ifm_type != RTM_NEWADDR)
                break;
            next += nextifm->ifm_msglen;
        }

        if (ifm->ifm_flags & IFF_UP) {
            sdl = (struct sockaddr_dl *) (ifm + 1);
            /* search for the right network interface */
            if (sdl->sdl_family != AF_LINK)
                continue;
            if (strncmp(sdl->sdl_data, ifdata.if_name, sdl->sdl_nlen) != 0)
                continue;
            strncpy(s, sdl->sdl_data, sdl->sdl_nlen);
            s[sdl->sdl_nlen] = '\0';

            rx_o = stats.rx_bytes;
            tx_o = stats.tx_bytes;
            /* write stats */
            stats.tx_packets = ifm->ifm_data.ifi_opackets;
            stats.rx_packets = ifm->ifm_data.ifi_ipackets;
            stats.rx_bytes = ifm->ifm_data.ifi_ibytes;
            stats.tx_bytes = ifm->ifm_data.ifi_obytes;
            stats.rx_errors = ifm->ifm_data.ifi_ierrors;
            stats.tx_errors = ifm->ifm_data.ifi_oerrors;

            if (rx_o > stats.rx_bytes)
                stats.rx_over++;
            if (tx_o > stats.tx_bytes)
                stats.tx_over++;
        }
    }
    return 0;
}
