#include "utility.h"

#define TRIM_SPACES   0
#define TRIM_NEWLINES 1

char *my_strip_whitespace(char *str, int mode);
int isnewline(int c);

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

Eina_Bool
set_clip_content(char **content, char* text, int mode)
{
  Eina_Bool ret = EINA_TRUE;
  char *temp, *trim;
  /* Sanity check */
  if (!text) {
    WRN("ERROR: Text is NULL\n");
    text = "";
  }
  if (content) {
    switch (mode) {
      case 0:
        /* Don't trim */
        temp = malloc(strlen(text) + 1);
        strcpy(temp, text);
        break;
      case 1:
        /* Trim new lines */
        trim = my_strip_whitespace(text, TRIM_NEWLINES);
        temp = malloc(strlen(trim) + 1);
        strcpy(temp, trim);
        break;
      case 2:
        /* Trim all white Space
         *  since white space includes new lines
         *  drop thru here */
      case 3:
        /* Trim white space and new lines */
        trim = my_strip_whitespace(text, TRIM_SPACES);
        temp = malloc(strlen(trim) + 1);
        strcpy(temp, trim);
        break;
      default :
        /* Error Don't trim */
        WRN("ERROR: Invalid strip_mode %d\n", mode);
        temp = malloc(strlen(text) + 1);
        strcpy(temp, text);
        break;
    }
    if (!temp) {
      /* This is bad, leave it to calling function */
      CRI("ERROR: Memory allocation Failed!!");
      ret = EINA_FALSE;
    }
    *content = temp;
  } else
    ERR("Error: Clip content is Null!!");
  return ret;
}

Eina_Bool
set_clip_name(char **name, char * text, int mode)
{
  INF("Setting clip name");
  /* to be continued latter */
  return EINA_TRUE;
}

void
_truncate_label(const unsigned int n, Clip_Data *clip_data)
{
  char buf[n + 1];
  char *temp_buf, *strip_buf;
  Eina_List *it;


  //if (clip_inst->items) {
  if (1) {
      asprintf(&temp_buf,"%s",clip_data->content);
      memset(buf, '\0', sizeof(buf));
      strip_buf = strip_whitespace(temp_buf);
      strncpy(buf, strip_buf, n);
      asprintf(&clip_data->name, "%s", buf);
  }
}

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
my_strip_whitespace(char *str, int mode)
{
  char *end;
  int (*compare)(int);

  if (mode == TRIM_SPACES) compare = isspace;
  else                     compare = isnewline;

  while((*compare)(*str)) str++;

  if(*str == 0)  // empty string ?
    return str;

  end = str + strlen(str) - 1;
  while(end > str && (*compare)(*end))
    end--;

  // Write new null terminator
  *(end+1) = 0;

  return str;
}

int
isnewline(int c)
{
  if ((c == '\n')||(c == '\r')) return 1;
  else                      return 0;
}
