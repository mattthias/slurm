/******************************************************************************
 *
 * theme.h - slurm theme engine definitions
 *
 ******************************************************************************
 * $Id: theme.h,v 1.2 2003/07/09 16:45:37 hscholz Exp $
 *****************************************************************************/

#ifndef _THEME_H
#define _THEME_H

/* prototypes */
int theme_mapcolor(char *);
int theme_mapattrib(char *);
int theme_mapkey(char *);

typedef struct {
	int complete;
	int background;
	int rx;
	int rx_attrib;
	int tx;
	int tx_attrib;
	int text;
	int text_attrib;	
	int var;
	int var_attrib;
	int title;
	int title_attrib;
} theme;
theme t;

/* possible entities */
#define E_BACKGROUND	0x0101
#define E_RX			0x0102
#define E_TX			0x0104
#define E_TEXT			0x0108
#define E_TEXTVAR		0x0110
#define E_TITLE			0x0120
#define E_RXATTR		0x0201
#define E_TXATTR		0x0202
#define E_TEXTATTR		0x0204
#define E_TEXTVARATTR	0x0208
#define E_TITLEATTR		0x0210

#define E_ALL	(E_BACKGROUND | E_RX | E_TX | E_TEXT | E_TEXTVAR | \
				 E_RXATTR | E_TXATTR | E_TEXTATTR | E_TEXTVARATTR | \
				 E_TITLE | E_TITLEATTR)
#define E_NULL	0

/* color definitions */
#ifndef COLOR_BLACK
#define COLOR_BLACK		0
#define COLOR_RED		1
#define COLOR_GREEN		2
#define COLOR_YELLOW	3
#define COLOR_BLUE		4
#define COLOR_MAGENTA	5
#define COLOR_CYAN		6
#define COLOR_WHITE		7
#endif
#define COLOR_TRANSPARENT -1

#define COL_NORMAL	0
#define COL_BOLD	1
#define COL_DIM		2

/* internal color pairs */
#define PAIR_TEXT 1
#define PAIR_RX 2
#define PAIR_TX 3
#define PAIR_VAR 4
#define PAIR_TITLE 5

#endif
