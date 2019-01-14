/******************************************************************************
 *
 * slurm - yet another network load monitor
 *
 * slurm started as a FreeBSD port of the Linux ppp link monitor called
 * pppstatus by Gabriel Montenegro.
 * I ripped of the ppp dependend parts and the email checks to turn in into
 * a generic network load monitor running on all common Unices.
 *
 *  author: Hendrik Scholz, Matthias Schmitz
 * website: https://www.github.com/mattthias/slurm
 *
 * As pppstatus is licensed under the GPL, slurm is too. See the following
 * paragraphs for information about pppstatus and the license.
 *
 *****************************************************************************
 * $Id: slurm.c,v 1.86 2004/10/12 19:44:36 hscholz Exp $
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

#include "os.h"

#include "config.h"
#include "slurm.h"
#include "src/if_media.h"
#include "src/if_media.c"

#include "error.h"
#include "error.c"
#include "theme.h"
#include "theme.c"

#ifdef __HPUX__
#include "src/hpux.h"
#include "src/hpux.c"
#elif defined (__FreeBSD__) || defined (__FreeBSD_kernel__)
#include "src/freebsd.h"
#include "src/freebsd.c"
#elif defined (__linux__)
#include "src/linux.h"
#include "src/linux.c"
#elif defined (__OpenBSD__) || defined (__MicroBSD__)
#include "src/openbsd.h"
#include "src/openbsd.c"
#elif defined (__NetBSD__)
#include "src/netbsd.h"
#include "src/netbsd.c"
#elif defined (__Solaris__)
#include "src/solaris.h"
#include "src/solaris.c"
#elif defined (__APPLE__)
#include "src/macos.h"
#include "src/macos.c"
#elif defined (__DragonFly__)
#include "src/freebsd.h"
#include "src/freebsd.c"
#else
#warning "Your OS is not supported. Including dummy code!"
#include "src/unsupported.h"
#include "src/unsupported.c"
#endif

/******************************************************************************
 *
 * center()
 *
 * center a given string on a given line
 * this is pppstatus code :)
 *
 *****************************************************************************/

void center(short int line, char *strg, ...)
{
    short int where;
    char *final;
    va_list trans;

    final = malloc(80);
    va_start(trans, strg);
    (void) vsnprintf(final, 80, strg, trans);
    va_end(trans);
    where = (int) (40 - (strlen(final) / 2));
    if (line != 0 && line != 24) {
        move(line, 20);
        hline(32, 50);
    }
    mvprintw(line, where, "%s", final);
    free(final);
}

/******************************************************************************
 *
 * get_time()
 *
 * uptime stats struct with current time
 *
 *****************************************************************************/

void get_time(void)
{
    long x;
    stats.current_time = time(NULL);
    x = stats.current_time - stats.connect_time;
    stats.online_sec = x % 60;
    stats.online_min = (x / 60) % 60;
    stats.online_hour = (x / 3600) % 24;
    stats.online_days = (x / (3600 * 24)) % 365;
}

/******************************************************************************
 *
 * update_info()
 *
 * update the statistics and pay attention to buffer overruns while we are
 * here. As we allow decreasing values we have to pay attention to
 * no leave garbage on the screen.
 *
 *****************************************************************************/

void update_info(int displaymode)
{
    char draw[DRAWLEN + 1];
    attrset(COLOR_PAIR(PAIR_VAR) |
            ((unsigned long) (t.var_attrib ==
                              COL_BOLD) ? A_BOLD : A_NORMAL));
    if (displaymode != MODE_LARGE) {
        snprintf(draw, DRAWLEN - 1, "%lu",
                 stats.rx_packets - stats.rx_packets_off);
        strncat(draw, "               ", DRAWLEN - strlen(draw));
        mvprintw(20, 24, "%s", draw);

        snprintf(draw, DRAWLEN - 1, "%lu",
                 stats.tx_packets - stats.tx_packets_off);
        strncat(draw, "               ", DRAWLEN - strlen(draw));
        mvprintw(20, 65, "%s", draw);

        snprintf(draw, DRAWLEN - 1, "%lu",
                 stats.rx_errors - stats.rx_errors_off);
        strncat(draw, "               ", DRAWLEN - strlen(draw));
        mvprintw(22, 24, "%s", draw);

        snprintf(draw, DRAWLEN - 1, "%lu",
                 stats.tx_errors - stats.tx_errors_off);
        strncat(draw, "               ", DRAWLEN - strlen(draw));
        mvprintw(22, 65, "%s", draw);

        switch (data_type) {
        case TYPE_MEGA:
            snprintf(draw, DRAWLEN - 1, "%.3f MB", (stats.rx_bytes +
                                                    (stats.rx_over *
                                                     SNMPMAXBYTES) -
                                                    stats.rx_bytes_off) /
                     1048570);
            strncat(draw, "               ", DRAWLEN - strlen(draw));
            mvprintw(21, 24, "%s", draw);

            snprintf(draw, DRAWLEN - 1, "%.3f MB", (stats.tx_bytes +
                                                    (stats.tx_over *
                                                     SNMPMAXBYTES) -
                                                    stats.tx_bytes_off) /
                     1048570);
            strncat(draw, "               ", DRAWLEN - strlen(draw));
            mvprintw(21, 65, "%s", draw);
            break;
        case TYPE_GIGA:
            snprintf(draw, DRAWLEN - 1, "%.3f GB", (stats.rx_bytes +
                                                    (stats.rx_over *
                                                     SNMPMAXBYTES) -
                                                    stats.rx_bytes_off)
                     / 1073735680);
            strncat(draw, "               ", DRAWLEN - strlen(draw));
            mvprintw(21, 24, "%s", draw);

            snprintf(draw, DRAWLEN - 1, "%.3f GB", (stats.tx_bytes +
                                                    (stats.tx_over *
                                                     SNMPMAXBYTES) -
                                                    stats.tx_bytes_off)
                     / 1073735680);
            strncat(draw, "               ", DRAWLEN - strlen(draw));
            mvprintw(21, 65, "%s", draw);
            break;
        }
    }
}

#ifdef WITH_LED
/******************************************************************************
 *
 * led_on() / led_off()
 *
 * blinkenlights(tm)
 *
 *****************************************************************************/

void led_on(unsigned int who)
{
    int CHAR = 215;

    if (who == LED_RX) {
        attrset(COLOR_PAIR(PAIR_RX) |
                ((unsigned long) (t.rx_attrib ==
                                  COL_BOLD) ? A_BOLD : A_NORMAL));
        move(14, 37);
        hline(CHAR, 2);
    } else if (who == LED_TX) {
        attrset(COLOR_PAIR(PAIR_TX) |
                ((unsigned long) (t.tx_attrib ==
                                  COL_BOLD) ? A_BOLD : A_NORMAL));
        move(14, 41);
        hline(CHAR, 2);
    }
}

void led_off(unsigned int who)
{
    if (who == LED_RX) {
        attrset(COLOR_PAIR(PAIR_VAR) |
                ((unsigned long) (t.var_attrib ==
                                  COL_BOLD) ? A_BOLD : A_NORMAL));
        mvprintw(14, 37, "RX");
    } else if (who == LED_TX) {
        attrset(COLOR_PAIR(PAIR_VAR) |
                ((unsigned long) (t.var_attrib ==
                                  COL_BOLD) ? A_BOLD : A_NORMAL));
        mvprintw(14, 41, "TX");
    }
}
#endif

/******************************************************************************
 *
 * update_stat_large()
 *
 * update the data array holding the needed information and redraw the graph
 * this mode shows two large graphs, one for each direction
 *
 *****************************************************************************/

int update_stat_large(void)
{
    float sumspeed;
    float rxspeed;
    float txspeed;
    int i = 0, x, y;
    int rx_scalechanged = 0;    /* traffic scale changed? */
    int tx_scalechanged = 0;

    float tmp_maxspeed = 0;     /* needed for max speed calc in graph */
    int tmp_maxspeedpos = 1;    /* same here */

    char draw[DRAWLEN + 1];     /* what we draw on the screen */

    /* in case of REINIT we want to keep the rx and tx values as we can
     * recalculate the graph for the new mode based on them.
     * recalculation is done automatically as ??_maxspeed equals 0 after
     * initilization.
     */

    if (db_status == DB_STATUS_REINIT || db_status == DB_STATUS_STARTUP) {
        rx_maxspeed = 0;
        rx_overallmax = 0;
        tx_maxspeed = 0;
        tx_overallmax = 0;
        /* fill the graph array with zeros */
        for (x = 0; x < GRAPH_WIDTH; x++) {
            for (y = 0; y < GRAPH_HEIGHT; y++) {
                rx_graph[x][y] = 0;
                tx_graph[x][y] = 0;
            }
        }

        if (db_status == DB_STATUS_STARTUP) {
            stats.rx_bytes_comp = stats.rx_bytes;
            stats.tx_bytes_comp = stats.tx_bytes;
            for (x = 0; x < GRAPH_WIDTH; x++) {
                rx_speedarray[x] = 0;
                tx_speedarray[x] = 0;
            }
            db_status = DB_STATUS_RUNNING;

            return 1;
        }
        db_status = DB_STATUS_RUNNING;
    }

    /*
     * current speed in bits/s
     *
     * in case of the expected u_long overflow in stats.?x_bytes
     * we will calculate a negative number as {sum|tx|rx}speed.
     * to prevent this we have to check if the _comp value is larger
     * than the current value. If true, we set _comp to 0
     */

    if (stats.rx_bytes_comp > stats.rx_bytes)
        stats.rx_bytes_comp = 0;
    if (stats.tx_bytes_comp > stats.tx_bytes)
        stats.tx_bytes_comp = 0;

    sumspeed = ((stats.rx_bytes - stats.rx_bytes_comp) +
                (stats.tx_bytes - stats.tx_bytes_comp)) / refreshdelay;

    rxspeed = (stats.rx_bytes - stats.rx_bytes_comp) / refreshdelay;
    txspeed = (stats.tx_bytes - stats.tx_bytes_comp) / refreshdelay;

    /* print current speed */
    snprintf(draw, DRAWLEN - 1, "%.2f KB/s", (float) rxspeed / 1024);
    strncat(draw, "               ", DRAWLEN - strlen(draw));
    mvprintw(21, 24, "%s", draw);

    snprintf(draw, DRAWLEN - 1, "%.2f KB/s", (float) txspeed / 1024);
    strncat(draw, "               ", DRAWLEN - strlen(draw));
    mvprintw(21, 65, "%s", draw);

    stats.rx_bytes_comp = stats.rx_bytes;
    stats.tx_bytes_comp = stats.tx_bytes;

    /* set new max speed for graph if reached */
    if (rxspeed > rx_maxspeed) {
        rx_maxspeed = rxspeed;
        rx_maxspeedpos = 0;
        rx_scalechanged++;
        if (rxspeed > rx_overallmax)
            rx_overallmax = rxspeed;
    }

    /* set new max speed for graph if reached */
    if (txspeed > tx_maxspeed) {
        tx_maxspeed = txspeed;
        tx_maxspeedpos = 0;
        tx_scalechanged++;
        if (txspeed > tx_overallmax)
            tx_overallmax = txspeed;
    }

    /*
     * update the Graph Top Speed field
     * as it might be shorter than before just be sure to not leave
     * trailing garbage by printing spaces
     */

    if (rx_scalechanged) {
        snprintf(draw, DRAWLEN - 1, "%.2f KB/s",
                 (float) rx_maxspeed / 1024);
        strncat(draw, "               ", DRAWLEN - strlen(draw));
        mvprintw(22, 24, "%s", draw);
    }
    if (tx_scalechanged) {
        snprintf(draw, DRAWLEN - 1, "%.2f KB/s",
                 (float) tx_maxspeed / 1024);
        strncat(draw, "               ", DRAWLEN - strlen(draw));
        mvprintw(22, 65, "%s", draw);
    }


    /* increment position of max speed as we move the graph */
    rx_maxspeedpos++;
    tx_maxspeedpos++;

    /* check if max speed has to be lowered for the graph as the max speed
     * was reached too long ago
     */

    tmp_maxspeed = 0;
    tmp_maxspeedpos = 1;
    if (rx_maxspeedpos >= GRAPH_WIDTH) {
        /* max speed calculation has to be redone */
        for (i = 0; i < GRAPH_WIDTH; i++) {
            if (rx_speedarray[i] > tmp_maxspeed) {
                tmp_maxspeed = rx_speedarray[1];
                tmp_maxspeedpos = i;
            }
        }
        /* set new values */
        rx_maxspeed = tmp_maxspeed;
        rx_maxspeedpos = tmp_maxspeedpos;
    }

    tmp_maxspeed = 0;
    tmp_maxspeedpos = 1;
    if (tx_maxspeedpos >= GRAPH_WIDTH) {
        /* max speed calculation has to be redone */
        for (i = 0; i < GRAPH_WIDTH; i++) {
            if (tx_speedarray[i] > tmp_maxspeed) {
                tmp_maxspeed = tx_speedarray[1];
                tmp_maxspeedpos = i;
            }
        }
        /* set new values */
        tx_maxspeed = tmp_maxspeed;
        tx_maxspeedpos = tmp_maxspeedpos;
    }

    /* prepare the graph array
     *
     * shift the graph to the left and then add the last entry
     * in addition move the traffic stats left
     */

    /* move */
    for (x = GRAPH_WIDTH - 1; x >= 0; x--) {
        rx_speedarray[x] = rx_speedarray[x - 1];
        tx_speedarray[x] = tx_speedarray[x - 1];
        for (y = 0; y < GRAPHLARGE_HEIGHT; y++) {
            rx_graph[x][y] = rx_graph[x - 1][y];
            tx_graph[x][y] = tx_graph[x - 1][y];
        }
    }

    /* add the last column */
    if (rx_maxspeed > 0)
        i = (int) ((rxspeed / rx_maxspeed) * GRAPHLARGE_HEIGHT);
    for (y = 0; y <= GRAPHLARGE_HEIGHT; y++) {
        if (i > y)
            rx_graph[0][y] = 1;
        else
            rx_graph[0][y] = 0;
    }
    if (0 != (int) rxspeed)
        rx_graph[0][1] = 1;
    rx_speedarray[0] = rxspeed;

    if (tx_maxspeed > 0)
        i = (int) ((txspeed / tx_maxspeed) * GRAPHLARGE_HEIGHT);
    for (y = 0; y <= GRAPHLARGE_HEIGHT; y++) {
        if (i > y)
            tx_graph[0][y] = 1;
        else
            tx_graph[0][y] = 0;
    }
    if (0 != (int) txspeed)
        tx_graph[0][1] = 1;
    tx_speedarray[0] = txspeed;

    /*
     * rescale graph
     *
     * in case the maxspeed for the shown graph changed we need to
     * recalculate the whole graph
     *
     */

    if (rx_scalechanged) {
        /* for each line rewrite the graph */
        for (x = GRAPH_WIDTH - 1; x >= 0; x--) {
            i = (int) ((rx_speedarray[x] / rx_maxspeed) *
                       GRAPHLARGE_HEIGHT);
            for (y = 0; y <= GRAPHLARGE_HEIGHT; y++) {
                if (i > y)
                    rx_graph[x][y] = 1;
                else
                    rx_graph[x][y] = 0;
            }
            if (rx_speedarray[x] != 0)
                rx_graph[x][1] = 1;
        }
    }
    if (tx_scalechanged) {
        /* for each line rewrite the graph */
        for (x = GRAPH_WIDTH - 1; x >= 0; x--) {
            i = (int) ((tx_speedarray[x] / tx_maxspeed) *
                       GRAPHLARGE_HEIGHT);
            for (y = 0; y <= GRAPHLARGE_HEIGHT; y++) {
                if (i > y)
                    tx_graph[x][y] = 1;
                else
                    tx_graph[x][y] = 0;
            }
            if (tx_speedarray[x] != 0)
                tx_graph[x][1] = 1;
        }
    }


    /*
     * finally draw the two graphs line by line
     *
     * the rx graphs grows to the bottom
     */

    for (y = GRAPHLARGE_HEIGHT - 1; y > 0; y--) {
        for (x = 0; x <= GRAPH_WIDTH; x++) {
            /* RX graph */
            if (rx_graph[x][y] == 1) {
                attrset(COLOR_PAIR(PAIR_RX) |
                        ((unsigned long) (t.rx_attrib ==
                                          COL_BOLD) ? A_BOLD : A_NORMAL));
                mvprintw(11 - y, x + 1, SYMBOL_TRAFFIC);
            } else
                mvprintw(11 - y, x + 1, SYMBOL_NOTRAFFIC);
            /* TX graph */
            if (tx_graph[x][y] == 1) {
                attrset(COLOR_PAIR(PAIR_TX) |
                        ((unsigned long) (t.tx_attrib ==
                                          COL_BOLD) ? A_BOLD : A_NORMAL));
                mvprintw(10 + y, x + 1, SYMBOL_TRAFFIC);
            } else
                mvprintw(10 + y, x + 1, SYMBOL_NOTRAFFIC);
        }
    }
    return 0;
}

/******************************************************************************
 *
 * update_stat_split()
 *
 * updates the array holding information about the graph and redraw
 * the graph.
 *
 *****************************************************************************/

int update_stat_split(void)
{
    float sumspeed;
    float rxspeed;
    float txspeed;
    int i = 0, x, y;
    int rx_scalechanged = 0;    /* traffic scale changed? */
    int tx_scalechanged = 0;

    float tmp_maxspeed = 0;     /* needed for max speed calc in graph */
    int tmp_maxspeedpos = 1;    /* same here */

    char draw[DRAWLEN + 1];     /* what we draw on the screen */

    /* in case of REINIT we want to keep the rx and tx values as we can
     * recalculate the graph for the new mode based on them.
     * recalculation is done automatically as ??_maxspeed equals 0 after
     * initilization.
     */

    if (db_status != DB_STATUS_RUNNING) {
        rx_maxspeed = 0;
        rx_overallmax = 0;
        tx_maxspeed = 0;
        tx_overallmax = 0;
        /* fill the graph array with zeros */
        for (x = 0; x < GRAPH_WIDTH; x++) {
            for (y = 0; y < GRAPH_HEIGHT; y++) {
                rx_graph[x][y] = 0;
                tx_graph[x][y] = 0;
            }
        }

        if (db_status == DB_STATUS_STARTUP) {
            stats.rx_bytes_comp = stats.rx_bytes;
            stats.tx_bytes_comp = stats.tx_bytes;
            for (x = 0; x < GRAPH_WIDTH; x++) {
                rx_speedarray[x] = 0;
                tx_speedarray[x] = 0;
            }
            db_status = DB_STATUS_RUNNING;

            return 1;
        }
        db_status = DB_STATUS_RUNNING;
    }

    /*
     * current speed in bits/s
     *
     * in case of the expected u_long overflow in stats.?x_bytes
     * we will calculate a negative number as {sum|tx|rx}speed.
     * to prevent this we have to check if the _comp value is larger
     * than the current value. If true, we set _comp to to 2^32 - _comp
     */

    if (stats.rx_bytes_comp > stats.rx_bytes)
        stats.rx_bytes_comp = SNMPMAXBYTES - stats.rx_bytes_comp;
    if (stats.tx_bytes_comp > stats.tx_bytes)
        stats.tx_bytes_comp = SNMPMAXBYTES - stats.tx_bytes_comp;

    sumspeed = ((stats.rx_bytes - stats.rx_bytes_comp) +
                (stats.tx_bytes - stats.tx_bytes_comp)) / refreshdelay;

    rxspeed = (stats.rx_bytes - stats.rx_bytes_comp) / refreshdelay;
    txspeed = (stats.tx_bytes - stats.tx_bytes_comp) / refreshdelay;

    /* print current speed
     * instead of directly printing the string we first clear the draw string
     * and then fill in the speed. This way we prevent garbage from being
     * left on the screen.
     */

    attrset(COLOR_PAIR(PAIR_VAR) |
            ((unsigned long) (t.var_attrib ==
                              COL_BOLD) ? A_BOLD : A_NORMAL));
    snprintf(draw, DRAWLEN - 1, "%.2f KB/s    ", rxspeed / 1024);
    strncat(draw, "               ", DRAWLEN - strlen(draw));
    mvprintw(17, 24, "%s", draw);
    snprintf(draw, DRAWLEN - 1, "%.2f KB/s    ", txspeed / 1024);
    strncat(draw, "               ", DRAWLEN - strlen(draw));
    mvprintw(17, 65, "%s", draw);

    stats.rx_bytes_comp = stats.rx_bytes;
    stats.tx_bytes_comp = stats.tx_bytes;

    /* set new max speed for graph if reached */
    if (rxspeed > rx_maxspeed) {
        rx_maxspeed = rxspeed;
        rx_maxspeedpos = 0;
        rx_scalechanged++;
        if (rxspeed > rx_overallmax) {
            rx_overallmax = rxspeed;
            snprintf(draw, DRAWLEN - 1, "%.2f KB/s",
                     (float) rxspeed / 1024);
            strncat(draw, "               ", DRAWLEN - strlen(draw));
            mvprintw(19, 24, "%s", draw);
        }
    }

    /* set new max speed for graph if reached */
    if (txspeed > tx_maxspeed) {
        tx_maxspeed = txspeed;
        tx_maxspeedpos = 0;
        tx_scalechanged++;
        if (txspeed > tx_overallmax) {
            tx_overallmax = txspeed;
            snprintf(draw, DRAWLEN - 1, "%.2f KB/s",
                     (float) txspeed / 1024);
            strncat(draw, "               ", DRAWLEN - strlen(draw));
            mvprintw(19, 65, "%s", draw);
        }
    }

    /*
     * update the Graph Top Speed field
     * as it might be shorter than before just be sure to not leave
     * trailing garbage by printing spaces
     */

    if (rx_scalechanged) {
        snprintf(draw, DRAWLEN - 1, "%.2f KB/s", rx_maxspeed / 1024);
        strncat(draw, "               ", DRAWLEN - strlen(draw));
        mvprintw(18, 24, "%s", draw);
    }
    if (tx_scalechanged) {
        snprintf(draw, DRAWLEN - 1, "%.2f KB/s", tx_maxspeed / 1024);
        strncat(draw, "               ", DRAWLEN - strlen(draw));
        mvprintw(18, 65, "%s", draw);
    }


    /* increment position of max speed as we move the graph */
    rx_maxspeedpos++;
    tx_maxspeedpos++;

    /* check if max speed has to be lowered for the graph as the max speed
     * was reached too long ago
     */

    tmp_maxspeed = 0;
    tmp_maxspeedpos = 1;
    if (rx_maxspeedpos >= GRAPHSPLIT_WIDTH) {
        /* max speed calculation has to be redone */
        for (i = 0; i < GRAPHSPLIT_WIDTH; i++) {
            if (rx_speedarray[i] > tmp_maxspeed) {
                tmp_maxspeed = rx_speedarray[1];
                tmp_maxspeedpos = i;
            }
        }
        /* set new values */
        rx_maxspeed = tmp_maxspeed;
        rx_maxspeedpos = tmp_maxspeedpos;
    }

    tmp_maxspeed = 0;
    tmp_maxspeedpos = 1;
    if (tx_maxspeedpos >= GRAPHSPLIT_WIDTH) {
        /* max speed calculation has to be redone */
        for (i = 0; i < GRAPHSPLIT_WIDTH; i++) {
            if (tx_speedarray[i] > tmp_maxspeed) {
                tmp_maxspeed = tx_speedarray[1];
                tmp_maxspeedpos = i;
            }
        }
        /* set new values */
        tx_maxspeed = tmp_maxspeed;
        tx_maxspeedpos = tmp_maxspeedpos;
    }

    /* prepare the graph array
     *
     * shift the graph to the left and then add the last entry
     * in addition move the traffic stats left
     */

    /* move */
    for (x = GRAPHSPLIT_WIDTH - 1; x >= 0; x--) {
        rx_speedarray[x] = rx_speedarray[x - 1];
        tx_speedarray[x] = tx_speedarray[x - 1];
        for (y = 0; y < GRAPHSPLIT_HEIGHT; y++) {
            rx_graph[x][y] = rx_graph[x - 1][y];
            tx_graph[x][y] = tx_graph[x - 1][y];
        }
    }

    /* add the last column */
    if (rx_maxspeed > 0)
        i = (int) ((rxspeed / rx_maxspeed) * GRAPHSPLIT_HEIGHT);
    for (y = 0; y <= GRAPHSPLIT_HEIGHT; y++) {
        if (i > y)
            rx_graph[0][y] = 1;
        else
            rx_graph[0][y] = 0;
    }
    if (0 != (int) rxspeed)
        rx_graph[0][1] = 1;
    rx_speedarray[0] = rxspeed;

    if (tx_maxspeed > 0)
        i = (int) ((txspeed / tx_maxspeed) * GRAPHSPLIT_HEIGHT);
    for (y = 0; y <= GRAPHSPLIT_HEIGHT; y++) {
        if (i > y)
            tx_graph[0][y] = 1;
        else
            tx_graph[0][y] = 0;
    }
    if (0 != (int) txspeed)
        tx_graph[0][1] = 1;
    tx_speedarray[0] = txspeed;

    /*
     * rescale graph
     *
     * in case the maxspeed for the shown graph changed we need to
     * recalculate the whole graph
     *
     */

    if (rx_scalechanged) {
        /* for each line rewrite the graph */
        for (x = GRAPHSPLIT_WIDTH - 1; x >= 0; x--) {
            i = (int) ((rx_speedarray[x] / rx_maxspeed) *
                       GRAPHSPLIT_HEIGHT);
            for (y = 0; y <= GRAPHSPLIT_HEIGHT; y++) {
                if (i > y)
                    rx_graph[x][y] = 1;
                else
                    rx_graph[x][y] = 0;
            }
            if (rx_speedarray[x] != 0)
                rx_graph[x][1] = 1;
        }
    }
    if (tx_scalechanged) {
        /* for each line rewrite the graph */
        for (x = GRAPHSPLIT_WIDTH - 1; x >= 0; x--) {
            i = (int) ((tx_speedarray[x] / tx_maxspeed) *
                       GRAPHSPLIT_HEIGHT);
            for (y = 0; y <= GRAPHSPLIT_HEIGHT; y++) {
                if (i > y)
                    tx_graph[x][y] = 1;
                else
                    tx_graph[x][y] = 0;
            }
            if (tx_speedarray[x] != 0)
                tx_graph[x][1] = 1;
        }
    }


    /*
     * finally draw the two graphs line by line
     *
     * the rx graphs grows to the bottom
     */

    for (y = GRAPHSPLIT_HEIGHT - 1; y > 0; y--) {
        for (x = 0; x <= GRAPHSPLIT_WIDTH; x++) {
            /* RX graph */
            if (rx_graph[x][y] == 1) {
                attrset(COLOR_PAIR(PAIR_RX) |
                        ((unsigned long) (t.rx_attrib ==
                                          COL_BOLD) ? A_BOLD : A_NORMAL));
                mvprintw(7 - y, x + 1, SYMBOL_TRAFFIC);
            } else
                mvprintw(7 - y, x + 1, SYMBOL_NOTRAFFIC);
            /* TX graph */
            if (tx_graph[x][y] == 1) {
                attrset(COLOR_PAIR(PAIR_TX) |
                        ((unsigned long) (t.tx_attrib ==
                                          COL_BOLD) ? A_BOLD : A_NORMAL));
                mvprintw(6 + y, x + 1, SYMBOL_TRAFFIC);
            } else
                mvprintw(6 + y, x + 1, SYMBOL_NOTRAFFIC);
        }
    }
    return 0;
}

/******************************************************************************
 *
 * update_stat_combined()
 *
 * this is the classic view for the graph known from 0.0.3
 *
 *****************************************************************************/
int update_stat_combined(void)
{
    float sumspeed;
    float rxspeed;
    float txspeed;
    int i = 0, x, y;
    int rx_scalechanged = 0;    /* traffic scale changed? */

    float tmp_maxspeed = 0;     /* needed for max speed calc in graph */
    int tmp_maxspeedpos = 1;    /* same here */

    char draw[DRAWLEN + 1];     /* what we draw on the screen */

    /* in case of REINIT we want to keep the rx and tx values as we can
     * recalculate the graph for the new mode based on them.
     */

    if (db_status == DB_STATUS_REINIT || db_status == DB_STATUS_STARTUP) {

        tx_maxspeed = 0;
        tx_overallmax = 0;
        rx_maxspeed = 0;
        rx_overallmax = 0;
        comb_maxspeed = 0;
        comb_overallmax = 0;
        /* fill the graph array with zeros */
        for (x = 0; x < GRAPH_WIDTH; x++) {
            for (y = 0; y < GRAPH_HEIGHT; y++) {
                rx_graph[x][y] = 0;
                tx_graph[x][y] = 0;
            }
        }

        /* force graph recalculation */
        rx_scalechanged++;

        if (db_status == DB_STATUS_STARTUP) {
            stats.rx_bytes_comp = stats.rx_bytes;
            stats.tx_bytes_comp = stats.tx_bytes;
            for (x = 0; x < GRAPH_WIDTH; x++) {
                rx_speedarray[x] = 0;
                tx_speedarray[x] = 0;
            }
            db_status = DB_STATUS_RUNNING;
            return 1;
        }
        db_status = DB_STATUS_RUNNING;
    }

    /*
     * current speed in bits/s
     *
     * in case of the expected u_long overflow in stats.?x_bytes
     * we will calculate a negative number as {sum|tx|rx}speed.
     * to prevent this we have to check if the _comp value is larger
     * than the current value. If true, we set _comp to 0
     */

    if (stats.rx_bytes_comp > stats.rx_bytes)
        stats.rx_bytes_comp = 0;
    if (stats.tx_bytes_comp > stats.tx_bytes)
        stats.tx_bytes_comp = 0;

    sumspeed = ((stats.rx_bytes - stats.rx_bytes_comp) +
                (stats.tx_bytes - stats.tx_bytes_comp)) / refreshdelay;

    rxspeed = (stats.rx_bytes - stats.rx_bytes_comp) / refreshdelay;
    txspeed = (stats.tx_bytes - stats.tx_bytes_comp) / refreshdelay;

    /* print current speed */
    snprintf(draw, DRAWLEN - 1, "%.2f KB/s", sumspeed / 1024);
    strncat(draw, "               ", DRAWLEN - strlen(draw));
    mvprintw(18, 24, "%s", draw);

    stats.rx_bytes_comp = stats.rx_bytes;
    stats.tx_bytes_comp = stats.tx_bytes;

    /* set new max speed for graph if reached */
    if (sumspeed > comb_maxspeed) {
        comb_maxspeed = sumspeed;
        rx_maxspeedpos = 0;
        rx_scalechanged++;
        if (sumspeed > comb_overallmax) {
            comb_overallmax = sumspeed;
            snprintf(draw, DRAWLEN - 1, "%.2f KB/s",
                     (float) sumspeed / 1024);
            strncat(draw, "               ", DRAWLEN - strlen(draw));
            mvprintw(19, 65, "%s", draw);
        }
    }

    /*
     * update the Graph Top Speed field
     * as it might be shorter than before just be sure to not leave
     * trailing garbage by printing spaces
     */

    if (rx_scalechanged) {
        snprintf(draw, DRAWLEN - 1, "%.2f KB/s", comb_maxspeed / 1024);
        strncat(draw, "               ", DRAWLEN - strlen(draw));
        mvprintw(19, 24, "%s", draw);
    }

    /* increment position of max speed as we move the graph */
    rx_maxspeedpos++;

    /* check if max speed has to be lowered for the graph as the max speed
     * was reached too long ago
     */

    tmp_maxspeed = 0;
    tmp_maxspeedpos = 1;
    if (rx_maxspeedpos >= GRAPHCOMBINED_WIDTH) {
        /* max speed calculation has to be redone */
        for (i = 0; i < GRAPHCOMBINED_WIDTH; i++) {
            if (rx_speedarray[i] > tmp_maxspeed) {
                tmp_maxspeed = rx_speedarray[1];
                tmp_maxspeedpos = i;
            }
        }
        /* set new values */
        comb_maxspeed = tmp_maxspeed;
        rx_maxspeedpos = tmp_maxspeedpos;
    }

    /* prepare the graph array
     *
     * shift the graph to the left and then add the last entry
     * in addition move the traffic stats left
     *
     * there seems to be some redundancy in here regarding the rx/tx values
     * but we need it for seamless mode switching
     *
     */

    /* move */
    for (x = GRAPHCOMBINED_WIDTH - 1; x >= 0; x--) {
        rx_speedarray[x] = rx_speedarray[x - 1];
        tx_speedarray[x] = tx_speedarray[x - 1];
        for (y = 0; y < GRAPHCOMBINED_HEIGHT; y++) {
            rx_graph[x][y] = rx_graph[x - 1][y];
            tx_graph[x][y] = tx_graph[x - 1][y];
        }
    }

    /* add the last column */
    if (comb_maxspeed > 0)
        i = (int) ((sumspeed / comb_maxspeed) * GRAPHCOMBINED_HEIGHT);
    for (y = 0; y <= GRAPHCOMBINED_HEIGHT; y++) {
        if (i > y)
            rx_graph[0][y] = 1;
        else
            rx_graph[0][y] = 0;
    }
    if (0 != (int) sumspeed)
        rx_graph[0][1] = 1;

    rx_speedarray[0] = rxspeed;
    tx_speedarray[0] = txspeed;

    /*
     * rescale graph
     *
     * in case the maxspeed for the shown graph changed we need to
     * recalculate the whole graph
     *
     */

    if (rx_scalechanged) {
        /* for each line rewrite the graph */
        for (x = GRAPHCOMBINED_WIDTH - 1; x >= 0; x--) {
            i = (int) ((rx_speedarray[x] / comb_maxspeed) *
                       GRAPHSPLIT_HEIGHT);
            for (y = 0; y <= GRAPHCOMBINED_HEIGHT; y++) {
                if (i > y)
                    rx_graph[x][y] = 1;
                else
                    rx_graph[x][y] = 0;
            }
            if (rx_speedarray[x] != 0)
                rx_graph[x][1] = 1;
        }
    }

    /*
     * finally draw the two graphs line by line
     *
     * the rx graphs grows to the bottom
     */

    for (y = GRAPHCOMBINED_HEIGHT - 1; y > 0; y--) {
        for (x = 0; x <= GRAPHCOMBINED_WIDTH; x++) {
            /* RX graph */
            if (rx_graph[x][y] == 1) {
                attrset(COLOR_PAIR(PAIR_RX) |
                        ((unsigned long) (t.rx_attrib ==
                                          COL_BOLD) ? A_BOLD : A_NORMAL));
                mvprintw(13 - y, x + 1, SYMBOL_TRAFFIC);
            } else
                mvprintw(13 - y, x + 1, SYMBOL_NOTRAFFIC);
        }
    }
    return 0;
}

/******************************************************************************
 *
 * usage()
 *
 * print usage and die
 *
 *****************************************************************************/

void usage(int code, char **argv)
{
    fprintf(stderr, "slurm %s - https://github.com/mattthias/slurm\n\n"
            "usage: %s [-hHz] [-csl] [-d delay] [-t theme] -i interface\n\n",
            PACKAGE_VERSION, argv[0]);
    fprintf(stderr, "    -h            print help\n"
            "    -z            zero counters at startup\n"
            "    -d delay      delay between refreshs in seconds"
            " (%d < delay < %d)\n"
            "    -c            old classic/combined view\n"
            "    -s            split window mode with stats\n"
            "    -l            large split window mode\n"
            "    -L            enable TX/RX 'leds'\n"
            "    -i interface  select network interface\n"
            "    -t theme      select a theme\n\n",
            REFRESH_MIN, REFRESH_MAX);
    _exit(code);
}

/******************************************************************************
 *
 * draw_face()
 *
 * draw the static parts of the screen
 *
 *****************************************************************************/

void draw_face(int displaymode, char *hostname)
{
    int x, y;

    attrset(COLOR_PAIR(PAIR_TEXT) |
            ((unsigned long) (t.text_attrib ==
                              COL_BOLD) ? A_BOLD : A_NORMAL));
    for (x = 0; x <= 24; x++) {
        for (y = 0; y <= 80; y++) {
            mvprintw(x, y, " ");
        }
    }
    refresh();
    if (displaymode != MODE_LARGE) {
        mvprintw(20, 6, "Received Packets:");
        mvprintw(22, 3, "Errors on Receiving:");
        mvprintw(20, 44, "Transmitted Packets:");
        switch (data_type) {
        case TYPE_MEGA:
            mvprintw(21, 7, "MBytes Received:");
            mvprintw(21, 45, "MBytes Transmitted:");
            break;
        case TYPE_GIGA:
            mvprintw(21, 7, "GBytes Received:");
            mvprintw(21, 45, "GBytes Transmitted:");
            break;
        }
    }

    if (displaymode == MODE_SPLIT) {
        mvprintw(22, 41, "Errors on Transmission:");
#ifdef WITH_LED
        if (ledenabled) {
            mvprintw(14, 37, "RX");
            mvprintw(14, 41, "TX");
        }
#endif
        mvprintw(17, 6, "Current RX Speed:");
        mvprintw(19, 2, "Overall Top RX Speed:");
        mvprintw(18, 4, "Graph Top RX Speed:");
        mvprintw(17, 47, "Current TX Speed:");
        mvprintw(19, 43, "Overall Top TX Speed:");
        mvprintw(18, 45, "Graph Top TX Speed:");
        mvprintw(15, 6, "Active Interface:");
        mvprintw(15, 48, "Interface Speed:");
    } else if (displaymode == MODE_COMBINED) {
        mvprintw(22, 41, "Errors on Transmission:");
#ifdef WITH_LED
        if (ledenabled) {
            mvprintw(14, 37, "RX");
            mvprintw(14, 41, "TX");
        }
#endif

        mvprintw(18, 9, "Current Speed:");
        mvprintw(19, 46, "Overall Top Speed:");
        mvprintw(19, 7, "Graph Top Speed:");
        mvprintw(16, 6, "Active Interface:");
        mvprintw(16, 48, "Interface Speed:");
    } else {
        /* large split mode */
        mvprintw(21, 6, "Current RX Speed:");
        mvprintw(21, 47, "Current TX Speed:");
        mvprintw(22, 4, "Graph Top RX Speed:");
        mvprintw(22, 45, "Graph Top TX Speed:");

    }
    attrset(COLOR_PAIR(PAIR_TITLE) |
            ((unsigned long) (t.title_attrib ==
                              COL_BOLD) ? A_BOLD : A_NORMAL));
    if (hostname[0] != '\0')
        center(00, "-= slurm %s on %s =-", VERSION, hostname);
    else
        center(00, "-= slurm %s =-", VERSION);
    attrset(COLOR_PAIR(PAIR_VAR) |
            ((unsigned long) (t.var_attrib ==
                              COL_BOLD) ? A_BOLD : A_NORMAL));
    if (displaymode == MODE_SPLIT) {
        mvprintw(15, 24, "%s", ifdata.if_name);
        mvprintw(15, 65, "%s", (char *) ifdata.if_speedstring);
    } else if (displaymode == MODE_COMBINED) {
        mvprintw(16, 24, "%s", ifdata.if_name);
        mvprintw(16, 65, "%s", (char *) ifdata.if_speedstring);
    }
    refresh();
}

/*****************************************************************************
 *
 * main()
 *
 * this is where the nightmare starts
 *
 ****************************************************************************/

int main(int argc, char *argv[])
{
    short int key_pressed;
    long now, old[2];
    int sec_value = 0;
    int min_value = 0;
    int reload = FALSE;
    int validinterface = 0;
    int displaymode = MODE_SPLIT;
    int modechange = 0;
    unsigned short int first = 1;
    unsigned short int disconnected = 0;
    char hostname[MAXHOSTNAMELEN + 1];
    extern long refreshdelay;
    extern int ledenabled;
    float tmpdelay;
    int op;
    int z_option = FALSE;
    char *themep = NULL;
    hostname[0] = '\0';

    /* HP-UX related as of now but could become standard */
    ifdata.if_valid = 0;
    ifdata.if_id = 0;
    ifdata.if_amount = 0;

    ledenabled = 0;

    /* determine hostname */
    if (gethostname(hostname, MAXHOSTNAMELEN) != 0)
        hostname[0] = '\0';

    /* handle command line options */
    while ((op = getopt(argc, argv, "Lhzd:clsi:t:")) != EOF) {
        switch (op) {
        case 'i':
            /* interface */
            snprintf((char *) ifdata.if_name,
                     (size_t) sizeof(ifdata.if_name), "%s", optarg);
            /* check if interface really exists */
#ifdef _HAVE_CHECKINTERFACE
            validinterface = checkinterface();
#else
            /* just trust the user for now */
            validinterface++;
#endif
            break;
        case 'h':
            /* print help and exit peacefully */
            usage(0, argv);
            break;
        case 'z':
            /* virtually reset all counters by adding an offset */
            z_option = TRUE;
            break;
        case 'd':
            /* delay between updates */
            tmpdelay = atof(optarg);
            if ((tmpdelay < REFRESH_MIN) || (tmpdelay > REFRESH_MAX)) {
                fprintf(stderr,
                        "delay has to be between %d and %d seconds!\n",
                        REFRESH_MIN, REFRESH_MAX);
                return 1;
            } else
                refreshdelay = tmpdelay;
            break;
        case 'c':
            /* use old classic/combined view */
            displaymode = MODE_COMBINED;
            break;
        case 'l':
            /* use large split window mode */
            displaymode = MODE_LARGE;
            break;
        case 'L':
            /* enable TX/RX led */
            ledenabled = 1;
            break;
        case 's':
            /* use default split window mode */
            displaymode = MODE_SPLIT;
            break;
        case 't':
            /* theme */
            themep = optarg;
            break;
        default:
            usage(1, argv);
            break;
        }
    }

    if (strlen(ifdata.if_name) == 0) {
#ifdef __linux__
        /* If no interface was given as option make an educated guess for a default interface */
        int rv = get_default_interface(&ifdata);
        if (rv == 0) {
            validinterface = checkinterface();
        } else {
            fprintf(stderr,
                    "\nNo network interface given on command line and auto-detect failed.\n"
                    "Please specify a network interface using \"-i <iface>\".\n\n",
                    REFRESH_MIN, REFRESH_MAX);
            usage(1, argv);

        }
#else
        usage(1, argv);
#endif
    }

    if (!validinterface) {
        fprintf(stderr, "specified device does not exist or cannot "
                "be monitored!\n\nIf you think this is an error please report "
                "it to https://github.com/mattthias/slurm/issues . Thanks!\n");
        exit(1);
    }

    /* Initialize some info variables */
    stats.tx_packets = 0;
    stats.tx_bytes = 0;
    stats.tx_errors = 0;
    stats.rx_packets = 0;
    stats.rx_bytes = 0;
    stats.rx_errors = 0;
    stats.tx_bytes_comp = 0;
    stats.tx_bytes_comp = 0;
    /* overruns */
    stats.rx_over = 0;
    stats.tx_over = 0;

    /*
     * run get_stat() once so watchif gets set and we get offsets in case
     * -z cli option was given.
     */
    get_stat();
    if (z_option == TRUE)
        zero_stats();
    else {
        /* set all counters to zero */
        stats.rx_packets_off = stats.rx_errors_off = 0;
        stats.rx_over_off = stats.tx_packets_off = 0;
        stats.tx_errors_off = stats.tx_over_off = 0;
        stats.rx_bytes_off = stats.tx_bytes_off = 0;
        stats.rx_bytes_comp_off = stats.tx_bytes_comp_off = 0;
    }

    /* determine interface speed
     * not all systems have this nice feature we display "unknown"
     * if the speed could not determined due to errors or lack of
     * this feature on the host operating system
     */
#if defined(_HAVE_BSD) || defined(__HPUX__) || defined(__Solaris__) || defined(__APPLE__) || defined(__linux__)
    ifdata.if_speed = get_if_speed(ifdata.if_name);

    /* if ERR_IFACE_NO_SPEED we could not determine the interface speed
     * if ERR_IFACE_DOWN the interface was down
     * as we cannot monitor an interface which is down we exit quit
     */
    if (ifdata.if_speed == ERR_IFACE_DOWN) {
        fprintf(stderr, "interface '%s' is down\n",
                (char *) ifdata.if_name);
        return 1;
    }

    /* ifdata.if_speed is in kbit/s */
    if (ifdata.if_speed >= 1000)
        snprintf((char *) ifdata.if_speedstring,
                 sizeof(ifdata.if_speedstring), "%d Mbit/s",
                 (int) ifdata.if_speed / 1000);
    else if ((ifdata.if_speed == 0)
             || (ifdata.if_speed == ERR_IFACE_NO_SPEED))
        snprintf((char *) ifdata.if_speedstring,
                 sizeof(ifdata.if_speedstring), "unknown");
    else
        snprintf((char *) ifdata.if_speedstring,
                 sizeof(ifdata.if_speedstring), "%d kbit/s",
                 (int) ifdata.if_speed / 100);
#else
    snprintf((char *) ifdata.if_speedstring,
             sizeof(ifdata.if_speedstring), "unknown");
#endif

#if MEDIADEBUG
    return 0;
#endif

#ifndef NO_CURSES_E
    printf("\e(U");
#endif


    if (themep == NULL)
        theme_setdefault(&t);
    else
        theme_readfile(&t, themep);

    if (t.complete != E_ALL)
        error(ERR_FATAL, "broken theme");

    initscr();
    intrflush(stdscr, FALSE);
    start_color();
    curs_set(0);                /* Toggle the cursor off */
    nonl();
    noecho();
    cbreak();
    nodelay(stdscr, TRUE);

#ifdef _HAVE_NCURSES
    if (use_default_colors() == OK)     /* need ncurses for transparency */
        t.background = -1;
#endif

    init_pair(PAIR_TEXT, t.text, t.background);
    init_pair(PAIR_RX, t.rx, t.background);
    init_pair(PAIR_TX, t.tx, t.background);
    init_pair(PAIR_VAR, t.var, t.background);
    init_pair(PAIR_TITLE, t.title, t.background);
    attrset(COLOR_PAIR(PAIR_TEXT) |
            ((unsigned long) (t.text_attrib ==
                              COL_BOLD) ? A_BOLD : A_NORMAL));

    /* curses is set up so make sure to shut it down correctly when
     * receiving termination signals by calling slurm_shutdown()    
     */

    signal(SIGHUP, slurm_shutdown);
    signal(SIGINT, slurm_shutdown);
    signal(SIGQUIT, slurm_shutdown);
    signal(SIGKILL, slurm_shutdown);
    signal(SIGTERM, slurm_shutdown);

    draw_face(displaymode, hostname);
    update_info(displaymode);
    now = time(NULL);
    old[0] = old[1] = time(NULL) - 3;

    for (;;) {
        get_stat();
#ifdef WITH_LED
        if (ledenabled) {
            if (displaymode != MODE_LARGE) {
                /* In case of a new RX Packet... the RX Led Turn on 
                 * Else... Turn off */
                if (stats.rx_packets != stats.rx_packets_led)
                    led_on(LED_RX);
                else
                    led_off(LED_RX);
                /* In case of a new TX Packet... the TX Led Turn on
                 * Else... Turn off */
                if (stats.tx_packets != stats.tx_packets_led)
                    led_on(LED_TX);
                else
                    led_off(LED_TX);
            }
        }                       /* end ledenabled */
#endif

        stats.rx_packets_led = stats.rx_packets;
        stats.tx_packets_led = stats.tx_packets;

        get_time();
        /* check if we have to switch to Gbytes mode */
        if (data_type != TYPE_GIGA &&
            ((stats.tx_bytes > 1073735680) ||
             (stats.rx_bytes > 1073735680))) {
            data_type = TYPE_GIGA;
            draw_face(displaymode, hostname);
        }

        update_info(displaymode);

        if ((!sec_value && first) || disconnected) {
            sec_value = stats.online_sec;
            min_value = stats.online_min;
            update_stat(displaymode);
            first = 0;
            disconnected = 0;
        }

        now = time(NULL);

        if (stats.online_sec != sec_value) {
            update_stat(displaymode);
            sec_value = stats.online_sec;
        }

        /* move cursor to upper left corner */
        move(0, 0);

        tmpdelay = refreshdelay * 10;
        do {
            usleep(100 * 1000); /* 10th of a second */
            tmpdelay--;
            key_pressed = getch();
            if (key_pressed != ERR)
                break;
        } while (tmpdelay > 0);

        if (key_pressed != ERR && tolower(key_pressed) == 'q')
            break;
        else if (key_pressed != ERR && tolower(key_pressed) == 'c') {
            displaymode = MODE_COMBINED;
            modechange++;
        } else if (key_pressed != ERR && tolower(key_pressed) == 'l') {
            displaymode = MODE_LARGE;
            modechange++;
        } else if (key_pressed != ERR && tolower(key_pressed) == 's') {
            displaymode = MODE_SPLIT;
            modechange++;
        } else if (key_pressed != ERR && tolower(key_pressed) == 'm') {
            if (displaymode == MODE_COMBINED)
                displaymode = MODE_SPLIT;
            else if (displaymode == MODE_SPLIT)
                displaymode = MODE_LARGE;
            else
                displaymode = MODE_COMBINED;

            modechange++;
        } else if (key_pressed != ERR && tolower(key_pressed) == 'r') {
            draw_face(displaymode, hostname);
            update_stat(displaymode);
            reload = TRUE;
        } else if (key_pressed != ERR && tolower(key_pressed) == 'z') {
            zero_stats();
            update_stat(displaymode);
            reload = TRUE;
        }

        /* if the display mode change we need to redraw the face and
         * reinitialize the graph arrays
         * draw_face() clears the screen
         */

        if (modechange) {
            db_status = DB_STATUS_REINIT;
            modechange = 0;
            draw_face(displaymode, hostname);
        }
    }
    slurm_shutdown(1);          /* needs a bogus parameter :) */
    return 0;
}

/******************************************************************************
 *
 * update_stat()
 *
 * graph update dispatcher, just calls the right function
 *
 *****************************************************************************/
int update_stat(int displaymode)
{
    switch (displaymode) {
    case MODE_COMBINED:
        return update_stat_combined();
        break;
    case MODE_SPLIT:
        return update_stat_split();
        break;
    case MODE_LARGE:
        return update_stat_large();
        break;
    default:
        return 1;
    }
}

/******************************************************************************
 *
 * zero_stats()
 *
 * redefine the offsets for all counters so we can virtually reset
 * the counters on the display
 *
 *****************************************************************************/

void zero_stats(void)
{
    stats.rx_packets_off = stats.rx_packets;
    stats.rx_errors_off = stats.rx_errors;
    stats.rx_over_off = stats.rx_over;
    stats.tx_packets_off = stats.tx_packets;
    stats.tx_errors_off = stats.tx_errors;
    stats.tx_over_off = stats.tx_over;
    stats.rx_bytes_off = stats.rx_bytes;
    stats.tx_bytes_off = stats.tx_bytes;
    stats.rx_bytes_comp_off = stats.rx_bytes_comp;
    stats.tx_bytes_comp_off = stats.tx_bytes_comp;
}

/******************************************************************************
 *
 * slurm_shutdown()
 *
 * shutdown curses so we get a clean terminal.
 *
 *****************************************************************************/

void slurm_shutdown(int sig)
{
    endwin();
    system("clear");
    curs_set(1);
    fprintf(stdout, "slurm %s  - "
            "http://www.github.com/mattthias/slurm/\n", VERSION);

    /* close dev if running Linux */
#ifdef __linux__
    fclose(proc_net_dev);
#endif
    exit(0);
}
