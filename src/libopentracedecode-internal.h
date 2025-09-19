/*
 * This file is part of the libopentracedecode project.
 *
 * Copyright (C) 2011 Uwe Hermann <uwe@hermann-uwe.de>
 * Copyright (C) 2012 Bert Vermeulen <bert@biot.com>
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

#ifndef LIBSIGROKDECODE_LIBSIGROKDECODE_INTERNAL_H
#define LIBSIGROKDECODE_LIBSIGROKDECODE_INTERNAL_H

/* Use the stable ABI subset as per PEP 384. */
#define Py_LIMITED_API 0x03020000

#include <Python.h> /* First, so we avoid a _POSIX_C_SOURCE warning. */
#include <opentracedecode/libopentracedecode.h>

/*
 * Static definition of tables ending with an all-zero sentinel entry
 * may raise warnings when compiling with -Wmissing-field-initializers.
 * GCC suppresses the warning only with { 0 }, clang wants { } instead.
 */
#ifdef __clang__
#  define ALL_ZERO { }
#else
#  define ALL_ZERO { 0 }
#endif

enum {
	OTD_TERM_ALWAYS_FALSE,
	OTD_TERM_HIGH,
	OTD_TERM_LOW,
	OTD_TERM_RISING_EDGE,
	OTD_TERM_FALLING_EDGE,
	OTD_TERM_EITHER_EDGE,
	OTD_TERM_NO_EDGE,
	OTD_TERM_SKIP,
};

struct otd_term {
	int type;
	int channel;
	uint64_t num_samples_to_skip;
	uint64_t num_samples_already_skipped;
};

/* Custom Python types: */

typedef struct {
	PyObject_HEAD
	struct otd_decoder_inst *di;
	uint64_t abs_start_samplenum;
	unsigned int itercnt;
	uint8_t *inbuf;
	uint64_t inbuflen;
	PyObject *sample;
} otd_logic;

struct otd_session {
	int session_id;

	/* List of decoder instances. */
	GSList *di_list;

	/* List of frontend callbacks to receive decoder output. */
	GSList *callbacks;
};

/* srd.c */
OTD_PRIV int otd_decoder_searchpath_add(const char *path);

/* session.c */
OTD_PRIV struct otd_pd_callback *otd_pd_output_callback_find(struct otd_session *sess,
		int output_type);

/* instance.c */
OTD_PRIV int otd_inst_start(struct otd_decoder_inst *di);
OTD_PRIV void match_array_free(struct otd_decoder_inst *di);
OTD_PRIV void condition_list_free(struct otd_decoder_inst *di);
OTD_PRIV int otd_inst_decode(struct otd_decoder_inst *di,
		uint64_t abs_start_samplenum, uint64_t abs_end_samplenum,
		const uint8_t *inbuf, uint64_t inbuflen, uint64_t unitsize);
OTD_PRIV int process_samples_until_condition_match(struct otd_decoder_inst *di, gboolean *found_match);
OTD_PRIV int otd_inst_flush(struct otd_decoder_inst *di);
OTD_PRIV int otd_inst_send_eof(struct otd_decoder_inst *di);
OTD_PRIV int otd_inst_terminate_reset(struct otd_decoder_inst *di);
OTD_PRIV void otd_inst_free(struct otd_decoder_inst *di);
OTD_PRIV void otd_inst_free_all(struct otd_session *sess);

/* log.c */
#if defined(G_OS_WIN32) && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 4))
/*
 * On MinGW, we need to specify the gnu_printf format flavor or GCC
 * will assume non-standard Microsoft printf syntax.
 */
OTD_PRIV int otd_log(int loglevel, const char *format, ...)
		__attribute__((__format__ (__gnu_printf__, 2, 3)));
#else
OTD_PRIV int otd_log(int loglevel, const char *format, ...) G_GNUC_PRINTF(2, 3);
#endif

#define otd_spew(...)	otd_log(OTD_LOG_SPEW, __VA_ARGS__)
#define otd_dbg(...)	otd_log(OTD_LOG_DBG,  __VA_ARGS__)
#define otd_info(...)	otd_log(OTD_LOG_INFO, __VA_ARGS__)
#define otd_warn(...)	otd_log(OTD_LOG_WARN, __VA_ARGS__)
#define otd_err(...)	otd_log(OTD_LOG_ERR,  __VA_ARGS__)

/* decoder.c */
OTD_PRIV long otd_decoder_apiver(const struct otd_decoder *d);

/* type_decoder.c */
OTD_PRIV PyObject *otd_Decoder_type_new(void);
OTD_PRIV const char *output_type_name(unsigned int idx);

/* type_logic.c */
OTD_PRIV PyObject *otd_logic_type_new(void);

/* module_sigrokdecode.c */
PyMODINIT_FUNC PyInit_opentracedecode(void);
PyMODINIT_FUNC PyInit_opentracedecode(void);

/* util.c */
OTD_PRIV PyObject *py_import_by_name(const char *name);
OTD_PRIV int py_attr_as_str(PyObject *py_obj, const char *attr, char **outstr);
OTD_PRIV int py_attr_as_strlist(PyObject *py_obj, const char *attr, GSList **outstrlist);
OTD_PRIV int py_dictitem_as_str(PyObject *py_obj, const char *key, char **outstr);
OTD_PRIV int py_listitem_as_str(PyObject *py_obj, Py_ssize_t idx, char **outstr);
OTD_PRIV int py_pydictitem_as_str(PyObject *py_obj, PyObject *py_key, char **outstr);
OTD_PRIV int py_pydictitem_as_long(PyObject *py_obj, PyObject *py_key, int64_t *out);
OTD_PRIV int py_str_as_str(PyObject *py_str, char **outstr);
OTD_PRIV int py_strseq_to_char(PyObject *py_strseq, char ***out_strv);
OTD_PRIV GVariant *py_obj_to_variant(PyObject *py_obj);

/* exception.c */
#if defined(G_OS_WIN32) && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 4))
/*
 * On MinGW, we need to specify the gnu_printf format flavor or GCC
 * will assume non-standard Microsoft printf syntax.
 */
OTD_PRIV void otd_exception_catch(const char *format, ...)
		__attribute__((__format__ (__gnu_printf__, 1, 2)));
#else
OTD_PRIV void otd_exception_catch(const char *format, ...) G_GNUC_PRINTF(1, 2);
#endif

#endif
