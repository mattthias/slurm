/* $Id: linux.c,v 1.12 2003/09/06 10:44:50 hscholz Exp $ */

/*****************************************************************************
 *
 * checkinterface()
 *
 * check if a given interface exists and is up.
 * return TRUE if found, FALSE if not
 *
 ****************************************************************************/

int checkinterface(void)
{
    int interfacefound = FALSE;
    unsigned int i;
    struct if_nameindex *ifs;

    if ((ifs = if_nameindex()) == NULL)
        return FALSE;

    for (i = 0; ifs[i].if_index; i++) {
        if (strcmp(ifs[i].if_name, ifdata.if_name) == 0) {
            interfacefound = TRUE;
            break;
        }
    }

    return interfacefound;
}

/******************************************************************************
 *
 * get_stat()
 *
 * read the network statistics from /proc/net/dev (PATH_NET_DEV)
 * if the file is not open open it. fseek() to the beginning and parse
 * each line until we've found the right interface
 *
 * returns 0 if successful, 1 in case of error
 *
 *****************************************************************************/

int get_stat(void)
{
    static int opened = 0;
    char buffer[BUFSIZE];
    char *ptr;
    char *devname;
    unsigned long dump;
    int interfacefound;
    unsigned long rx_o, tx_o;

    if (opened != 1) {
        if ((proc_net_dev = fopen(PATH_NET_DEV, "r")) == NULL) {
            fprintf(stderr, "cannot open %s!\nnot running Linux?\n",
                    PATH_NET_DEV);
            exit(1);
        }
        opened++;
    }

    /* backup old rx/tx values */
    rx_o = stats.rx_bytes;
    tx_o = stats.tx_bytes;

    /* do not parse the first two lines as they only contain static garbage */
    fseek(proc_net_dev, 0, SEEK_SET);
    fgets(buffer, BUFSIZE - 1, proc_net_dev);
    fgets(buffer, BUFSIZE - 1, proc_net_dev);

    interfacefound = 0;
    while (fgets(buffer, BUFSIZE - 1, proc_net_dev) != NULL) {
        /* find the device name and substitute ':' with '\0' */
        ptr = buffer;
        while (*ptr == ' ')
            ptr++;
        devname = ptr;
        while (*ptr != ':')
            ptr++;
        *ptr = '\0';
        ptr++;
        if (!strcmp(devname, (char *) ifdata.if_name)) {
            /* read stats and fill struct */
            sscanf(ptr,
                   "%lg %lu %lu %lu %lu %lu %lu %lu %lg %lu %lu %lu %lu %lu %lu %lu",
                   &stats.rx_bytes, &stats.rx_packets, &stats.rx_errors,
                   &dump, &dump, &dump, &dump, &dump, &stats.tx_bytes,
                   &stats.tx_packets, &stats.tx_errors, &dump, &dump,
                   &dump, &dump, &dump);
            interfacefound = 1;
            continue;           /* break, as we won't get any new information */
        }
    }
    if (interfacefound) {
        if (rx_o > stats.rx_bytes)
            stats.rx_over++;
        if (tx_o > stats.tx_bytes)
            stats.tx_over++;
    }
    return (interfacefound == 1) ? 0 : 1;
}


/*****************************************************************************
 *
 *  get_default_interface()
 *
 *  If only one non local interface is up use that as default
 *
 ****************************************************************************/
int get_default_interface(IfData * ifdata)
{


    struct if_nameindex *ifs;
    ifs = if_nameindex();

    struct ifreq ifr;
    unsigned int i;
    unsigned int index;
    unsigned int iface_up = 0;
    int ret;

    /* Create a socket to ioctl on */
    int sk = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

    /* get an array of if_nameindex structs (one for each iface) */
    ifs = if_nameindex();

    /* iterate over the array and .. */
    for (i = 0; ifs[i].if_index; i++) {

        /* ..write the name of the iface we want to check into the ifr struct and .. */
        strncpy(ifr.ifr_name, ifs[i].if_name, IFNAMSIZ);

        /* .. perform an ioctl SIOCGIFFLAGS to get the iface flags */
        ret = ioctl(sk, SIOCGIFFLAGS, &ifr);
        if (ret < 0)
            return ret;

        if (ifr.ifr_flags & IFF_LOOPBACK) {     /* skip local loopback */
            continue;
        } else if (ifr.ifr_flags & IFF_UP) {    /* check if the iface is up (IFF_UP is set) */
            iface_up++;
            index = i;          /* save the index to get the iface name later */
        }
    }

    if (iface_up == 1) {
        strncpy(ifdata->if_name, ifs[index].if_name,
                sizeof(ifdata->if_name));
        ret = 0;
    } else {
        ret = -1;
    }

    return ret;
}
