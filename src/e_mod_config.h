#ifdef E_TYPEDEFS
#else
#ifndef E_MOD_CONFIG_H
#define E_MOD_CONFIG_H

/* We create a structure config for our module, and also a config structure
 * for every item element (you can have multiple gadgets for the same module) */
typedef struct _Config Config;
typedef struct _Config_Item Config_Item;

struct _Config
{
  Eina_List *items;
  E_Module *module;
  E_Config_Dialog *config_dialog;
  const char *log_name;
  Eina_Bool label_length_changed;  /* Flag indicating a need to update all clip
                                       labels as configfuration changed. */

  int version;       /* Configuration version                           */
  int clip_copy;     /* Clipboard to use                                */
  int clip_select;   /* Clipboard to use                                */
  int sync;          /* Synchronize clipboards flag                     */
  int persistence;   /* History file persistance                        */
  int hist_reverse;  /* Order to display History                        */
  double hist_items;    /* Number of history items to store             */
  double label_length;  /* Number of characters of item to display      */
  int trim_ws;       /* Should we trim White space from selection       */
  int trim_nl;       /* Should we trim new lines from selection         */
  int confirm_clear; /* Display history confirmation dialog on deletion */
};

struct _Config_Item
{
  const char *id;
};

E_Config_Dialog *_config_clipboard_module(E_Container *con, const char *params __UNUSED__);

extern Config *clipboard_config;

#endif
#endif
