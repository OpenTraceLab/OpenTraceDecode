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
#include <libopentracedecode-internal.h> /* First, to avoid compiler warning. */
#include <libopentracedecode.h>
#include <stdint.h>
#include <stdlib.h>
#include <check.h>
#include "lib.h"

/*
 * Check whether otd_session_new() works.
 * If it returns != OTD_OK (or segfaults) this test will fail.
 */
START_TEST(test_session_new)
{
	int ret;
	struct otd_session *sess;

	otd_init(NULL);
	ret = otd_session_new(&sess);
	ck_assert(ret == OTD_OK);
	otd_exit();
}
END_TEST

/*
 * Check whether otd_session_new() fails for bogus parameters.
 * If it returns OTD_OK (or segfaults) this test will fail.
 */
START_TEST(test_session_new_bogus)
{
	int ret;

	otd_init(NULL);
	ret = otd_session_new(NULL);
	ck_assert(ret != OTD_OK);
	otd_exit();
}
END_TEST

/*
 * Check whether multiple otd_session_new() calls work.
 * If any call returns != OTD_OK (or segfaults) this test will fail.
 */
START_TEST(test_session_new_multiple)
{
	int ret;
	struct otd_session *sess1, *sess2, *sess3;

	sess1 = sess2 = sess3 = NULL;

	otd_init(NULL);

	/* Multiple otd_session_new() calls must work. */
	ret = otd_session_new(&sess1);
	ck_assert(ret == OTD_OK);
	ret = otd_session_new(&sess2);
	ck_assert(ret == OTD_OK);
	ret = otd_session_new(&sess3);
	ck_assert(ret == OTD_OK);

	/* The returned session pointers must all be non-NULL. */
	ck_assert(sess1 != NULL);
	ck_assert(sess2 != NULL);
	ck_assert(sess3 != NULL);

	/* The returned session pointers must not be the same. */
	ck_assert(sess1 != sess2);
	ck_assert(sess1 != sess3);
	ck_assert(sess2 != sess3);

	/* Each session must have another ID than any other session. */
	ck_assert(sess1->session_id != sess2->session_id);
	ck_assert(sess1->session_id != sess3->session_id);
	ck_assert(sess2->session_id != sess3->session_id);

	/* Destroying any of the sessions must work. */
	ret = otd_session_destroy(sess1);
	ck_assert(ret == OTD_OK);
	ret = otd_session_destroy(sess2);
	ck_assert(ret == OTD_OK);
	ret = otd_session_destroy(sess3);
	ck_assert(ret == OTD_OK);

	otd_exit();
}
END_TEST

/*
 * Check whether otd_session_destroy() works.
 * If it returns != OTD_OK (or segfaults) this test will fail.
 */
START_TEST(test_session_destroy)
{
	int ret;
	struct otd_session *sess;

	otd_init(NULL);
	otd_session_new(&sess);
	ret = otd_session_destroy(sess);
	ck_assert(ret == OTD_OK);
	otd_exit();
}
END_TEST

/*
 * Check whether otd_session_destroy() fails for bogus sessions.
 * If it returns OTD_OK (or segfaults) this test will fail.
 */
START_TEST(test_session_destroy_bogus)
{
	int ret;

	otd_init(NULL);
	ret = otd_session_destroy(NULL);
	ck_assert(ret != OTD_OK);
	otd_exit();
}
END_TEST

static void conf_check_ok(struct otd_session *sess, int key, uint64_t x)
{
	int ret;

	ret = otd_session_metadata_set(sess, key, g_variant_new_uint64(x));
	ck_assert(ret == OTD_OK, "otd_session_metadata_set(%p, %d, %"
		PRIu64 ") failed: %d.", sess, key, x, ret);
}

static void conf_check_fail(struct otd_session *sess, int key, uint64_t x)
{
	int ret;
	GVariant *value = g_variant_new_uint64(x);

	ret = otd_session_metadata_set(sess, key, value);
	ck_assert(ret != OTD_OK, "otd_session_metadata_set(%p, %d, %"
		PRIu64 ") worked.", sess, key, x);
	if (ret != OTD_OK)
		g_variant_unref(value);
}

static void conf_check_fail_null(struct otd_session *sess, int key)
{
	int ret;

	ret = otd_session_metadata_set(sess, key, NULL);
	ck_assert(ret != OTD_OK,
		"otd_session_metadata_set(NULL) for key %d worked.", key);
}

static void conf_check_fail_str(struct otd_session *sess, int key, const char *s)
{
	int ret;
	GVariant *value = g_variant_new_string(s);

	ret = otd_session_metadata_set(sess, key, value);
	ck_assert(ret != OTD_OK, "otd_session_metadata_set() for key %d "
		"failed: %d.", key, ret);
	if (ret != OTD_OK)
		g_variant_unref(value);
}

/*
 * Check whether otd_session_metadata_set() works.
 * If it returns != OTD_OK (or segfaults) this test will fail.
 */
START_TEST(test_session_metadata_set)
{
	uint64_t i;
	struct otd_session *sess;

	otd_init(NULL);
	otd_session_new(&sess);
	/* Try a bunch of values. */
	for (i = 0; i < 1000; i++)
		conf_check_ok(sess, OTD_CONF_SAMPLERATE, i);
	/* Try the max. possible value. */
	conf_check_ok(sess, OTD_CONF_SAMPLERATE, UINT64_MAX);
	otd_session_destroy(sess);
	otd_exit();
}
END_TEST

/*
 * Check whether otd_session_metadata_set() fails with invalid input.
 * If it returns OTD_OK (or segfaults) this test will fail.
 */
START_TEST(test_session_metadata_set_bogus)
{
	struct otd_session *sess;

	otd_init(NULL);
	otd_session_new(&sess);

	/* Incorrect GVariant type (currently only uint64 is used). */
	conf_check_fail_str(sess, OTD_CONF_SAMPLERATE, "");
	conf_check_fail_str(sess, OTD_CONF_SAMPLERATE, "Foo");

	/* NULL data pointer. */
	conf_check_fail_null(sess, OTD_CONF_SAMPLERATE);

	/* NULL session. */
	conf_check_fail(NULL, OTD_CONF_SAMPLERATE, 0);

	/* Invalid keys. */
	conf_check_fail(sess, -1, 0);
	conf_check_fail(sess, 9, 0);
	conf_check_fail(sess, 123, 0);

	otd_session_destroy(sess);
	otd_exit();
}
END_TEST

/*
 * Check whether otd_session_terminate_reset() succeeds on newly created
 * sessions, as well as after calling start() and meta(). No data is fed
 * to decoders here.
 */
START_TEST(test_session_reset_nodata)
{
	struct otd_session *sess;
	int ret;
	GVariant *data;

	otd_init(NULL);
	otd_session_new(&sess);
	ret = otd_session_terminate_reset(sess);
	ck_assert(ret == OTD_OK);
	ret = otd_session_start(sess);
	ck_assert(ret == OTD_OK);
	ret = otd_session_terminate_reset(sess);
	ck_assert(ret == OTD_OK);
	data = g_variant_new_uint64(1000000);
	ret = otd_session_metadata_set(sess, OTD_CONF_SAMPLERATE, data);
	ck_assert(ret == OTD_OK);
	ret = otd_session_terminate_reset(sess);
	ck_assert(ret == OTD_OK);
	ret = otd_session_destroy(sess);
	ck_assert(ret == OTD_OK);
	otd_exit();
}
END_TEST

Suite *suite_session(void)
{
	Suite *s;
	TCase *tc;

	s = suite_create("session");

	tc = tcase_create("new_destroy");
	tcase_add_checked_fixture(tc, srdtest_setup, srdtest_teardown);
	tcase_add_test(tc, test_session_new);
	tcase_add_test(tc, test_session_new_bogus);
	tcase_add_test(tc, test_session_new_multiple);
	tcase_add_test(tc, test_session_destroy);
	tcase_add_test(tc, test_session_destroy_bogus);
	suite_add_tcase(s, tc);

	tc = tcase_create("config");
	tcase_add_checked_fixture(tc, srdtest_setup, srdtest_teardown);
	tcase_add_test(tc, test_session_metadata_set);
	tcase_add_test(tc, test_session_metadata_set_bogus);
	suite_add_tcase(s, tc);

	tc = tcase_create("reset");
	tcase_add_test(tc, test_session_reset_nodata);
	suite_add_tcase(s, tc);

	return s;
}
