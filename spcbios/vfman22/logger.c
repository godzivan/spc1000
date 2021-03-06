/*
 * logger.c version 2.2 part of the vfman 2.2 package
 *
 * Copyright 2009-2013 Fred Jan Kraan
 *
 * This version delivered in 2013 by Fred Jan Kraan (fjkraan@xs4all.nl)
 *
 * vfman is placed under the GNU General Public License in March 2010.
 *
 *  This file is part of Vfman 2.2.
 *
 *  Vfman is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  Vfman is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Epspd88; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

/*
logger.c - logging for vfman
*/

#include <stdio.h>
#include <stdarg.h>
#include "logger.h"

static int outLevel = LOG_INFO;

/* Log a message. */
void msg(int level, const char *fmt, ...)
{
  va_list args;

  if (level <= outLevel) {
    va_start(args, fmt);
    vfprintf(stdout, fmt, args);
    va_end(args);
  }
}

void setLogLevel(int level) {
    outLevel = level;
    if (level < LOG_ERROR) outLevel = LOG_ERROR;
    if (level > LOG_TRACE) outLevel = LOG_TRACE;
}

int getLogLevel() {
    return outLevel;
}