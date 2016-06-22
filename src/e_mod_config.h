#ifdef E_TYPEDEFS
#else
#ifndef E_MOD_CONFIG_H
#define E_MOD_CONFIG_H

typedef struct _Config Config;
typedef struct _Config_Item Config_Item;

struct _Config
{
  Eina_List *items;
  E_Config_Dialog *cfd;
  E_Module *module;
  E_Config_Dialog *_config_dialog;
  int persistence;
};

struct _Config_Item
{
  const char *id;
};

E_Config_Dialog *_config_clipboard_module(E_Container *con, const char *params);

extern Config *clipboard_config;

#endif
#endif
