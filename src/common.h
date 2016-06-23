#ifndef CLIPBOARD_COMMON_H
#define CLIPBOARD_COMMON_H

#include <string.h>
#include <e.h>
#include "config.h"

#define MAGIC_LABEL_SIZE 20
#define MAGIC_HIST_SIZE  20

typedef struct _Clip_Data {
    void *inst;
    char *name;
    char *content;
} Clip_Data;

typedef struct _Instance Instance;
struct _Instance
{
   E_Gadcon_Client *gcc;
   E_Menu *menu;
   Ecore_X_Window win;
   Ecore_Timer  *check_timer;
   Evas_Object *o_button;
   Eina_List *handle;
   Eina_List *items;
};

#endif
