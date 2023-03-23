/*
 * sideRETRO - A pipeline for detecting Somatic Insertion of DE novo RETROcopies
 * Copyright (C) 2019-2020 Thiago L. A. Miller <tmiller@mochsl.org.br
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "config.h"

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <signal.h>
#include <check.h>
#include "check_sider.h"

#include "../src/wrapper.h"
#include "../src/log.h"
#include "../src/utils.h"

START_TEST (test_fequal)
{
	double a, b;

	a = -0.0000000000001;
	b = -0.0000000000001;

	ck_assert_int_eq (fequal (a, b), 1);

	a = 0.0;
	b = 0.0;

	ck_assert_int_eq (fequal (a, b), 1);

	a = -0.000000000001;
	b = -0.0000000000001;

	ck_assert_int_eq (fequal (a, b), 0);
}
END_TEST

START_TEST (test_equalstring)
{
	const char str1[] = "string test 1";
	const char str2[] = "string test 2";
	const char str3[] = "string test 1";

	ck_assert_int_eq (equalstring (str1, str2), 0);
	ck_assert_int_eq (equalstring (str1, str3), 1);
}
END_TEST

START_TEST (test_casequalstring)
{
	const char str1[] = "STRING TEST 1";
	const char str2[] = "string test 2";
	const char str3[] = "string test 1";

	ck_assert_int_eq (casequalstring (str1, str2), 0);
	ck_assert_int_eq (casequalstring (str1, str3), 1);
}
END_TEST

START_TEST (test_cmpstringp)
{
	const char *str1 = "alpha";
	const char *str2 = "beta";
	const char *str3 = "gamma";

	ck_assert_int_lt (cmpstringp (&str1, &str2), 0);
	ck_assert_int_gt (cmpstringp (&str3, &str2), 0);
	ck_assert_int_eq (cmpstringp (&str2, &str2), 0);
}
END_TEST

START_TEST (test_casecmpstringp)
{
	const char *str1 = "ALPHA";
	const char *str2 = "alpha";
	const char *str3 = "GAMMA";

	ck_assert_int_eq (casecmpstringp (&str1, &str2), 0);
	ck_assert_int_gt (casecmpstringp (&str3, &str2), 0);
}
END_TEST

START_TEST (test_chomp)
{
	char str[] = "PONGA";
	char str_with_new_line[] = "PONGA\n";

	chomp (str);
	chomp (str_with_new_line);

	ck_assert_str_eq (str, str_with_new_line);
	ck_assert_str_eq (str, "PONGA");
}
END_TEST

START_TEST (test_trimc)
{
	char str_left_padding[] = "###PONGA";
	char str_right_padding[] = "PONGA###";
	char str_left_right_padding[] = "####PONGA#####";

	trimc (str_left_padding, '#');
	trimc (str_right_padding, '#');
	trimc (str_left_right_padding, '#');

	ck_assert_str_eq (str_left_padding, str_right_padding);
	ck_assert_str_eq (str_left_padding, str_left_right_padding);
	ck_assert_str_eq (str_left_padding, "PONGA");
}
END_TEST

START_TEST (test_trim)
{
	char str_left_padding[] = "   PONGA";
	char str_right_padding[] = "PONGA   ";
	char str_left_right_padding[] = "   PONGA   ";

	trim (str_left_padding);
	trim (str_right_padding);
	trim (str_left_right_padding);

	ck_assert_str_eq (str_left_padding, str_right_padding);
	ck_assert_str_eq (str_left_padding, str_left_right_padding);
	ck_assert_str_eq (str_left_padding, "PONGA");
}
END_TEST

START_TEST (test_upper)
{
	char str[] = "ponga";
	ck_assert_str_eq (upper (str), "PONGA");
}
END_TEST

START_TEST (test_lower)
{
	char str[] = "PONGA";
	ck_assert_str_eq (lower (str), "ponga");
}
END_TEST

START_TEST (test_path_dir)
{
	char path[] = "/home/ponga/ponga.txt";
	char *path_copy = xstrdup (path);

	char *dir = path_dir (path);
	ck_assert_str_eq (path, path_copy);
	ck_assert_str_eq (dir, "/home/ponga");

	free (path_copy);
	free (dir);
}
END_TEST

START_TEST (test_path_file)
{
	char path[] = "/home/ponga/ponga.txt";
	char path_dot[] = "/home/ponga/.ponga.txt";
	char *path_copy = xstrdup (path);
	char *path_dot_copy = xstrdup (path_dot);

	char *file_w_ext = NULL;
	char *file_wo_ext = NULL;
	char *file_dot_w_ext = NULL;
	char *file_dot_wo_ext = NULL;
	char *file_no_change = NULL;

	file_w_ext = path_file (path, 0);
	ck_assert_str_eq (file_w_ext, "ponga.txt");
	ck_assert_str_eq (path, path_copy);

	file_wo_ext = path_file (path, 1);
	ck_assert_str_eq (file_wo_ext, "ponga");

	file_dot_w_ext = path_file (path_dot, 0);
	ck_assert_str_eq (file_dot_w_ext, ".ponga.txt");
	ck_assert_str_eq (path_dot, path_dot_copy);

	file_dot_wo_ext = path_file (path_dot, 1);
	ck_assert_str_eq (file_dot_wo_ext, ".ponga");

	file_no_change = path_file (file_dot_wo_ext, 1);
	ck_assert_str_eq (file_no_change, ".ponga");

	free (path_copy);
	free (path_dot_copy);
	free (file_w_ext);
	free (file_wo_ext);
	free (file_dot_w_ext);
	free (file_dot_wo_ext);
	free (file_no_change);
}
END_TEST

START_TEST (test_which)
{
	ck_assert_int_eq (which ("wc"), 1);
	ck_assert_int_eq (which ("pongadelapenha"), 0);
}
END_TEST

START_TEST (test_exists)
{
	char file[] = "/tmp/pongaXXXXXX";

	xmkstemp (file);

	ck_assert_int_eq (exists (file), 1);
	ck_assert_int_eq (exists ("pongatrongaflofa"), 0);

	xunlink (file);
}
END_TEST

START_TEST (test_xstrdup_concat)
{
	char *surname = "ponguita";
	char *full_name = NULL;

	full_name = xstrdup_concat (full_name, "ponga");
	full_name = xstrdup_concat (full_name, " ");
	full_name = xstrdup_concat (full_name, surname);

	ck_assert_str_eq (full_name, "ponga ponguita");

	xfree (full_name);
}
END_TEST

START_TEST (test_xasprintf_concat)
{
	char *str = xstrdup ("I want");

	xasprintf_concat (&str, " %d potatos", 5);
	xasprintf_concat (&str, " and %d tomatos", 10);

	ck_assert_str_eq (str, "I want 5 potatos and 10 tomatos");

	xfree (str);
}
END_TEST

START_TEST (test_mkdir_p)
{
	DIR *dp = NULL;
	const char *dir = "/tmp/ponga1";
	const char *dir2 = "/tmp/ponga2";
	const char *subdir = "/tmp/ponga2/ponga3";

	mkdir_p (dir);
	mkdir_p (subdir);

	dp = opendir (dir);
	ck_assert (dp != NULL);
	closedir (dp);

	dp = opendir (subdir);
	ck_assert (dp != NULL);
	closedir (dp);

	rmdir (dir);
	rmdir (subdir);
	rmdir (dir2);
}
END_TEST

static void
test_sigint (int sig)
{
	ck_assert_int_eq (sig, SIGINT);
}

static void
test_sigterm (int sig)
{
	ck_assert_int_eq (sig, SIGTERM);
}

static void
test_sigquit (int sig)
{
	ck_assert_int_eq (sig, SIGQUIT);
}

START_TEST (test_setup_signal)
{
	setup_signal (SIGINT, test_sigint);
	setup_signal (SIGQUIT, test_sigquit);
	setup_signal (SIGTERM, test_sigterm);

	if (raise (SIGINT))
		log_errno_fatal ("raise failed");

	if (raise (SIGQUIT))
		log_errno_fatal ("raise failed");

	if (raise (SIGTERM))
		log_errno_fatal ("raise failed");
}
END_TEST

START_TEST (test_buf_expand)
{
	char *name = NULL;
	size_t size = 0;
	size_t total_size = 0;

	size = 6;
	name = xcalloc (size, sizeof (char));
	strncpy (name, "ponga", size);
	ck_assert_str_eq (name, "ponga");

	size = buf_expand ((void **) &name,
			sizeof (char), size, 5);

	// 6 + 5 = 11 -> 2^3 == 8 e 2^4 == 16
	total_size = 16;

	strncpy (name, "pongaponga", size);
	ck_assert_str_eq (name, "pongaponga");
	ck_assert_int_eq (size, total_size);

	xfree (name);
}
END_TEST

START_TEST (test_entry_size)
{
	char *name = NULL;
	size_t size = 0;

	size = entry_set (&name, size, "ponga");
	ck_assert_str_eq (name, "ponga");
	ck_assert_int_eq (size, 8); // 2^3

	size = entry_set (&name, size, "pongaponga");
	ck_assert_str_eq (name, "pongaponga");
	ck_assert_int_eq (size, 16); // 2^4

	xfree (name);
}
END_TEST

Suite *
make_utils_suite (void)
{
	Suite *s;
	TCase *tc_core;

	s = suite_create ("Utils");

	/* Core test case */
	tc_core = tcase_create ("Core");

	tcase_add_test (tc_core, test_fequal);
	tcase_add_test (tc_core, test_equalstring);
	tcase_add_test (tc_core, test_casequalstring);
	tcase_add_test (tc_core, test_cmpstringp);
	tcase_add_test (tc_core, test_casecmpstringp);
	tcase_add_test (tc_core, test_chomp);
	tcase_add_test (tc_core, test_trimc);
	tcase_add_test (tc_core, test_trim);
	tcase_add_test (tc_core, test_upper);
	tcase_add_test (tc_core, test_lower);
	tcase_add_test (tc_core, test_path_dir);
	tcase_add_test (tc_core, test_path_file);
	tcase_add_test (tc_core, test_which);
	tcase_add_test (tc_core, test_exists);
	tcase_add_test (tc_core, test_xstrdup_concat);
	tcase_add_test (tc_core, test_xasprintf_concat);
	tcase_add_test (tc_core, test_mkdir_p);
	tcase_add_test (tc_core, test_setup_signal);
	tcase_add_test (tc_core, test_buf_expand);
	tcase_add_test (tc_core, test_entry_size);
	suite_add_tcase (s, tc_core);

	return s;
}
