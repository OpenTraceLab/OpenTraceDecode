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
 * Check various basic init related things.
 *
 *  - Check whether an otd_init() call with path == NULL works.
 *    If it returns != OTD_OK (or segfaults) this test will fail.
 *
 *  - Check whether a subsequent otd_exit() works.
 *    If it returns != OTD_OK (or segfaults) this test will fail.
 */
START_TEST(test_init_exit)
{
	int ret;

	ret = otd_init(NULL);
	ck_assert_int_eq(ret, OTD_OK);
	ret = otd_exit();
	ck_assert_int_eq(ret, OTD_OK);
}
END_TEST

/*
 * Check whether nested otd_init()/otd_exit() calls work/fail correctly.
 * Two consecutive otd_init() calls without any otd_exit() inbetween are
 * not allowed and should fail. However, two consecutive otd_exit() calls
 * are currently allowed, the second one will just be a NOP basically.
 */
START_TEST(test_init_exit_2)
{
	int ret;

	ret = otd_init(NULL);
	ck_assert(ret == OTD_OK);
	ret = otd_init(NULL);
	ck_assert(ret != OTD_OK);
	ret = otd_exit();
	ck_assert(ret == OTD_OK);
	ret = otd_exit();
	ck_assert(ret == OTD_OK);
}
END_TEST

/*
 * Check whether three nested otd_init()/otd_exit() calls work/fail correctly.
 */
START_TEST(test_init_exit_3)
{
	int ret;

	ret = otd_init(NULL);
	ck_assert(ret == OTD_OK);
	ret = otd_init(NULL);
	ck_assert(ret != OTD_OK);
	ret = otd_init(NULL);
	ck_assert(ret != OTD_OK);
	ret = otd_exit();
	ck_assert(ret == OTD_OK);
	ret = otd_exit();
	ck_assert(ret == OTD_OK);
	ret = otd_exit();
	ck_assert(ret == OTD_OK);
}
END_TEST

Suite *suite_core(void)
{
	Suite *s;
	TCase *tc;

	s = suite_create("core");

	tc = tcase_create("init_exit");
	tcase_add_checked_fixture(tc, srdtest_setup, srdtest_teardown);
	tcase_add_test(tc, test_init_exit);
	tcase_add_test(tc, test_init_exit_2);
	tcase_add_test(tc, test_init_exit_3);
	suite_add_tcase(s, tc);

	return s;
}
