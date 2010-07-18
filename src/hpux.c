#include <netio.h>
#define WAIT_PCKS_COUNTER   15

/* $Id: hpux.c,v 1.7 2003/02/23 17:26:02 hscholz Exp $ */

/*****************************************************************************
 *
 * _countinterfaces()
 *
 * count all network interfaces in the system. This function is intended to 
 * use it only in hpux.c
 *
 ****************************************************************************/

int _countinterfaces(void)
{
    int     val, num_iface=-1, sd;
    if ((sd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
        return (-1);

    if (ioctl(sd, SIOCGIFNUM, &val) != -1)
        num_iface = val;
    close(sd);

    return num_iface;
}

/*****************************************************************************
 *
 * _getifdata()
 *
 * get the Interface-ID, the Interface-Speed, and over all, check if the
 * given interface really exists. This function is intended to use it only in
 * hpux.c
 *
 ****************************************************************************/

void _getifdata()
{
    int buffer, fd, val, ret = -1;
    unsigned int len, i;
    char tmpinterfacestring[sizeof(ifdata.if_name)+1],*strstrmatch;
    struct nmparms params;
    mib_ifEntry * if_buf;

    
    /*
     * The interface description is more then the pure devicename.
     * Let's do some formating to allow a propper pattern matching
     */
    strcpy(tmpinterfacestring,ifdata.if_name);
    strcat(tmpinterfacestring," ");

    for (i=0; i <= ifdata.if_amount; i++)
    {
        if ((fd = open_mib("/dev/lan", O_RDWR, i, 0)) >= 0)
        {
            if ((if_buf = (mib_ifEntry *) malloc (sizeof(mib_ifEntry))) != 0) {
                params.objid  = ID_ifEntry;
                params.buffer = if_buf;
                len = sizeof(mib_ifEntry);
                params.len    = &len;
                if_buf->ifIndex = i+1;
                if ((ret = get_mib_info(fd, &params)) == 0) {
                    /*
                     * The interface given by the user must start at the
                     * beginning of if_buf->ifDescr. If that's the case,
                     * strstrmatch is equal to if_buf->ifDescr. If not,
                     * strstrmatch might be a subset of if_buf->ifDescr,
                     * or NULL
                     */
                    strstrmatch = strstr(if_buf->ifDescr, (char *)tmpinterfacestring);
                    if ( strstrmatch && (strcmp(strstrmatch,if_buf->ifDescr)== 0))
                    {
                        ifdata.if_valid = 1;
                        ifdata.if_id = i+1;
                        break;
                    }
                }
            }
        }
        free(if_buf);
        close_mib(fd);
    }
    return;
}

/*****************************************************************************
 *
 * checkinterface()
 *
 * check if a given interface exists, return 1 if it does and 0 if not (This
 * function is a wrapper function for _countinterfaces && _getifdata.)
 *
 ****************************************************************************/
int checkinterface(void)
{
    /* ==  0 no network interfaces, -1 sth. went wrong */
    if ((ifdata.if_amount =_countinterfaces()) > 0)
        _getifdata();
    return ifdata.if_valid;
}

/******************************************************************************
 *
 * get_stat()
 *
 * stub function for all unsupported operating systems
 *
 *****************************************************************************/

int get_stat(void)
{
    int             i,fd, ret=-1;
    static int wait_pcks_counter=WAIT_PCKS_COUNTER+1;
    unsigned int    len;
    unsigned long   rx_o, tx_o;
    struct          nmparms params, params2;
    mib_ifEntry     *if_buf;

    static nmapi_logstat *if_ptr = (nmapi_logstat *) 0;
    if (ifdata.if_valid == 1 && (fd = open_mib("/dev/lan", O_RDWR, 0, 0)) >= 0)
    {
        if ((if_buf = (mib_ifEntry *) malloc (sizeof(mib_ifEntry))) != 0)
        {
            if_buf->ifIndex = ifdata.if_id;
            params.objid  = ID_ifEntry;
            params.buffer = if_buf;
            len = (unsigned int) sizeof(mib_ifEntry);
            params.len    = &len;
            if ((ret = get_mib_info(fd, &params)) == 0)
            {
                rx_o = stats.rx_bytes; tx_o = stats.tx_bytes;

                stats.tx_bytes = if_buf->ifOutOctets;
                stats.rx_bytes = if_buf->ifInOctets;
                stats.tx_errors = if_buf->ifOutErrors;
                stats.rx_errors = if_buf->ifInErrors;

                if (rx_o > stats.rx_bytes)
                    stats.rx_over++;
                if (tx_o > stats.tx_bytes)
                    stats.tx_over++;
            }
        }
        free(if_buf);

        /*
         * Getting the tx/rx packets every run often hurts to much performance
         * With WAIT_PCKS_COUNTER=15 i save on my system 43% cpu usage.instead of
         * WAIT_PCKS_COUNTER=0
         */
        if( wait_pcks_counter > WAIT_PCKS_COUNTER )
        {
            if ((if_ptr = (nmapi_logstat *) malloc(sizeof(nmapi_logstat) * ifdata.if_amount)) != 0 )
            {
                len = (unsigned int) ifdata.if_amount *sizeof(nmapi_logstat);
                if ((ret = get_logical_stat(if_ptr, &len)) == 0)
                {
                    for (i=0; i <= ifdata.if_amount; i++)
                    {
                        if(if_ptr[i].ifindex == ifdata.if_id)
                        {
                            stats.tx_packets = if_ptr[i].out_packets;
                            stats.rx_packets = if_ptr[i].in_packets;
                        }
                    }
                }
            }
            free(if_ptr);
            wait_pcks_counter=0;
        }
        else
        {
            wait_pcks_counter++;
        }
    }
    close_mib(fd);

    return(0);
}
