/*
 * This file is part of the libopentracedecode project.
 *
 * Copyright (C) 2010 Uwe Hermann <uwe@hermann-uwe.de>
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

#ifndef OPENTRACEDECODE_LIBOPENTRACEDECODE_H
#define OPENTRACEDECODE_LIBOPENTRACEDECODE_H

#include <stdint.h>
#include <glib.h>

#ifdef __cplusplus
extern "C" {
#endif

struct otd_session;

/**
 * @file
 *
 * The public libopentracedecode header file to be used by frontends.
 *
 * This is the only file that libopentracedecode users (frontends) are supposed
 * to use and include. There are other header files which get installed with
 * libopentracedecode, but those are not meant to be used directly by frontends.
 *
 * The correct way to get/use the libopentracedecode API functions is:
 *
 * @code{.c}
 *   #include <libopentracedecode/libopentracedecode.h>
 * @endcode
 */

/*
 * All possible return codes of libopentracedecode functions must be listed here.
 * Functions should never return hardcoded numbers as status, but rather
 * use these enum values. All error codes are negative numbers.
 *
 * The error codes are globally unique in libopentracedecode, i.e. if one of the
 * libopentracedecode functions returns a "malloc error" it must be exactly the
 * same return value as used by all other functions to indicate "malloc error".
 * There must be no functions which indicate two different errors via the
 * same return code.
 *
 * Also, for compatibility reasons, no defined return codes are ever removed
 * or reused for different errors later. You can only add new entries and
 * return codes, but never remove or redefine existing ones.
 */

/** Status/error codes returned by libopentracedecode functions. */
enum otd_error_code {
	OTD_OK               =  0, /**< No error */
	OTD_ERR              = -1, /**< Generic/unspecified error */
	OTD_ERR_MALLOC       = -2, /**< Malloc/calloc/realloc error */
	OTD_ERR_ARG          = -3, /**< Function argument error */
	OTD_ERR_BUG          = -4, /**< Errors hinting at internal bugs */
	OTD_ERR_PYTHON       = -5, /**< Python C API error */
	OTD_ERR_DECODERS_DIR = -6, /**< Protocol decoder path invalid */
	OTD_ERR_TERM_REQ     = -7, /**< Termination requested */

	/*
	 * Note: When adding entries here, don't forget to also update the
	 * otd_strerror() and otd_strerror_name() functions in error.c.
	 */
};

/* libopentracedecode loglevels. */
enum otd_loglevel {
	OTD_LOG_NONE = 0, /**< Output no messages at all. */
	OTD_LOG_ERR  = 1, /**< Output error messages. */
	OTD_LOG_WARN = 2, /**< Output warnings. */
	OTD_LOG_INFO = 3, /**< Output informational messages. */
	OTD_LOG_DBG  = 4, /**< Output debug messages. */
	OTD_LOG_SPEW = 5, /**< Output very noisy debug messages. */
};

/*
 * Use OTD_API to mark public API symbols, and OTD_PRIV for private symbols.
 *
 * Variables and functions marked 'static' are private already and don't
 * need OTD_PRIV. However, functions which are not static (because they need
 * to be used in other libopentracedecode-internal files) but are also not
 * meant to be part of the public libopentracedecode API, must use OTD_PRIV.
 *
 * This uses the 'visibility' feature of gcc (requires gcc >= 4.0).
 *
 * This feature is not available on MinGW/Windows, as it is a feature of
 * ELF files and MinGW/Windows uses PE files.
 *
 * Details: http://gcc.gnu.org/wiki/Visibility
 */

/* Marks public libopentracedecode API symbols. */
#if defined _WIN32
#  if defined DLL_EXPORT
#    define OTD_API __declspec(dllexport)
#  else
#    define OTD_API extern
#  endif
#else
#  define OTD_API __attribute__((visibility("default")))
#endif

/* Marks private, non-public libopentracedecode symbols (not part of the API). */
#if defined _WIN32
#  define OTD_PRIV /* EMPTY */
#else
#  define OTD_PRIV __attribute__((visibility("hidden")))
#endif

/*
 * When adding an output type, don't forget to...
 *   - expose it to PDs in module_opentracedecode.c:PyInit_sigrokdecode()
 *   - add a check in type_decoder.c:Decoder_put()
 *   - add a debug string in type_decoder.c:output_type_name()
 */
enum otd_output_type {
	OTD_OUTPUT_ANN,
	OTD_OUTPUT_PYTHON,
	OTD_OUTPUT_BINARY,
	OTD_OUTPUT_LOGIC,
	OTD_OUTPUT_META,
};

enum otd_configkey {
	OTD_CONF_SAMPLERATE = 10000,
};

struct otd_decoder {
	/** The decoder ID. Must be non-NULL and unique for all decoders. */
	char *id;

	/** The (short) decoder name. Must be non-NULL. */
	char *name;

	/** The (long) decoder name. Must be non-NULL. */
	char *longname;

	/** A (short, one-line) description of the decoder. Must be non-NULL. */
	char *desc;

	/**
	 * The license of the decoder. Valid values: "gplv2+", "gplv3+".
	 * Other values are currently not allowed. Must be non-NULL.
	 */
	char *license;

	/** List of possible decoder input IDs. */
	GSList *inputs;

	/** List of possible decoder output IDs. */
	GSList *outputs;

	/** List of tags associated with this decoder. */
	GSList *tags;

	/** List of channels required by this decoder. */
	GSList *channels;

	/** List of optional channels for this decoder. */
	GSList *opt_channels;

	/**
	 * List of annotation classes. Each list item is a GSList itself, with
	 * two NUL-terminated strings: name and description.
	 */
	GSList *annotations;

	/**
	 * List of annotation rows (row items: id, description, and a list
	 * of annotation classes belonging to this row).
	 */
	GSList *annotation_rows;

	/**
	 * List of binary classes. Each list item is a GSList itself, with
	 * two NUL-terminated strings: name and description.
	 */
	GSList *binary;

	/**
	 * List of logic output channels (item: id, description).
	 */
	GSList *logic_output_channels;

	/** List of decoder options. */
	GSList *options;

	/** Python module. */
	void *py_mod;

	/** opentracedecode.Decoder class. */
	void *py_dec;
};

enum otd_initial_pin {
	OTD_INITIAL_PIN_LOW,
	OTD_INITIAL_PIN_HIGH,
	OTD_INITIAL_PIN_SAME_AS_SAMPLE0,
};

/**
 * Structure which contains information about one protocol decoder channel.
 * For example, I2C has two channels, SDA and SCL.
 */
struct otd_channel {
	/** The ID of the channel. Must be non-NULL. */
	char *id;
	/** The name of the channel. Must not be NULL. */
	char *name;
	/** The description of the channel. Must not be NULL. */
	char *desc;
	/** The index of the channel, i.e. its order in the list of channels. */
	int order;
};

struct otd_decoder_option {
	char *id;
	char *desc;
	GVariant *def;
	GSList *values;
};

struct otd_decoder_annotation_row {
	char *id;
	char *desc;
	GSList *ann_classes;
};

struct otd_decoder_logic_output_channel {
	char *id;
	char *desc;
};

struct otd_decoder_inst {
	struct otd_decoder *decoder;
	struct otd_session *sess;
	void *py_inst;
	char *inst_id;
	GSList *pd_output;
	int dec_num_channels;
	int *dec_channelmap;
	int data_unitsize;
	uint8_t *channel_samples;
	GSList *next_di;

	/** List of conditions a PD wants to wait for. */
	GSList *condition_list;

	/** Array of booleans denoting which conditions matched. */
	GArray *match_array;

	/** Absolute start sample number. */
	uint64_t abs_start_samplenum;

	/** Absolute end sample number. */
	uint64_t abs_end_samplenum;

	/** Pointer to the buffer/chunk of input samples. */
	const uint8_t *inbuf;

	/** Length (in bytes) of the input sample buffer. */
	uint64_t inbuflen;

	/** Absolute current samplenumber. */
	uint64_t abs_cur_samplenum;

	/** Array of "old" (previous sample) pin values. */
	GArray *old_pins_array;

	/** Handle for this PD stack's worker thread. */
	GThread *thread_handle;

	/** Indicates whether new samples are available for processing. */
	gboolean got_new_samples;

	/** Indicates whether the worker thread has handled all samples. */
	gboolean handled_all_samples;

	/** Requests termination of wait() and decode(). */
	gboolean want_wait_terminate;

	/** Requests that .wait() terminates a Python iteration. */
	gboolean communicate_eof;

	/** Indicates the current state of the decoder stack. */
	int decoder_state;

	GCond got_new_samples_cond;
	GCond handled_all_samples_cond;
	GMutex data_mutex;
};

struct otd_pd_output {
	int pdo_id;
	int output_type;
	struct otd_decoder_inst *di;
	char *proto_id;
	/* Only used for OUTPUT_META. */
	const GVariantType *meta_type;
	char *meta_name;
	char *meta_descr;
};

struct otd_proto_data {
	uint64_t start_sample;
	uint64_t end_sample;
	struct otd_pd_output *pdo;
	void *data;
};
struct otd_proto_data_annotation {
	int ann_class; /* Index into "struct otd_decoder"->annotations. */
	char **ann_text;
};
struct otd_proto_data_binary {
	int bin_class; /* Index into "struct otd_decoder"->binary. */
	uint64_t size;
	const uint8_t *data;
};
struct otd_proto_data_logic {
	int logic_group;
	uint64_t repeat_count; /* Number of times the value in data was repeated. */
	const uint8_t *data; /* Bitfield containing the states of the logic outputs */
};

typedef void (*otd_pd_output_callback)(struct otd_proto_data *pdata,
					void *cb_data);

struct otd_pd_callback {
	int output_type;
	otd_pd_output_callback cb;
	void *cb_data;
};

/* srd.c */
OTD_API int otd_init(const char *path);
OTD_API int otd_exit(void);
OTD_API GSList *otd_searchpaths_get(void);

/* session.c */
OTD_API int otd_session_new(struct otd_session **sess);
OTD_API int otd_session_start(struct otd_session *sess);
OTD_API int otd_session_metadata_set(struct otd_session *sess, int key,
		GVariant *data);
OTD_API int otd_session_send(struct otd_session *sess,
		uint64_t abs_start_samplenum, uint64_t abs_end_samplenum,
		const uint8_t *inbuf, uint64_t inbuflen, uint64_t unitsize);
OTD_API int otd_session_send_eof(struct otd_session *sess);
OTD_API int otd_session_terminate_reset(struct otd_session *sess);
OTD_API int otd_session_destroy(struct otd_session *sess);
OTD_API int otd_pd_output_callback_add(struct otd_session *sess,
		int output_type, otd_pd_output_callback cb, void *cb_data);

/* decoder.c */
OTD_API const GSList *otd_decoder_list(void);
OTD_API struct otd_decoder *otd_decoder_get_by_id(const char *id);
OTD_API int otd_decoder_load(const char *name);
OTD_API char *otd_decoder_doc_get(const struct otd_decoder *dec);
OTD_API int otd_decoder_unload(struct otd_decoder *dec);
OTD_API int otd_decoder_load_all(void);
OTD_API int otd_decoder_unload_all(void);

/* instance.c */
OTD_API int otd_inst_option_set(struct otd_decoder_inst *di,
		GHashTable *options);
OTD_API int otd_inst_channel_set_all(struct otd_decoder_inst *di,
		GHashTable *channels);
OTD_API struct otd_decoder_inst *otd_inst_new(struct otd_session *sess,
		const char *id, GHashTable *options);
OTD_API int otd_inst_stack(struct otd_session *sess,
		struct otd_decoder_inst *di_from, struct otd_decoder_inst *di_to);
OTD_API struct otd_decoder_inst *otd_inst_find_by_id(struct otd_session *sess,
		const char *inst_id);
OTD_API int otd_inst_initial_pins_set_all(struct otd_decoder_inst *di,
		GArray *initial_pins);

/* log.c */
typedef int (*otd_log_callback)(void *cb_data, int loglevel,
				  const char *format, va_list args);
OTD_API int otd_log_loglevel_set(int loglevel);
OTD_API int otd_log_loglevel_get(void);
OTD_API int otd_log_callback_get(otd_log_callback *cb, void **cb_data);
OTD_API int otd_log_callback_set(otd_log_callback cb, void *cb_data);
OTD_API int otd_log_callback_set_default(void);

/* error.c */
OTD_API const char *otd_strerror(int error_code);
OTD_API const char *otd_strerror_name(int error_code);

/* version.c */
OTD_API int otd_package_version_major_get(void);
OTD_API int otd_package_version_minor_get(void);
OTD_API int otd_package_version_micro_get(void);
OTD_API const char *otd_package_version_string_get(void);
OTD_API int otd_lib_version_current_get(void);
OTD_API int otd_lib_version_revision_get(void);
OTD_API int otd_lib_version_age_get(void);
OTD_API const char *otd_lib_version_string_get(void);
OTD_API GSList *otd_buildinfo_libs_get(void);
OTD_API char *otd_buildinfo_host_get(void);

#include "version.h"

#ifdef __cplusplus
}
#endif

#endif
