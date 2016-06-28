#ifndef CLIPBOARD_COMMON_H
#define CLIPBOARD_COMMON_H

#include <string.h>
#include <e.h>
#include "config.h"

#define MAGIC_LABEL_SIZE 20
#define MAGIC_HIST_SIZE  20

typedef struct _Clip_Data
{
    char *name;
    char *content;
} Clip_Data;

typedef struct _Instance Instance;
struct _Instance
{
    E_Gadcon_Client *gcc;
    E_Menu *menu;
    Evas_Object *o_button;
};

typedef struct _Mod_Inst Mod_Inst;
struct _Mod_Inst
{
    Ecore_X_Window win;
    Ecore_Timer  *check_timer;
    Eina_List *handle;
    Eina_List *items;
};

#endif
