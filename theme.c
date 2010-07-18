/******************************************************************************
 *
 * theme.c - slurm theme engine
 *
 ******************************************************************************
 * $Id: theme.c,v 1.5 2004/09/28 14:23:02 hscholz Exp $
 *****************************************************************************/

#ifndef _THEME_C
#define _THEME_C

/******************************************************************************
 *
 * theme_setdefault()
 *
 * set default theme
 *
 *****************************************************************************/

void theme_setdefault(theme *t)
{
	if (t == NULL)
		error(ERR_FATAL, "theme_setdefault() received NULL pointer");

	t->background	= COLOR_TRANSPARENT;
	t->rx			= COLOR_GREEN;
	t->tx			= COLOR_RED;
	t->text			= COLOR_WHITE;
	t->var			= COLOR_WHITE;
	t->title		= COLOR_WHITE;
	t->rx_attrib	= COL_BOLD;
	t->tx_attrib	= COL_BOLD;
	t->text_attrib	= COL_NORMAL;
	t->var_attrib	= COL_BOLD;
	t->title_attrib	= COL_BOLD;
	t->complete		= E_ALL;
}

/******************************************************************************
 *
 * theme_readfile()
 *
 * read color definitions from the given file and store in theme struct
 *
 *****************************************************************************/

int theme_readfile(theme *t, char *name)
{
	FILE *f;
	char buf[BUFSIZ+1];
	char filename[BUFSIZ+1];
	char *p, *val, *key;

	t->complete = E_NULL;

	/* search for the theme:
	 *  - in the local directory
	 *  - in ~/.slurm
	 *  - in /usr/local/share/slurm
	 *    (make this /usr/share/slurm for Debian systems by specifing
	 *     -D__Debian__)
	 */

	bzero(&filename, BUFSIZ);
	snprintf(filename, BUFSIZ, "%s.theme", name);
	if ((f = fopen(filename, "r")) == NULL)
	{
		bzero(&filename, BUFSIZ);
		snprintf(filename, BUFSIZ, "%s/.slurm/%s.theme", getenv("HOME"), name);
		if ((f = fopen(filename, "r")) == NULL)
		{
			bzero(&filename, BUFSIZ);
#ifdef __NetBSD__
			snprintf(filename, BUFSIZ, "/usr/pkg/share/slurm/%s.theme", name);
#elif __Debian__
			snprintf(filename, BUFSIZ, "/usr/share/slurm/%s.theme", name);
#else
			snprintf(filename, BUFSIZ, "/usr/local/share/slurm/%s.theme", name);
#endif
			if ((f = fopen(filename, "r")) == NULL)
				error(ERR_FATAL, "cannot find theme '%s'", name);
		}
	}

	while (fgets(buf, BUFSIZ, f) != NULL)
	{
		if (!strncmp(buf,"#",1)) continue; /* comment */
		p = buf;
		while (isspace((int) *p) && (*p+1 != '\0')) p++;
		key = p;
		while (isalpha((int) *p) && (*p+1 != '\0')) p++;
		if (isspace((int) *p))
		{
			*p++ = '\0';
			while (isspace((int) *p) && (*p+1 != '\0')) p++;
			if (*p != '=') continue;
			p++;
			while (isspace((int) *p) && (*p+1 != '\0')) p++;
		}
		else if (*p == '=')
		{
			*p++ = '\0';
			while (isspace((int) *p) && (*p+1 != '\0')) p++;
		}
		else
			continue;
		val = p;
		while (isalpha((int) *p) && (*p+1 != '\0')) p++;
		*p++ = '\0';

		switch (theme_mapkey(key))
		{
			case E_BACKGROUND:
				t->background = theme_mapcolor(val);
				t->complete |= E_BACKGROUND;
				break;
            case E_RX:
                t->rx = theme_mapcolor(val);
                t->complete |= E_RX;
                break;
            case E_TX:
                t->tx = theme_mapcolor(val);
                t->complete |= E_TX;
                break;
            case E_TEXT:
                t->text = theme_mapcolor(val);
                t->complete |= E_TEXT;
                break;
            case E_TITLE:
                t->title = theme_mapcolor(val);
                t->complete |= E_TITLE;
                break;
            case E_TEXTVAR:
                t->var = theme_mapcolor(val);
                t->complete |= E_TEXTVAR;
                break;
            case E_RXATTR:
                t->rx_attrib = theme_mapattrib(val);
                t->complete |= E_RXATTR;
                break;
            case E_TXATTR:
                t->tx_attrib = theme_mapattrib(val);
                t->complete |= E_TXATTR;
                break;
            case E_TEXTATTR:
                t->text_attrib = theme_mapattrib(val);
                t->complete |= E_TEXTATTR;
                break;
            case E_TEXTVARATTR:
                t->var_attrib = theme_mapattrib(val);
                t->complete |= E_TEXTVARATTR;
                break;
            case E_TITLEATTR:
                t->title_attrib = theme_mapattrib(val);
                t->complete |= E_TITLEATTR;
                break;
			default:
				error(ERR_WARNING, "unknown key '%s' in theme '%s'",
					key, filename);
		}
	}
	fclose(f);
	return (t->complete == E_ALL)? 1 : 0;
}

/******************************************************************************
 *
 * theme_mapattrib()
 *
 * map entity attribute to coresponding curses attribute
 *
 *****************************************************************************/

int theme_mapattrib(char *str)
{
	int val = COL_BOLD;
	char buf[32];
	int i;

	if (strlen(str) > 31) return val;
	bzero(&buf, 31);
	for (i = 0; i < strlen(str); i++)
		buf[i] = toupper((int) str[i]);
	buf[i+1] = '\0';

		 if (!strcmp(buf, "BOLD"))			val = COL_BOLD;
	else if (!strcmp(buf, "NORMAL"))		val = COL_NORMAL;
	else if (!strcmp(buf, "DIM"))			val = COL_DIM;

	return val;
}

/******************************************************************************
 *
 * theme_mapcolor()
 *
 * map color name to coresponding curses color name
 *
 *****************************************************************************/

int theme_mapcolor(char *str)
{
	int val = COLOR_GREEN;
	char buf[32];
	int i;

	if (strlen(str) > 31) return val;
	bzero(&buf, 31);
	for (i = 0; i < strlen(str); i++)
		buf[i] = toupper((int) str[i]);
	buf[i+1] = '\0';

		 if (!strcmp(buf, "BLACK"))			val = COLOR_BLACK;
	else if (!strcmp(buf, "RED"))			val = COLOR_RED;
	else if (!strcmp(buf, "GREEN"))			val = COLOR_GREEN;
	else if (!strcmp(buf, "YELLOW"))		val = COLOR_YELLOW;
	else if (!strcmp(buf, "BLUE"))			val = COLOR_BLUE;
	else if (!strcmp(buf, "MAGENTA"))		val = COLOR_MAGENTA;
	else if (!strcmp(buf, "CYAN"))			val = COLOR_CYAN;
	else if (!strcmp(buf, "WHITE"))			val = COLOR_WHITE;
	else if (!strcmp(buf, "TRANSPARENT"))	val = COLOR_TRANSPARENT;

	return val;
}

/******************************************************************************
 *
 * theme_mapkey()
 *
 * map entity name to internal name
 *
 *****************************************************************************/

int theme_mapkey(char *str)
{
	int e = 0;
	char buf[32];
	int i;

	if (strlen(str) > 31) return e;
	bzero(&buf, 31);
	for (i = 0; i < strlen(str); i++)
		buf[i] = toupper((int) str[i]);
	buf[i+1] = '\0';

		 if (!strcmp(buf, "BACKGROUND"))	e = E_BACKGROUND;
	else if (!strcmp(buf, "RX"))			e = E_RX;
	else if (!strcmp(buf, "TX"))			e = E_TX;
	else if (!strcmp(buf, "TEXT"))			e = E_TEXT;
	else if (!strcmp(buf, "TITLE"))			e = E_TITLE;
	else if (!strcmp(buf, "TEXTVAR"))		e = E_TEXTVAR;
	else if (!strcmp(buf, "RXATTR"))		e = E_RXATTR;
	else if (!strcmp(buf, "TXATTR"))		e = E_TXATTR;
	else if (!strcmp(buf, "TEXTATTR"))		e = E_TEXTATTR;
	else if (!strcmp(buf, "TEXTVARATTR"))	e = E_TEXTVARATTR;
	else if (!strcmp(buf, "TITLEATTR"))		e = E_TITLEATTR;
	return e;
}
#endif
