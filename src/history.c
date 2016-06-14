#include "history.h"

#define CLIPBOARD_MOD_NAME "clipboard"
#define DATA_DIR    CLIPBOARD_MOD_NAME
#define HISTORY_NAME "history"
#define HISTORY_MAGIC_SIZE 32
#define HISTORY_VERSION     1 /**index (-1) into array below  */
#if 0
  #define PATH_MAX 4096
  /* PATH_MAX is defined in <linux/limits.h>
   *     While this define is used through out enlightenment source code
   *     its use is perhaps a bad idea.
   *
   * see http://insanecoding.blogspot.com/2007/11/pathmax-simply-isnt.html
   *    "Since a path can be longer than PATH_MAX, the define is useless,
   *     writing code based off of it is wrong, and the functions that
   *     require it are broken."
   *
   * Regardless in the e-dev tradition we are using it here.
   */
#endif

// Private Funcions
Eina_Bool _mkpath_if_not_exists(const char *path);
Eina_Bool _set_data_path(char *path);
Eina_Bool _set_history_path(char *path);

/**
 * @brief Creates path if non-existant
 *
 * @param path char array to create.
 * @return EINA_TRUE on success EINA_FALSE otherwise
 *
 */
Eina_Bool
_mkpath_if_not_exists(const char *path)
{
    Eina_Bool success = EINA_TRUE;

    if (!path) return EINA_FALSE;
    if(!ecore_file_exists(path))
       return ecore_file_mkdir(path);
    return success;
}

/**
 * @brief Sets the XDG_DATA_HOME location for saving clipboard history data.
 *
 * @param path char array to store history path in.
 * @return EINA_TRUE on success EINA_FALSE otherwise
 *
 * This function assumes the path char array is large enough to store
 *   data path in. Naive implementation, no checks !!
 *
 * You have been warned!!
 *
 */
Eina_Bool
_set_data_path(char *path)
{
    // FIXME: Non-portable nix only code
    char *temp_str;
    Eina_Bool success = EINA_TRUE;

    if (!path) return EINA_FALSE;
    /* See if XDG_DATA_HOME is defined
     *     if so use it
     *     if not use XDG_DATA_HOME default
     */
    temp_str = getenv ("XDG_DATA_HOME");
    if (temp_str!=NULL) {
      strcpy(path, temp_str);
      // Ensure XDG_DATA_HOME terminates in '/'
      if (path[strlen(path)-1] != '/')
        strcat(path,"/");
      strcat(path, DATA_DIR);
      // printf ("The current path is: %s\n",path);
    }
    /* XDG_DATA_HOME default */
    else {
      temp_str = getenv("HOME");
      // Hopefully unnecessary Safety check
      if (temp_str!=NULL)
         sprintf(path,"%s/.local/share/",temp_str);
      else {
         // Should never happen
         strcpy(path, "");
         success = EINA_FALSE;
     }
    }
    //DEBUG("history path: %s", _path);
    return success;
}

/**
 * @brief Sets the path and file name for saving clipboard history data.
 *
 * @param history_path char array to store history path in.
 * @return EINA_TRUE on success EINA_FALSE otherwise
 *
 * This function not only sets the history path but also creates the needed
 * directories if they do not exist.
 *
 * This function assumes the path char array is large enough to store
 *   data path in. Naive implementation, no checks !!
 *
 * You have been warned!!
 */
Eina_Bool
_set_history_path(char *path)
{
   Eina_Bool success = EINA_TRUE;
   if (!path) return EINA_FALSE;
   if(_set_data_path(path)) {
       sprintf(path,"%s%s/", path, CLIPBOARD_MOD_NAME);
       success = _mkpath_if_not_exists(path);
       strcat(path, HISTORY_NAME);
   } else
       success = EINA_FALSE;
   return success;
}

/**
 * @brief  Reads clipboard Instantance history  from a binary file with location
 *           specified by FreeDesktop XDG specifications
 *
 * @param  inst, a pointer to clipboard instance
 *
 * @return  EET_ERROR_NONE on success
 *          Eet error identifier of error otherwise.
 *
 */
Eet_Error 
read_history(Instance* inst, int item_num) // FIXME item_num should not be needed
{
    Clip_Data *cd = NULL;
    char history_path[PATH_MAX];
    Eet_File *history_file;
    char *ret, *temp_buf;
    int i, size;
    char buf[MAGIC_LABEL_SIZE + 1], str[3];

    // FIXME
    if(!_set_history_path(history_path))
      return EET_ERROR_BAD_OBJECT;
    history_file = eet_open(history_path, EET_FILE_MODE_READ);
    if (!history_file) 
      return EET_ERROR_BAD_OBJECT;
    ret = eet_read(history_file, "MAX_ITEMS", &size);
    item_num=atoi(ret);

    for (i=1;i<=item_num;i++)
    {
        cd = E_NEW(Clip_Data, 1);  //new instance for another struct
        cd->inst = inst;

        sprintf(str, "%d", i);
        ret = eet_read(history_file,str, &size);
        // FIXME: DATA VALIDATION
        asprintf(&cd->content, "%s",ret);
        temp_buf = ret;
        temp_buf = strip_whitespace(temp_buf);
        memset(buf, '\0', sizeof(buf));
        strncpy(buf, temp_buf, MAGIC_LABEL_SIZE);
        asprintf(&(cd->name), "%s", buf);
        ((Instance*)cd->inst)->items = eina_list_append(((Instance*)cd->inst)->items, cd);
    }
    free(ret);
    //eet_shutdown();
    return eet_close(history_file);
}

/**
 * @brief  Saves clipboard Instantance history  in a binary file with location
 *           specified by FreeDesktop XDG specifications
 *
 * @param  inst, a pointer to clipboard instance
 *
 * @return  EET_ERROR_NONE on success
 *          Eet error identifier of error otherwise.
 *
 * This function not only sets the history path but will also create the needed
 *    directories if they do not exist. See warnings in auxillary functions.
 *
 */
Eet_Error
save_history(Instance* inst)
{
    Eina_List *l, *list = inst->items;
    int i=1;
    char str[3];
    Clip_Data *cd;
    char history_path[PATH_MAX];
    Eet_Error ret;
    //eet_init();
    if(!_set_history_path(history_path))
      return  EET_ERROR_BAD_OBJECT;
    if(!list){
      // FIXME: should just write empty data here
      ecore_file_remove(history_path);
      return EET_ERROR_BAD_OBJECT;
    }

    Eet_File *history_file = eet_open(history_path, EET_FILE_MODE_WRITE);

    if (history_file) {
        EINA_LIST_FOREACH(list, l, cd) {
            sprintf(str, "%d", i++);
            eet_write(history_file, str,  cd->content, strlen(cd->content) + 1, 0);
        }
        eet_write(history_file, "MAX_ITEMS",  str, strlen(str) + 1, 0);
        ret = eet_close(history_file);
        //eet_shutdown();
    } else
    {   fprintf(stderr, "Clipboard ERROR: Unable to open clipboard history.\n");
        return  EET_ERROR_BAD_OBJECT;
    }
    return ret;
}
