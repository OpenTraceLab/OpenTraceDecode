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
	fail_unless(ret == OTD_OK, "otd_init() failed: %d.", ret);
	ret = otd_exit();
	fail_unless(ret == OTD_OK, "otd_exit() failed: %d.", ret);
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
	fail_unless(ret == OTD_OK, "otd_init() 1 failed: %d.", ret);
	ret = otd_init(NULL);
	fail_unless(ret != OTD_OK, "otd_init() 2 didn't fail: %d.", ret);
	ret = otd_exit();
	fail_unless(ret == OTD_OK, "otd_exit() 2 failed: %d.", ret);
	ret = otd_exit();
	fail_unless(ret == OTD_OK, "otd_exit() 1 failed: %d.", ret);
}
END_TEST

/*
 * Check whether three nested otd_init()/otd_exit() calls work/fail correctly.
 */
START_TEST(test_init_exit_3)
{
	int ret;

	ret = otd_init(NULL);
	fail_unless(ret == OTD_OK, "otd_init() 1 failed: %d.", ret);
	ret = otd_init(NULL);
	fail_unless(ret != OTD_OK, "otd_init() 2 didn't fail: %d.", ret);
	ret = otd_init(NULL);
	fail_unless(ret != OTD_OK, "otd_init() 3 didn't fail: %d.", ret);
	ret = otd_exit();
	fail_unless(ret == OTD_OK, "otd_exit() 3 failed: %d.", ret);
	ret = otd_exit();
	fail_unless(ret == OTD_OK, "otd_exit() 2 failed: %d.", ret);
	ret = otd_exit();
	fail_unless(ret == OTD_OK, "otd_exit() 1 failed: %d.", ret);
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
