#ifndef CLIPBOARD_UTILITY_H
#define CLIPBOARD_UTILITY_H

#include <string.h>
#include <common.h>


char *strip_whitespace(char *str);
Eina_Bool set_clip_content(char **content, char* text, int mode);
Eina_Bool set_clip_name(char **name, char * text, int mode);

void _truncate_label(const unsigned int n, Clip_Data *clip_data);

#ifndef HAVE_STRNDUP
char *strndup(const char *s, size_t n);
#endif

#ifndef HAVE_STRDUP
char *strdup(const char *s);
#endif

#endif
