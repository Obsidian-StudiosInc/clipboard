#include "utility.h"

/**
 * @brief Strips whitespace from a string.
 *
 * @param str char pointer to a string.
 * 
 * @return a char pointer to a substring of the original string..
 *
 * If the given string was allocated dynamically, the caller must not overwrite
 *  that pointer with the returned value. The original pointer must be
 *  deallocated using the same allocator with which it was allocated.  The return
 *  value must NOT be deallocated using free etc.
 *
 * You have been warned!!
 */
char *
strip_whitespace(char *str)
{
  char *end;

  while(isspace(*str)) str++;

  if(*str == 0)  // empty string ?
    return str;

  end = str + strlen(str) - 1;
  while(end > str && isspace(*end))
    end--;

  // Write new null terminator
  *(end+1) = 0;

  return str;
}
