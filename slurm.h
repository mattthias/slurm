/******************************************************************************
 *
 * slurm config file, heavily based on the pppstatus version
 * see README file or slurm.c for more information
 *
 * $Id: slurm.h,v 1.42 2004/02/13 05:41:30 hscholz Exp $
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
 ***************************************************************************/

#define LED_RX    1
#define LED_TX    2

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

#define SYMBOL_TRAFFIC		"x"
#define SYMBOL_NOTRAFFIC	" "

#ifdef __linux__
#define PATH_NET_DEV "/proc/net/dev"
#endif

#define MODE_COMBINED 1
#define MODE_SPLIT 2
#define MODE_LARGE 3

#if __GNUC__ >= 2
#define UNUSED_ARG(ARG) ARG __attribute__((unused))
#elif defined(S_SPLINT_S)
#define UNUSED_ARG(ARG) /*@unused@*/ ARG
#define const /*@observer@*/ /*@temp@*/
#else
#define UNUSED_ARG(ARG) ARG
#endif

/* slap HPUX with a large trout
 * HPUX 10.xx cannot handle 2^32 int constants, so we have to "tweak" it
 */

#ifdef __HPUX__
#define SNMPMAXBYTES (4294967295 + 1)
#else
#define SNMPMAXBYTES (4294967296ULL)
#endif

#undef MAXHOSTNAMELEN
#define MAXHOSTNAMELEN 64

#define DRAWLEN 16

/* 10th of a second */
#define REFRESH_DEFAULT 1
#define REFRESH_MIN 1
#define REFRESH_MAX 300

#ifndef BUFSIZ
#define BUFSIZ 1024
#warning "setting buf size to 1024"
#endif

typedef struct statvline {
   int hline[76]; /* This will be the horzontal lines */
} statvline[10]; /* ... and this will be vertical lines */
statvline graph;

/* This structure stays the INFO variables */
typedef struct DataStats {
    unsigned long rx_packets;
    unsigned long rx_errors;
    int rx_over;
    unsigned long tx_packets;
    unsigned long tx_errors;
    int tx_over;
    double rx_bytes;
    double tx_bytes;
    double rx_bytes_comp;
    double tx_bytes_comp;
    double rx_packets_led;
    double tx_packets_led;
    unsigned long connect_time;
    unsigned long current_time;
    float top_speed;
    int online_days;
    int online_hour;
    int online_min;
    int online_sec;
	unsigned long rx_packets_off;
	unsigned long rx_errors_off;
    int rx_over_off;
    unsigned long tx_packets_off;
    unsigned long tx_errors_off;
    int tx_over_off;
    double rx_bytes_off;
    double tx_bytes_off;
    double rx_bytes_comp_off;
    double tx_bytes_comp_off;
} DataStats;
DataStats stats;

typedef struct IfData{
    char if_name[10];       /* The device name given as start parameter*/
    int  if_speed;          /* The Interface speed */
    char if_speedstring[12]; /* the measuring unit like Mbit, kbit */
    int  if_id;             /* The ID which the interface inside the OS has */
    int  if_amount;         /* The amount of all interfaces available */
    int  if_valid;          /* 1 = selected interface exists
                             * 0 = interfaces does not exists */
} IfData;

IfData ifdata;

/* Prototypes */
void center(short int line, char *strg, ...);
void chcolor(int x, int y);
int update_stat_combined(void);
int update_stat_split(void);
int update_stat_large(void);
int update_stat(int);
void update_info(int);
void led_on(unsigned int who);
void led_off(unsigned int who);
int is_online(void);
int get_stat(void);
void clear_info(void);
void ip_address(int display);
void get_time (void);
void do_exit (int, char *, short int);
void usage (int, char **);
void draw_face (int, char *);
void zero_stats (void);
void slurm_shutdown (int);

/* Variables Declarations */
#define TYPE_MEGA 0
#define TYPE_GIGA 1
int data_type = TYPE_MEGA;
long refreshdelay = REFRESH_DEFAULT;

/* internal database status */
/* possible modes are: */
#define DB_STATUS_STARTUP 1
#define DB_STATUS_RUNNING 2
#define DB_STATUS_REINIT  3
int db_status = DB_STATUS_STARTUP;

#ifdef __linux
static FILE *proc_net_dev;
#endif
/* End of Variables Declarations */

/* max speed in graph */
#define GRAPHSINGLE_WIDTH 77
#define GRAPHSINGLE_HEIGHT 10

/* define graph height for split screen graphs */
#define GRAPHSPLIT_HEIGHT 6
#define GRAPHSPLIT_WIDTH 77

#define GRAPHCOMBINED_WIDTH 77
#define GRAPHCOMBINED_HEIGHT 12

/* new max height */
#define GRAPH_HEIGHT 12
#define GRAPH_WIDTH 77

/* large split mode graph height */
#define GRAPHLARGE_HEIGHT 11

/* rx is higher than need as we use it for the combined view */
int rx_graph[GRAPHSPLIT_WIDTH][GRAPHCOMBINED_HEIGHT];
float rx_speedarray[GRAPHSPLIT_WIDTH];
static float rx_maxspeed;
static int rx_maxspeedpos;
int rx_overallmax;

int tx_graph[GRAPHSPLIT_WIDTH][GRAPHCOMBINED_HEIGHT];
float tx_speedarray[GRAPHSPLIT_WIDTH];
static float tx_maxspeed;
static int tx_maxspeedpos;
int tx_overallmax;

/* combined view */
static float comb_maxspeed;
int comb_overallmax;
int solcount;

int ledenabled;	/* TX/RX led enabled */
