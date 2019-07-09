#ifndef UTILS_H
#define UTILS_H

char * chomp       (char *str);
char * trim        (char *str);
char * trimc       (char *str, int c);

char * upper       (char *s);
char * lower       (char *s);

char * path_dir    (const char *path);
char * path_file   (const char *path, int rm_ext);

int    which       (const char *cmd);
int    exists      (const char *file);

char * xstrdup_concat   (char *dest, const char *src);
int    xasprintf_concat (char **strp, const char *fmt, ...)
	__attribute__((format (printf, 2, 3)));

#endif /* utils.h */
