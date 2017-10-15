/* $Id: freebsd.c,v 1.8 2003/02/23 17:26:02 hscholz Exp $ */

/*****************************************************************************
 *
 * checkinterface()
 *
 * check if a given interface exists and is up.
 * return TRUE if it does and FALSE if not
 *
 ****************************************************************************/

int checkinterface(void)
{
    int validinterface = FALSE;

    int i, num_iface;
    size_t len;
    int name[6];
    struct ifmibdata ifmd;

    len = sizeof(num_iface);
    sysctlbyname("net.link.generic.system.ifcount", &num_iface, &len, NULL,
                 0);
    for (i = 1; i <= num_iface; i++) {
        name[0] = CTL_NET;
        name[1] = PF_LINK;
        name[2] = NETLINK_GENERIC;
        name[3] = IFMIB_IFDATA;
        name[4] = i;
        name[5] = IFDATA_GENERAL;

        len = sizeof(ifmd);
        sysctl(name, 6, &ifmd, &len, NULL, 0);
        if (strcmp(ifmd.ifmd_name, (char *) ifdata.if_name) == 0) {
            /*
             * now we have an interface and just have to see if it's up
             * in case we just want to debug media types we disable
             * IFF_UP flags
             */
#ifndef MEDIADEBUG
            if (ifmd.ifmd_flags & IFF_UP)
#endif
                validinterface = TRUE;
            break;              /* in any case we can stop searching here */
        }
    }
    return validinterface;
}

/******************************************************************************
 *
 * get_stat()
 *
 * use sysctl() to read the statistics and fill statistics struct
 *
 ****************************************************************************/

int get_stat(void)
{
    /*
     * use sysctl() to get the right interface number if !dev_opened
     * then read the data directly from the ifmd_data struct
     */

    static int watchif = -1;
    int i, num_iface;
    size_t len;
    int name[6];
    struct ifmibdata ifmd;
    static int dev_opened = 0;
    unsigned long rx_o, tx_o;

    if (!dev_opened) {
        len = sizeof(num_iface);
        sysctlbyname("net.link.generic.system.ifcount", &num_iface, &len,
                     NULL, 0);
        for (i = 1; i <= num_iface; i++) {
            name[0] = CTL_NET;
            name[1] = PF_LINK;
            name[2] = NETLINK_GENERIC;
            name[3] = IFMIB_IFDATA;
            name[4] = i;
            name[5] = IFDATA_GENERAL;

            len = sizeof(ifmd);
            sysctl(name, 6, &ifmd, &len, NULL, 0);
            if (strcmp(ifmd.ifmd_name, (char *) ifdata.if_name) == 0) {
                /* got the right interface */
                watchif = i;
                dev_opened++;
            }
        }
    }
    /* in any case read the struct and record statistics */
    name[0] = CTL_NET;
    name[1] = PF_LINK;
    name[2] = NETLINK_GENERIC;
    name[3] = IFMIB_IFDATA;
    name[4] = watchif;
    name[5] = IFDATA_GENERAL;

    len = sizeof(ifmd);
    sysctl(name, 6, &ifmd, &len, NULL, 0);

    rx_o = stats.rx_bytes;
    tx_o = stats.tx_bytes;

    stats.tx_packets = ifmd.ifmd_data.ifi_opackets;
    stats.rx_packets = ifmd.ifmd_data.ifi_ipackets;
    stats.rx_bytes = ifmd.ifmd_data.ifi_ibytes;
    stats.tx_bytes = ifmd.ifmd_data.ifi_obytes;
    stats.rx_errors = ifmd.ifmd_data.ifi_ierrors;
    stats.tx_errors = ifmd.ifmd_data.ifi_oerrors;

    if (rx_o > stats.rx_bytes)
        stats.rx_over++;
    if (tx_o > stats.tx_bytes)
        stats.tx_over++;

    return (0);
}
