/*
 * This file is part of the libopentracedecode project.
 *
 * Copyright (C) 2013 Uwe Hermann <uwe@hermann-uwe.de>
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
#include <opentracedecode/libopentracedecode.h>

/**
 * @file
 *
 * Error handling in libopentracedecode.
 */

/**
 * @defgroup grp_error Error handling
 *
 * Error handling in libopentracedecode.
 *
 * libopentracedecode functions usually return @ref OTD_OK upon success, or a
 * negative error code on failure.
 *
 * @{
 */

/**
 * Return a human-readable error string for the given libopentracedecode error
 * code.
 *
 * @param error_code A libopentracedecode error code number, such as
 *                   OTD_ERR_MALLOC.
 *
 * @return A const string containing a short, human-readable (English)
 *         description of the error, such as "memory allocation error".
 *         The string must NOT be free'd by the caller!
 *
 * @see otd_strerror_name
 *
 * @since 0.2.0
 */
OTD_API const char *otd_strerror(int error_code)
{
	const char *str;

	/*
	 * Note: All defined OTD_* error macros from libopentracedecode.h must
	 * have an entry in this function, as well as in otd_strerror_name().
	 */

	switch (error_code) {
	case OTD_OK:
		str = "no error";
		break;
	case OTD_ERR:
		str = "generic/unspecified error";
		break;
	case OTD_ERR_MALLOC:
		str = "memory allocation error";
		break;
	case OTD_ERR_ARG:
		str = "invalid argument";
		break;
	case OTD_ERR_BUG:
		str = "internal error";
		break;
	case OTD_ERR_PYTHON:
		str = "Python API error";
		break;
	case OTD_ERR_DECODERS_DIR:
		str = "decoders directory access error";
		break;
	default:
		str = "unknown error";
		break;
	}

	return str;
}

/**
 * Return the "name" string of the given libopentracedecode error code.
 *
 * For example, the "name" of the OTD_ERR_MALLOC error code is
 * "OTD_ERR_MALLOC", the name of the OTD_OK code is "OTD_OK", and so on.
 *
 * This function can be used for various purposes where the "name" string of
 * a libopentracedecode error code is useful.
 *
 * @param error_code A libopentracedecode error code number, such as
 *                   OTD_ERR_MALLOC.
 *
 * @return A const string containing the "name" of the error code as string.
 *         The string must NOT be free'd by the caller!
 *
 * @see otd_strerror
 *
 * @since 0.2.0
 */
OTD_API const char *otd_strerror_name(int error_code)
{
	const char *str;

	/*
	 * Note: All defined OTD_* error macros from libopentracedecode.h must
	 * have an entry in this function, as well as in otd_strerror().
	 */

	switch (error_code) {
	case OTD_OK:
		str = "OTD_OK";
		break;
	case OTD_ERR:
		str = "OTD_ERR";
		break;
	case OTD_ERR_MALLOC:
		str = "OTD_ERR_MALLOC";
		break;
	case OTD_ERR_ARG:
		str = "OTD_ERR_ARG";
		break;
	case OTD_ERR_BUG:
		str = "OTD_ERR_BUG";
		break;
	case OTD_ERR_PYTHON:
		str = "OTD_ERR_PYTHON";
		break;
	case OTD_ERR_DECODERS_DIR:
		str = "OTD_ERR_DECODERS_DIR";
		break;
	default:
		str = "unknown error code";
		break;
	}

	return str;
}

/** @} */
