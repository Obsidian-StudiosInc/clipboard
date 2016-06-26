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
  char *middle;

  while(isspace(*str)) str++; // cleaning white chars before string

  if(*str == 0)  // empty string ?
    return str;

  end = str + strlen(str) - 1;  // finding end position
   while(end > str && isspace(*end)) end--;

  // Write new null terminator
  //~ *(end+1) = 0;

  middle = calloc(strlen(str), sizeof(char));
  char *start=middle; //remember start position

  while (str<=end && strlen(start)<MAGIC_LABEL_SIZE){
    if (!isspace(*str)) {
      *middle=*str;
      middle++;
  }
  else {
    *middle = ' ';
    middle++;
  }
  str++;
}
  return start;
}
