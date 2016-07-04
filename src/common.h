#ifndef CLIPBOARD_COMMON_H
#define CLIPBOARD_COMMON_H

#include <string.h>
#include <e.h>
#include "config.h"

#define MAGIC_LABEL_SIZE 50
#define MAGIC_HIST_SIZE  20

typedef struct _Clip_Data
{
    /* A structure used for storing clipboard data in */
    char *name;
    char *content;
} Clip_Data;

typedef struct _Instance Instance;
struct _Instance
{
    /* An instance of our module with its elements */

    /* pointer to this gadget's container */
    E_Gadcon_Client *gcc;

    /* Pointer to gadget or float menu */
    E_Menu *menu;

    /* Pointer to mouse button object
     * to add call back to */
    Evas_Object *o_button;
};


typedef struct _Mod_Inst Mod_Inst;
struct _Mod_Inst
{
    /* Sructure to store a global module instance in
     *   complete with a hidden window for event notification purposes */

    /* A pointer to an Ecore window used to
     * recieve or send clipboard events to */
    Ecore_X_Window win;

    /* Timer callback function to reguest Clipboard events */
    Ecore_Timer  *check_timer;

    /* Callback function to handle clipboard events */
    Eina_List *handle;

    /* Stores Clipboard History */
    Eina_List *items;
};

#endif
