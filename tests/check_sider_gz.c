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
#include <check.h>
#include "check_sider.h"

#include "../src/utils.h"
#include "../src/wrapper.h"
#include "../src/gz.c"

static void
create_gz (const char *cnt, char *path)
{
	FILE *fp = NULL;
	int fd;

	fd = xmkstemp (path);
	fp = xfdopen (fd, "w");

	xfprintf (fp, "%s", cnt);

	xfclose (fp);
}

static void
create_big_text_gz (char *path)
{
	FILE *fp = NULL;
	int fd, i, j;

	fd = xmkstemp (path);
	fp = xfdopen (fd, "w");

	for  (i = 0; i < 10; i++)
		{
			for  (j = 0; j < 10000; j++)
				xfprintf (fp, "ponga");
			xfprintf (fp, "\n");
		}

	xfclose (fp);
}

static void
create_long_line_gz (char *path)
{
	FILE *fp = NULL;
	int fd, i;

	fd = xmkstemp (path);
	fp = xfdopen (fd, "w");

	for  (i = 0; i < 10000; i++)
		xfprintf (fp, "ponga");

	xfclose (fp);
}

START_TEST (test_open_fatal)
{
	char gz_path[] = "/tmp/ponga.txt.XXXXXX";
	gz_open_for_reading (gz_path);
}
END_TEST

START_TEST (test_close_fatal)
{
	GzFile *gz;
	char gz_path[] = "/tmp/ponga.txt.XXXXXX";

	create_gz ("PONGA\n", gz_path);
	gz = gz_open_for_reading (gz_path);

	gz_close (gz);
	xunlink (gz_path);

	gz_close (NULL);
	gz_close (gz);
}
END_TEST

START_TEST (test_read_fatal1)
{
	GzFile *gz = NULL;
	char gz_path[] = "/tmp/ponga.txt.XXXXXX";
	char *line = NULL;
	size_t n = 0;

	create_gz ("PONGA\n", gz_path);
	gz = gz_open_for_reading (gz_path);
	gzclose (gz->fp);

	while (gz_getline (gz, &line, &n))
		;

	xfree (line);
	gz_close (gz);
	xunlink (gz_path);
}
END_TEST

START_TEST (test_read_fatal2)
{
	GzFile *gz = NULL;
	char gz_path[] = "/tmp/ponga.txt.XXXXXX";
	char *line = NULL;
	size_t n = 0;

	create_big_text_gz (gz_path);
	gz = gz_open_for_reading (gz_path);

	gz_getline (gz, &line, &n);
	gzclose (gz->fp);

	while (gz_getline (gz, &line, &n))
		;

	xfree (line);
	gz_close (gz);
	xunlink (gz_path);
}
END_TEST

START_TEST (test_read1)
{
	GzFile *gz = NULL;
	char gz_path[] = "/tmp/ponga.txt.XXXXXX";
	char *line = NULL;
	size_t n = 0;

	create_big_text_gz (gz_path);
	gz = gz_open_for_reading (gz_path);

	while (gz_getline (gz, &line, &n))
		;

	xfree (line);
	gz_close (gz);
	xunlink (gz_path);
}
END_TEST

START_TEST (test_read2)
{
	GzFile *gz = NULL;
	char gz_path[] = "/tmp/ponga.txt.XXXXXX";
	char *line = NULL;
	int i = 0;
	int l = 0;
	size_t n = 0;

	const char *gz_cnt_ex =
		"=> 1 ponga\n"
		"=> 2 ponga\n"
		"=> 3 ponga\n"
		"=> 4 ponga\n"
		"=> 5 ponga\n"
		"=> 6 ponga\n"
		"=> 7 ponga\n"
		"=> 8 ponga\n"
		"=> 9 ponga\n"
		"=> 10 ponga\n";

	create_gz (gz_cnt_ex, gz_path);
	gz = gz_open_for_reading (gz_path);

	while (gz_getline (gz, &line, &n))
		{
			sscanf (line, "%*s %d", &l);
			ck_assert_int_eq (l, ++i);
		}

	ck_assert_int_eq (gz_get_num_line (gz), i);
	ck_assert_str_eq (gz_get_filename (gz), gz_path);

	xfree (line);
	gz_close (gz);
	xunlink (gz_path);
}
END_TEST

START_TEST (test_read_long_line)
{
	GzFile *gz = NULL;
	char gz_path[] = "/tmp/ponga.txt.XXXXXX";
	char *line = NULL;
	size_t n = 0;

	create_long_line_gz (gz_path);
	gz = gz_open_for_reading (gz_path);

	while (gz_getline (gz, &line, &n))
		;

	xfree (line);
	gz_close (gz);
	xunlink (gz_path);
}
END_TEST

Suite *
make_gz_suite (void)
{
	Suite *s;
	TCase *tc_core;
	TCase *tc_abort;
	TCase *tc_segfault;

	s = suite_create ("GZ");

	/* Core test case */
	tc_core = tcase_create ("Core");

	/* Abort test case */
	tc_abort = tcase_create ("Abort");

	/* Segfault test case */
	tc_segfault = tcase_create ("Segfault");

	tcase_add_test (tc_core, test_read1);
	tcase_add_test (tc_core, test_read2);
	tcase_add_test (tc_core, test_read_long_line);

	tcase_add_test_raise_signal (tc_abort,    test_open_fatal,  SIGABRT);

	tcase_add_test_raise_signal (tc_segfault, test_close_fatal, SIGSEGV);
	tcase_add_test_raise_signal (tc_segfault, test_read_fatal1, SIGSEGV);
	tcase_add_test_raise_signal (tc_segfault, test_read_fatal2, SIGSEGV);

	suite_add_tcase (s, tc_core);
	suite_add_tcase (s, tc_abort);
	suite_add_tcase (s, tc_segfault);

	return s;
}
