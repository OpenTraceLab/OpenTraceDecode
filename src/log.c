/*
 * This file is part of the libopentracedecode project.
 *
 * Copyright (C) 2011-2012 Uwe Hermann <uwe@hermann-uwe.de>
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
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 */

#include <config.h>
#include "libopentracedecode-internal.h" /* First, so we avoid a _POSIX_C_SOURCE warning. */
#include <opentracedecode/libopentracedecode.h>
#include <stdarg.h>
#include <stdio.h>
#include <glib/gprintf.h>

/**
 * @file
 *
 * Controlling the libopentracedecode message logging functionality.
 */

/**
 * @defgroup grp_logging Logging
 *
 * Controlling the libopentracedecode message logging functionality.
 *
 * @{
 */

/* Currently selected libopentracedecode loglevel. Default: OTD_LOG_WARN. */
static int cur_loglevel = OTD_LOG_WARN; /* Show errors+warnings per default. */

/* Function prototype. */
static int otd_logv(void *cb_data, int loglevel, const char *format,
		    va_list args);

/* Pointer to the currently selected log callback. Default: otd_logv(). */
static otd_log_callback otd_log_cb = otd_logv;

/*
 * Pointer to private data that can be passed to the log callback.
 * This can be used (for example) by C++ GUIs to pass a "this" pointer.
 */
static void *otd_log_cb_data = NULL;

/**
 * Set the libopentracedecode loglevel.
 *
 * This influences the amount of log messages (debug messages, error messages,
 * and so on) libopentracedecode will output. Using OTD_LOG_NONE disables all
 * messages.
 *
 * Note that this function itself will also output log messages. After the
 * loglevel has changed, it will output a debug message with OTD_LOG_DBG for
 * example. Whether this message is shown depends on the (new) loglevel.
 *
 * @param loglevel The loglevel to set (OTD_LOG_NONE, OTD_LOG_ERR,
 *                 OTD_LOG_WARN, OTD_LOG_INFO, OTD_LOG_DBG, or OTD_LOG_SPEW).
 *
 * @return OTD_OK upon success, OTD_ERR_ARG upon invalid loglevel.
 *
 * @since 0.1.0
 */
OTD_API int otd_log_loglevel_set(int loglevel)
{
	if (loglevel < OTD_LOG_NONE || loglevel > OTD_LOG_SPEW) {
		otd_err("Invalid loglevel %d.", loglevel);
		return OTD_ERR_ARG;
	}

	cur_loglevel = loglevel;

	otd_dbg("libopentracedecode loglevel set to %d.", loglevel);

	return OTD_OK;
}

/**
 * Get the libopentracedecode loglevel.
 *
 * @return The currently configured libopentracedecode loglevel.
 *
 * @since 0.1.0
 */
OTD_API int otd_log_loglevel_get(void)
{
	return cur_loglevel;
}

/**
 * Set the libopentracedecode log callback to the specified function.
 *
 * @param cb Function pointer to the log callback function to use.
 *           Must not be NULL.
 * @param cb_data Pointer to private data to be passed on. This can be used
 *                by the caller to pass arbitrary data to the log functions.
 *                This pointer is only stored or passed on by libopentracedecode,
 *                and is never used or interpreted in any way. The pointer
 *                is allowed to be NULL if the caller doesn't need/want to
 *                pass any data.
 *
 * @return OTD_OK upon success, OTD_ERR_ARG upon invalid arguments.
 *
 * @since 0.3.0
 */
OTD_API int otd_log_callback_set(otd_log_callback cb, void *cb_data)
{
	if (!cb) {
		otd_err("log: %s: cb was NULL", __func__);
		return OTD_ERR_ARG;
	}

	/* Note: 'cb_data' is allowed to be NULL. */

	otd_log_cb = cb;
	otd_log_cb_data = cb_data;

	return OTD_OK;
}

/**
 * Get the libopentracedecode log callback routine and callback data.
 *
 * @param[out] cb Pointer to a function pointer to receive the log callback
 *	function. Optional, can be NULL.
 * @param[out] cb_data Pointer to a void pointer to receive the log callback's
 *	additional arguments. Optional, can be NULL.
 *
 * @return OTD_OK upon success.
 *
 * @since 0.5.2
 */
OTD_API int otd_log_callback_get(otd_log_callback *cb, void **cb_data)
{
	if (cb)
		*cb = otd_log_cb;
	if (cb_data)
		*cb_data = otd_log_cb_data;

	return OTD_OK;
}

/**
 * Set the libopentracedecode log callback to the default built-in one.
 *
 * Additionally, the internal 'otd_log_cb_data' pointer is set to NULL.
 *
 * @return OTD_OK upon success, a (negative) error code otherwise.
 *
 * @since 0.1.0
 */
OTD_API int otd_log_callback_set_default(void)
{
	/*
	 * Note: No log output in this function, as it should safely work
	 * even if the currently set log callback is buggy/broken.
	 */
	otd_log_cb = otd_logv;
	otd_log_cb_data = NULL;

	return OTD_OK;
}

static int otd_logv(void *cb_data, int loglevel, const char *format,
		    va_list args)
{
	/* This specific log callback doesn't need the void pointer data. */
	(void)cb_data;

	(void)loglevel;

	if (fputs("srd: ", stderr) < 0
			|| g_vfprintf(stderr, format, args) < 0
			|| putc('\n', stderr) < 0)
		return OTD_ERR;

	fflush(stderr);

	return OTD_OK;
}

/** @private */
OTD_PRIV int otd_log(int loglevel, const char *format, ...)
{
	int ret;
	va_list args;

	/* Only output messages of at least the selected loglevel(s). */
	if (loglevel > cur_loglevel)
		return OTD_OK;

	va_start(args, format);
	ret = otd_log_cb(otd_log_cb_data, loglevel, format, args);
	va_end(args);

	return ret;
}

/** @} */
