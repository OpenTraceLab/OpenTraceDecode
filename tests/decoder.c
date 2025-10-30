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
#include <libopentracedecode.h> /* First, to avoid compiler warning. */
#include <stdlib.h>
#include <check.h>
#include "lib.h"

/*
 * Check whether otd_decoder_load_all() works.
 * If it returns != OTD_OK (or segfaults) this test will fail.
 */
START_TEST(test_load_all)
{
	int ret;

	otd_init(DECODERS_TESTDIR);
	ret = otd_decoder_load_all();
	ck_assert(ret == OTD_OK);
	otd_exit();
}
END_TEST

/*
 * Check whether otd_decoder_load_all() fails without prior otd_init().
 * If it returns != OTD_OK (or segfaults) this test will fail.
 * See also: http://sigrok.org/bugzilla/show_bug.cgi?id=178
 */
START_TEST(test_load_all_no_init)
{
	int ret;

	ret = otd_decoder_load_all();
	ck_assert(ret != OTD_OK);
}
END_TEST

/*
 * Check whether otd_decoder_load() works.
 * If it returns != OTD_OK (or segfaults) this test will fail.
 */
START_TEST(test_load)
{
	int ret;

	otd_init(DECODERS_TESTDIR);
	ret = otd_decoder_load("uart");
	ck_assert(ret == OTD_OK);
	ret = otd_decoder_load("spi");
	ck_assert(ret == OTD_OK);
	ret = otd_decoder_load("usb_signalling");
	ck_assert(ret == OTD_OK);
	otd_exit();
}
END_TEST

/*
 * Check whether otd_decoder_load() fails for non-existing or bogus PDs.
 * If it returns OTD_OK (or segfaults) this test will fail.
 */
START_TEST(test_load_bogus)
{
	otd_init(DECODERS_TESTDIR);
	/* http://sigrok.org/bugzilla/show_bug.cgi?id=176 */
	ck_assert(otd_decoder_load(NULL) != OTD_OK);
	ck_assert(otd_decoder_load("") != OTD_OK);
	ck_assert(otd_decoder_load(" ") != OTD_OK);
	ck_assert(otd_decoder_load("nonexisting") != OTD_OK);
	ck_assert(otd_decoder_load("UART") != OTD_OK);
	ck_assert(otd_decoder_load("UaRt") != OTD_OK);
	ck_assert(otd_decoder_load("u a r t") != OTD_OK);
	ck_assert(otd_decoder_load("uart ") != OTD_OK);
	ck_assert(otd_decoder_load(" uart") != OTD_OK);
	ck_assert(otd_decoder_load(" uart ") != OTD_OK);
	ck_assert(otd_decoder_load("uart spi") != OTD_OK);
	otd_exit();
}
END_TEST

/*
 * Check whether otd_decoder_load() works/fails for valid/bogus PDs.
 * If it returns incorrect values (or segfaults) this test will fail.
 */
START_TEST(test_load_valid_and_bogus)
{
	otd_init(DECODERS_TESTDIR);
	ck_assert(otd_decoder_load("") != OTD_OK);
	ck_assert(otd_decoder_load("uart") == OTD_OK);
	ck_assert(otd_decoder_load("") != OTD_OK);
	ck_assert(otd_decoder_load("spi") == OTD_OK);
	ck_assert(otd_decoder_load("") != OTD_OK);
	ck_assert(otd_decoder_load("can") == OTD_OK);
	ck_assert(otd_decoder_load("") != OTD_OK);
	otd_exit();
}
END_TEST

/*
 * Check whether otd_decoder_load() fails when run multiple times.
 * If it returns a value != OTD_OK (or segfaults) this test will fail.
 * See also: http://sigrok.org/bugzilla/show_bug.cgi?id=177
 */
START_TEST(test_load_multiple)
{
	int ret;

	otd_init(DECODERS_TESTDIR);
	ret = otd_decoder_load("uart");
	ck_assert(ret == OTD_OK);
	ret = otd_decoder_load("uart");
	ck_assert(ret == OTD_OK);
	ret = otd_decoder_load("uart");
	ck_assert(ret == OTD_OK);
	otd_exit();
}
END_TEST

/*
 * Check whether otd_decoder_load() fails if a non-existing PD dir is used.
 * If it returns OTD_OK (or segfaults) this test will fail.
 */
START_TEST(test_load_nonexisting_pd_dir)
{
#if 0
	/* TODO: Build libopentracedecode with no default PD dir. */
	otd_init("/nonexisting_dir");
	ck_assert(otd_decoder_load("spi") != OTD_OK);
	ck_assert(g_slist_length((GSList *)otd_decoder_list()) == 0);
	otd_exit();
#endif
}
END_TEST

/*
 * Check whether otd_decoder_unload_all() works.
 * If it returns != OTD_OK (or segfaults) this test will fail.
 */
START_TEST(test_unload_all)
{
	int ret;

	otd_init(DECODERS_TESTDIR);
	ret = otd_decoder_load_all();
	ck_assert(ret == OTD_OK);
	ret = otd_decoder_unload_all();
	ck_assert(ret == OTD_OK);
	otd_exit();
}
END_TEST

/*
 * Check whether otd_decoder_unload_all() works without prior otd_init().
 * If it returns != OTD_OK (or segfaults) this test will fail.
 */
START_TEST(test_unload_all_no_init)
{
	int ret;

	ret = otd_decoder_unload_all();
	ck_assert(ret == OTD_OK);
}
END_TEST

/*
 * Check whether otd_decoder_unload_all() works multiple times.
 * If it returns != OTD_OK (or segfaults) this test will fail.
 */
START_TEST(test_unload_all_multiple)
{
	int ret, i;

	otd_init(DECODERS_TESTDIR);
	for (i = 0; i < 10; i++) {
		ret = otd_decoder_load_all();
		ck_assert(ret == OTD_OK);
		ret = otd_decoder_unload_all();
		ck_assert(ret == OTD_OK);
	}
	otd_exit();
}
END_TEST

/*
 * Check whether otd_decoder_unload_all() works multiple times (no load).
 * If it returns != OTD_OK (or segfaults) this test will fail.
 */
START_TEST(test_unload_all_multiple_noload)
{
	int ret, i;

	otd_init(DECODERS_TESTDIR);
	for (i = 0; i < 10; i++) {
		ret = otd_decoder_unload_all();
		ck_assert(ret == OTD_OK);
	}
	otd_exit();
}
END_TEST

/*
 * Check whether otd_decoder_unload() works.
 * If it returns != OTD_OK (or segfaults) this test will fail.
 */
START_TEST(test_unload)
{
	int ret;
	struct otd_decoder *dec;

	otd_init(DECODERS_TESTDIR);
	ret = otd_decoder_load("uart");
	ck_assert(ret == OTD_OK);
	dec = otd_decoder_get_by_id("uart");
	ck_assert(dec != NULL);
	ret = otd_decoder_unload(dec);
	ck_assert(ret == OTD_OK);
	otd_exit();
}
END_TEST

/*
 * Check whether otd_decoder_unload(NULL) fails.
 * If it returns OTD_OK (or segfaults) this test will fail.
 */
START_TEST(test_unload_null)
{
	otd_init(DECODERS_TESTDIR);
	ck_assert(otd_decoder_unload(NULL) != OTD_OK);
	otd_exit();
}
END_TEST

/*
 * Check whether otd_decoder_unload(NULL) fails without prior otd_init().
 * If it returns OTD_OK (or segfaults) this test will fail.
 */
START_TEST(test_unload_null_no_init)
{
	ck_assert(otd_decoder_unload(NULL) != OTD_OK);
}
END_TEST

/*
 * Check whether otd_decoder_list() returns a non-empty list.
 * If it returns an empty list (or segfaults) this test will fail.
 */
START_TEST(test_decoder_list)
{
	otd_init(DECODERS_TESTDIR);
	otd_decoder_load_all();
	ck_assert(otd_decoder_list() != NULL);
	otd_exit();
}
END_TEST

/*
 * Check whether otd_decoder_list() without prior otd_decoder_load_all()
 * returns an empty list (return value != NULL).
 * If it returns a non-empty list (or segfaults) this test will fail.
 */
START_TEST(test_decoder_list_no_load)
{
	otd_init(DECODERS_TESTDIR);
	ck_assert(otd_decoder_list() == NULL);
	otd_exit();
}
END_TEST

/*
 * Check whether otd_decoder_list() without prior otd_init()
 * returns an empty list.
 * If it returns a non-empty list (or segfaults) this test will fail.
 * See also: http://sigrok.org/bugzilla/show_bug.cgi?id=178
 */
START_TEST(test_decoder_list_no_init)
{
	otd_decoder_load_all();
	ck_assert(otd_decoder_list() == NULL);
}
END_TEST

/*
 * Check whether otd_decoder_list() without prior otd_init() and without
 * prior otd_decoder_load_all() returns an empty list.
 * If it returns a non-empty list (or segfaults) this test will fail.
 */
START_TEST(test_decoder_list_no_init_no_load)
{
	ck_assert(otd_decoder_list() == NULL);
}
END_TEST

/*
 * Check whether otd_decoder_list() returns the correct number of PDs.
 * If it returns a wrong number (or segfaults) this test will fail.
 */
START_TEST(test_decoder_list_correct_numbers)
{
	otd_init(DECODERS_TESTDIR);
	ck_assert(g_slist_length((GSList *)otd_decoder_list()) == 0);
	otd_decoder_load("spi");
	ck_assert(g_slist_length((GSList *)otd_decoder_list()) == 1);
	otd_decoder_load("uart");
	ck_assert(g_slist_length((GSList *)otd_decoder_list()) == 2);
	otd_decoder_load("can");
	ck_assert(g_slist_length((GSList *)otd_decoder_list()) == 3);
	otd_decoder_load("can"); /* Load same PD twice. */
	ck_assert(g_slist_length((GSList *)otd_decoder_list()) == 3);
	otd_exit();
}
END_TEST

/*
 * Check whether otd_decoder_get_by_id() works.
 * If it returns NULL for valid PDs (or segfaults) this test will fail.
 */
START_TEST(test_get_by_id)
{
	otd_init(DECODERS_TESTDIR);
	otd_decoder_load("uart");
	ck_assert(otd_decoder_get_by_id("uart") != NULL);
	ck_assert(otd_decoder_get_by_id("can") == NULL);
	otd_decoder_load("can");
	ck_assert(otd_decoder_get_by_id("uart") != NULL);
	ck_assert(otd_decoder_get_by_id("can") != NULL);
	otd_exit();
}
END_TEST

/*
 * Check whether otd_decoder_get_by_id() works multiple times in a row.
 * If it returns NULL for valid PDs (or segfaults) this test will fail.
 */
START_TEST(test_get_by_id_multiple)
{
	otd_init(DECODERS_TESTDIR);
	otd_decoder_load("uart");
	ck_assert(otd_decoder_get_by_id("uart") != NULL);
	ck_assert(otd_decoder_get_by_id("uart") != NULL);
	ck_assert(otd_decoder_get_by_id("uart") != NULL);
	ck_assert(otd_decoder_get_by_id("uart") != NULL);
	otd_exit();
}
END_TEST

/*
 * Check whether otd_decoder_get_by_id() fails for bogus PDs.
 * If it returns a value != NULL (or segfaults) this test will fail.
 */
START_TEST(test_get_by_id_bogus)
{
	otd_init(DECODERS_TESTDIR);
	ck_assert(otd_decoder_get_by_id(NULL) == NULL);
	ck_assert(otd_decoder_get_by_id("") == NULL);
	ck_assert(otd_decoder_get_by_id(" ") == NULL);
	ck_assert(otd_decoder_get_by_id("nonexisting") == NULL);
	ck_assert(otd_decoder_get_by_id("sPi") == NULL);
	ck_assert(otd_decoder_get_by_id("SPI") == NULL);
	ck_assert(otd_decoder_get_by_id("s p i") == NULL);
	ck_assert(otd_decoder_get_by_id(" spi") == NULL);
	ck_assert(otd_decoder_get_by_id("spi ") == NULL);
	ck_assert(otd_decoder_get_by_id(" spi ") == NULL);
	ck_assert(otd_decoder_get_by_id("spi uart") == NULL);
	otd_exit();
}
END_TEST

/*
 * Check whether otd_decoder_doc_get() works.
 * If it returns NULL for valid PDs (or segfaults) this test will fail.
 */
START_TEST(test_doc_get)
{
	struct otd_decoder *dec;
	char *doc;

	otd_init(DECODERS_TESTDIR);
	otd_decoder_load("uart");
	dec = otd_decoder_get_by_id("uart");
	doc = otd_decoder_doc_get(dec);
	ck_assert(doc != NULL);
	g_free(doc);
	otd_exit();
}
END_TEST

/*
 * Check whether otd_decoder_doc_get() fails with NULL as argument.
 * If it returns a value != NULL (or segfaults) this test will fail.
 * See also: http://sigrok.org/bugzilla/show_bug.cgi?id=179
 * Check whether otd_decoder_doc_get() fails with dec->py_mod == NULL.
 * If it returns a value != NULL (or segfaults) this test will fail.
 * See also: http://sigrok.org/bugzilla/show_bug.cgi?id=180
 */
START_TEST(test_doc_get_null)
{
	struct otd_decoder dec;

	dec.py_mod = NULL;

	otd_init(DECODERS_TESTDIR);
	ck_assert(otd_decoder_doc_get(NULL) == NULL);
	ck_assert(otd_decoder_doc_get(&dec) == NULL);
	otd_exit();
}
END_TEST

Suite *suite_decoder(void)
{
	Suite *s;
	TCase *tc;

	s = suite_create("decoder");

	tc = tcase_create("load");
	tcase_set_timeout(tc, 0);
	tcase_add_checked_fixture(tc, srdtest_setup, srdtest_teardown);
	tcase_add_test(tc, test_load_all);
	tcase_add_test(tc, test_load_all_no_init);
	tcase_add_test(tc, test_load);
	tcase_add_test(tc, test_load_bogus);
	tcase_add_test(tc, test_load_valid_and_bogus);
	tcase_add_test(tc, test_load_multiple);
	tcase_add_test(tc, test_load_nonexisting_pd_dir);
	suite_add_tcase(s, tc);

	tc = tcase_create("unload");
	tcase_add_checked_fixture(tc, srdtest_setup, srdtest_teardown);
	tcase_add_test(tc, test_unload_all);
	tcase_add_test(tc, test_unload_all_no_init);
	tcase_add_test(tc, test_unload_all_multiple);
	tcase_add_test(tc, test_unload_all_multiple_noload);
	tcase_add_test(tc, test_unload);
	tcase_add_test(tc, test_unload_null);
	tcase_add_test(tc, test_unload_null_no_init);
	suite_add_tcase(s, tc);

	tc = tcase_create("list");
	tcase_add_checked_fixture(tc, srdtest_setup, srdtest_teardown);
	tcase_add_test(tc, test_decoder_list);
	tcase_add_test(tc, test_decoder_list_no_load);
	tcase_add_test(tc, test_decoder_list_no_init);
	tcase_add_test(tc, test_decoder_list_no_init_no_load);
	tcase_add_test(tc, test_decoder_list_correct_numbers);
	suite_add_tcase(s, tc);

	tc = tcase_create("get_by_id");
	tcase_add_test(tc, test_get_by_id);
	tcase_add_test(tc, test_get_by_id_multiple);
	tcase_add_test(tc, test_get_by_id_bogus);
	suite_add_tcase(s, tc);

	tc = tcase_create("doc_get");
	tcase_add_test(tc, test_doc_get);
	tcase_add_test(tc, test_doc_get_null);
	suite_add_tcase(s, tc);

	return s;
}
