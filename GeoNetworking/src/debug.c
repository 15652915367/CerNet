/***************************************************************************
 *                              CarGeo6 Project                            *
 *         C2CNet layer implementation conforming with GeoNet D2.2         *
 *                                                                         *
 *                      Copyright(C)2010 ESPRIT-INRIA                      *
 *									   *
 *									   *
 *   Authors:								   *
 *   Manabu TSUKADA <Manabu.Tsukada@inria.fr>                              *
 *   			                                                   *
 * 	 				                                   *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

/**
 * @file debug.c
 * debug code.
 * @author Manabu TSUKADA
 */

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <sys/uio.h>
#include <errno.h>
#include <string.h>
#include <time.h>

#include "debug.h"
#include "itsnet.h"

static FILE *sdbg;

#if 0
static const char *dbg_strdate(char *str)
{
	struct timespec ts;
	time_t t;
	int ret;

	memset(&ts, 0, sizeof(ts));

	ret = clock_gettime(CLOCK_REALTIME, &ts);
	if (ret != 0)
		sprintf(str, "(clock_gettime error=%d)", errno);
	else {
		t = (time_t)ts.tv_sec; /* XXX: fix it! */
		if (t == 0) {
			strcpy(str, "(undefined)");
		} else {
			char buf[1024];
			sprintf(str, "%.19s",
				(ctime_r(&t, buf) ? buf : "(ctime_r error)"));
		}
	}

	return str;
}
#endif

void dbgprint(const char *fname, const char *fmt, ...)
{
        char s[1024];
        va_list args;
 	if (dbg_level != 0){ 
		va_start(args, fmt);
		vsprintf(s, fmt, args);
		va_end(args);

		fprintf(sdbg, "%s", s);
		fflush(sdbg);
	}
}

#if 0
void debug_print_buffer(const void *data, int len, const char *fname, 
			const char *fmt, ...)
{ 
	int i; 
	char s[1024];
        va_list args;
 
        va_start(args, fmt);
        vsprintf(s, fmt, args);
        fprintf(sdbg, "%s: %s", fname, s);
        va_end(args);
	for (i = 0; i < len; i++) { 
		if (i % 16 == 0) fprintf(sdbg, "\n%04x: ", i);
		fprintf(sdbg, "%02x ", ((unsigned char *)data)[i]);
	} 
	fprintf(sdbg, "\n\n");
	fflush(sdbg);
}

void debug_print_func(void *arg, void (*func)(void *arg, void *stream))
{
	func(arg, sdbg);
	fflush(sdbg);
}
#endif

int debug_open(const char *path)
{
	FILE *fp;

	if (!path)
		return -EINVAL;
	if (sdbg && sdbg != stderr)
		return -EALREADY;

	if ((fp = fopen(path, "a")) == NULL ){
		fprintf(stderr, "file open error!!\n");
		exit(0);
	}
		
	if (!fp)
		return -errno;
	sdbg = fp;

	return 0;
}

void debug_close(void)
{
	if (sdbg && sdbg != stderr)
		fclose(sdbg);
	debug_init();
}

void debug_init(void)
{
	sdbg = stderr;
}

