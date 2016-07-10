#ifndef CLIPBOARD_UTILITY_H
#define CLIPBOARD_UTILITY_H

#include <string.h>
#include <common.h>


char *strip_whitespace(char *str);
Eina_Bool set_clip_content(char **content, char* text, int mode);

#endif
