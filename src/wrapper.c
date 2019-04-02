#include "config.h"

#include <string.h>
#include <stdarg.h>
#include "log.h"
#include "wrapper.h"

char *
xstrdup (const char *str)
{
	char *ret = strdup (str);

	if (ret == NULL)
		log_errno_fatal ("strdup failed");

	return ret;
}

void *
xmalloc (size_t size)
{
	void *ret = malloc (size);

	if (ret == NULL && !size)
		ret = malloc (1);

	if (ret == NULL)
		log_errno_fatal ("malloc failed");

	return ret;
}

int
xvasprintf (char **strp, const char *fmt, va_list ap)
{
	int ret = vasprintf (strp, fmt, ap);

	if (ret < 0)
		log_errno_fatal ("vasprintf failed");

	return ret;
}

int
xasprintf (char **strp, const char *fmt, ...)
{
	int ret = 0;
	va_list ap;

	va_start (ap, fmt);
	ret = vasprintf (strp, fmt, ap);

	if (ret < 0)
		log_errno_fatal ("asprintf failed");

	va_end (ap);
	return ret;
}

void *
xcalloc (size_t nmemb, size_t size)
{
	void *ret = calloc (nmemb, size);

	if (ret == NULL && (!nmemb || !size))
		ret = calloc (1, 1);

	if (ret == NULL)
		log_errno_fatal ("calloc failed");

	return ret;
}

void *
xrealloc (void *ptr, size_t size)
{
	void *ret = realloc (ptr, size);

	if (ret == NULL && !size)
		ret = realloc (ret, 1);

	if (ret == NULL)
		log_errno_fatal ("realloc failed");

	return ret;
}

void
xfree (void *ptr)
{
	if (ptr == NULL)
		return;
	free (ptr);
}

FILE *
xfopen (const char *path, const char *mode)
{
	FILE *fp = fopen (path, mode);
	if (fp != NULL)
		return fp;

	if (*mode && mode[1] == '+')
		log_errno_fatal ("Could not open '%s' for reading and writing", path);
	else if (*mode == 'w' || *mode == 'a')
		log_errno_fatal ("Could not open '%s' for writing", path);
	else
		log_errno_fatal ("Could not open '%s' for reading", path);
}

void
xfclose (FILE *fp)
{
	if (fp == NULL)
		return;

	if (fclose (fp) == EOF)
		log_errno_fatal ("Could not close file stream");
}

FILE *
xpopen (const char *cmd, const char *mode)
{
	FILE *pp = popen (cmd, mode);
	if (pp != NULL)
		return pp;

	if (*mode && mode[0] == 'w')
		log_errno_fatal ("Could not open pipe for writing to '%s'", cmd);
	else
		log_errno_fatal ("Could not open pipe for reading from '%s'", cmd);
}

int
xpclose (FILE *pp)
{
	if (pp == NULL)
		return EOF;

	int stat = pclose (pp);

	if (stat == EOF)
		log_errno_fatal ("Could not close pipe");

	return WEXITSTATUS (stat);
}