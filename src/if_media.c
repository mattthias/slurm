/******************************************************************************
 *
 * src/if_media.h - part of slurm
 *
 * this file handles basic network information functions for all
 * operating systems.
 *
 *****************************************************************************
 * $Id: if_media.c,v 1.35 2004/09/27 13:08:59 hscholz Exp $
 *****************************************************************************/

#if defined (__FreeBSD__) || defined (__OpenBSD__) || defined (__NetBSD__) || defined (__MicroBSD__) || defined (__APPLE__)
#define MEDIA_H_SUPPORTED
#endif

#if defined (__OpenBSD__) || defined (__NetBSD__) || defined (__MicroBSD__) || defined (__APPLE__)
#define NON_FreeBSD 1
#define WIRELESS 1
#else
#if defined(__FreeBSD__)
#if (__FreeBSD_version >= 450000)
#define WIRELESS 1
#endif
#else
#define WIRELESS 0
#endif
#endif



/******************************************************************************
 *
 * get_if_speed()
 *
 * determine current interface speed, needs interface name as argument
 * return the interface speed as an integer. unit: kbit/s
 * in case of error return ERR_IFACE_NO_SPEED
 *
 * tested/supported operating systems:
 *
 *  - FreeBSD
 *  - OpenBSD
 *  - NetBSD
 *  - MicroBSD (99% OpenBSD)
 *
 *****************************************************************************/

#ifdef MEDIA_H_SUPPORTED
int get_if_speed(char *ifstring)
{
    int speed = ERR_IFACE_NO_SPEED;
    int s;                      /* socket */
    struct ifmediareq ifmr;
    int *media_list;
    int type, physical;

    if ((s = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        fprintf(stderr, "cannot create socket!\n");
        return ERR_IFACE_NO_SPEED;
    }

    memset(&ifmr, 0, sizeof(ifmr));
    strncpy(ifmr.ifm_name, (char *) ifstring, sizeof(ifmr.ifm_name));

    if (ioctl(s, SIOCGIFMEDIA, (caddr_t) & ifmr) < 0) {
        fprintf(stderr,
                "interface does not support SIOCGIFMEDIA ioctl()\n");
        return ERR_IFACE_NO_SPEED;
    }

    if (ifmr.ifm_count == 0) {
        fprintf(stderr, "%s: no media types?\n", (char *) ifstring);
        return ERR_IFACE_NO_SPEED;
    }

    media_list = (int *) malloc(ifmr.ifm_count * sizeof(int));
    if (media_list == NULL)
        fprintf(stderr, "malloc() error in if_media.c\n");
    ifmr.ifm_ulist = media_list;

    if (ioctl(s, SIOCGIFMEDIA, (caddr_t) & ifmr) < 0) {
        fprintf(stderr, "ioctl(SIOCGIFMEDIA) failed\n");
        return -1;
    }

    /*
     * define type and physical
     *
     * bits:
     *      0-4 Media variant
     *      5-7 Media type
     *
     */

    type = ifmr.ifm_active & 0xf0;
    physical = ifmr.ifm_active & 0x0f;

#ifdef MEDIADEBUG
    printf("      all: %6d\n", ifmr.ifm_current);
    printf("   active: %6d\n", ifmr.ifm_active);
    printf("   status: %6d\n", ifmr.ifm_status);
    printf("     type: %6d\n", type);
    printf("     phys: %6d\n", physical);
    printf("if active: %6d\n", ifmr.ifm_active & IFM_ACTIVE);

#ifdef __APPLE__
    printf("    count: %6d\n", ifmr.ifm_count);
#endif                          /* Apple debugging */
#endif

    /* switch type */
    switch (type) {
        /* Ethernet */
    case IFM_ETHER:
        switch (physical) {
#ifdef __FreeBSD__
#if __FreeBSD__ <= 4
        case IFM_1000_FX:
        case IFM_1000_TX:
#endif
#endif
        case IFM_1000_SX:
        case IFM_1000_LX:
        case IFM_1000_CX:
#ifdef IFM_1000_T
        case IFM_1000_T:
#endif
            speed = 1000 * 1000;
            break;
        case IFM_100_TX:
        case IFM_100_FX:
        case IFM_100_T4:
        case IFM_100_VG:
        case IFM_100_T2:
            speed = 100 * 1000;
            break;
        case IFM_10_T:
        case IFM_10_2:
        case IFM_10_5:
        case IFM_10_FL:
        case IFM_10_STP:
            speed = 10 * 1000;
            break;
#if defined(__OpenBSD__) || defined (__MicroBSD__) || defined (__NetBSD__) || defined (__APPLE__)
        case IFM_HPNA_1:
#else
#if __FreeBSD__ <= 4
        case IFM_homePNA:
#endif
#endif
            speed = 1 * 1000;
            break;
        default:
            speed = ERR_IFACE_NO_SPEED;
            break;
        }                       /* end switch physical */
        break;
        /* FDDI interfaces */
        /* fpa doesn't seem to support SIOCGIFMEDIA on FreeBSD
         * so we won't get here but anyway ...
         */
    case IFM_FDDI:
        switch (physical) {
        case IFM_FDDI_SMF:
        case IFM_FDDI_MMF:
        case IFM_FDDI_UTP:
            speed = 100 * 1000;
            break;
        default:
            speed = ERR_IFACE_NO_SPEED;
        }
        break;
#if WIRELESS
        /* IEEE 802.11 wireless interfaces */
    case IFM_IEEE80211:
        switch (physical) {
        case IFM_IEEE80211_FH1:
        case IFM_IEEE80211_DS1:
            speed = 1 * 1000;
            break;
        case IFM_IEEE80211_FH2:
        case IFM_IEEE80211_DS2:
            speed = 2 * 1000;
            break;
        case IFM_IEEE80211_DS5:
            speed = (int) 5.5 *1000;
            break;
        case IFM_IEEE80211_DS11:
            speed = 11 * 1000;
            break;
#if WIRELESS && defined(__FreeBSD__) && (__FreeBSD_version >= 460102)
        case IFM_IEEE80211_DS22:
            speed = 22 * 1000;
            break;
#endif
#if WIRELESS && \
	(defined(__FreeBSD__) && (__FreeBSD_version >= 500111)) || \
	(defined(__NetBSD__) && (__NetBSD_Version_ > 106020000))
        case IFM_IEEE80211_OFDM6:
            speed = 6 * 1000;
            break;
        case IFM_IEEE80211_OFDM9:
            speed = 9 * 1000;
            break;
        case IFM_IEEE80211_OFDM12:
            speed = 12 * 1000;
            break;
        case IFM_IEEE80211_OFDM18:
            speed = 18 * 1000;
            break;
        case IFM_IEEE80211_OFDM24:
            speed = 24 * 1000;
            break;
        case IFM_IEEE80211_OFDM36:
            speed = 36 * 1000;
            break;
        case IFM_IEEE80211_OFDM48:
            speed = 48 * 1000;
            break;
        case IFM_IEEE80211_OFDM54:
            speed = 54 * 1000;
            break;
        case IFM_IEEE80211_OFDM72:
            speed = 72 * 1000;
            break;
#endif
        default:
            speed = ERR_IFACE_NO_SPEED;
            break;
        }
        break;
#endif
    default:
        speed = ERR_IFACE_NO_SPEED;
    }                           /* end switch type */

#ifdef MEDIADEBUG
    printf("    speed: %6d\n", speed);
#endif
    return speed;
}
#elif defined (__HPUX__)
int get_if_speed(char *ifstring)
{
    int speed = ERR_IFACE_NO_SPEED, buffer, fd, val, ret = -1;
    unsigned int len, i;
    struct nmparms params;
    mib_ifEntry *if_buf;

    for (i = 0; i <= ifdata.if_amount; i++) {
        if ((fd = open_mib("/dev/lan", O_RDWR, i, 0)) >= 0) {
            if ((if_buf =
                 (mib_ifEntry *) malloc(sizeof(mib_ifEntry))) != 0) {
                params.objid = ID_ifEntry;
                params.buffer = if_buf;
                len = sizeof(mib_ifEntry);
                params.len = &len;
                if_buf->ifIndex = i + 1;
                if ((ret = get_mib_info(fd, &params)) == 0) {
                    if (i + 1 == ifdata.if_id)
                        if (if_buf->ifOper == 1)
                            speed = if_buf->ifSpeed / 1000;
                        else
                            speed ERR_IFACE_DOWN;
                }
            }
        }
        free(if_buf);
        close_mib(fd);
    }
    return speed;
}
#elif defined (__Solaris__)
/******************************************************************************
 *
 * Solaris interface speed detection
 *
 *****************************************************************************/
int get_if_speed(char *ifstring)
{
    int speed = ERR_IFACE_NO_SPEED;
    kstat_t *ksp;
    kstat_named_t *knp;
    kstat_ctl_t *kc;

    if ((kc = kstat_open()) == NULL)
        return ERR_IFACE_NO_SPEED;

    ksp = kstat_lookup(kc, NULL, -1, ifstring);
    if (ksp && kstat_read(kc, ksp, NULL) >= 0) {
        knp = (kstat_named_t *) kstat_data_lookup(ksp, "ifspeed");
        if (knp)
            speed = (int) knp->value.ui64 / 1000;
    }
    kstat_close(kc);

    return speed;
}
#else
int get_if_speed(UNUSED_ARG(char *ifstring))
{
    return ERR_IFACE_NO_SPEED;
}
#endif
