#include "e_mod_main.h"
#include "config_defaults.h"
#include "history.h"

#define _(S) S
#define ENABLE_DEBUG 1
#define DEBUG(f, ...) if (ENABLE_DEBUG) \
          printf("[clipboard] "f "\n", __VA_ARGS__)

#define TIMEOUT_1 1.0 // interval for timer

/* gadcon requirements */
static     Evas_Object *_gc_icon(const E_Gadcon_Client_Class *client_class, Evas * evas);
static const char      *_gc_id_new(const E_Gadcon_Client_Class *client_class);
static E_Gadcon_Client *_gc_init(E_Gadcon * gc, const char *name, const char *id, const char *style);
static void             _gc_orient(E_Gadcon_Client * gcc, E_Gadcon_Orient orient);
static const char      *_gc_label(const E_Gadcon_Client_Class *client_class);
static void             _gc_shutdown(E_Gadcon_Client * gcc);

/* and actually define the gadcon class that this module provides (just 1) */
static const E_Gadcon_Client_Class _gadcon_class = {
   GADCON_CLIENT_CLASS_VERSION,
   "clipboard",
   {
      _gc_init, _gc_shutdown, _gc_orient, _gc_label, _gc_icon, _gc_id_new, NULL,
      e_gadcon_site_is_not_toolbar
   },
   E_GADCON_CLIENT_STYLE_PLAIN
};

/* actual module specifics   */
Config *clipboard_config = NULL;
static E_Config_DD *conf_edd = NULL;
static E_Config_DD *conf_item_edd = NULL;
static Mod_Inst *clip_inst = NULL;

/*   First some call backs   */
static Eina_Bool _cb_clipboard_request(void *data);
static Eina_Bool _cb_event_selection(Instance *instance, int type, void *event);
static void      _cb_menu_item(Clip_Data *selected_clip);
static void      _cb_menu_post_deactivate(void *data, E_Menu *menu);
static void      _cb_show_menu(void *data, Evas *evas, Evas_Object *obj, Evas_Event_Mouse_Down *event);
/*   And then some auxillary functions */
static void      _clipboard_add_item(Clip_Data *clip_data);
static void      _clear_history(Instance *inst);
static void      _free_clip_data(Clip_Data *cd);
static void      _x_clipboard_update(const char *text);

/* Globals needed for convenience
 *   Probably should find a more 'acceptable solution' */
static E_Action *act = NULL;
const char *TMP_text = " ";

/*
 * This function is called when you add the Module to a Shelf or Gadgets, it
 * this is where you want to add functions to do things.
 */
static E_Gadcon_Client *
_gc_init(E_Gadcon *gc, const char *name, const char *id, const char *style)
{
  Evas_Object *o;
  E_Gadcon_Client *gcc, *last=NULL;

  Instance *inst = NULL;
  inst = E_NEW(Instance, 1);

  o = e_icon_add(gc->evas);
  e_icon_fdo_icon_set(o, "edit-paste");
  evas_object_show(o);

  gcc = e_gadcon_client_new(gc, name, id, style, o);
  gcc->data = inst;

  inst->gcc = gcc;
  inst->o_button = o;

  e_gadcon_client_util_menu_attach(gcc);
  evas_object_event_callback_add(inst->o_button, EVAS_CALLBACK_MOUSE_DOWN, (Evas_Object_Event_Cb) _cb_show_menu, inst);

  return gcc;
}

/*
 * This function is called when you remove the Module from a Shelf or Gadgets,
 * what this function really does is clean up, it removes everything the module
 * displays
 */
static void
_gc_shutdown(E_Gadcon_Client *gcc)
{
  Instance *inst = gcc->data;

  if (inst->menu) {
    e_menu_post_deactivate_callback_set(inst->menu, NULL, NULL);
    e_object_del(E_OBJECT(inst->menu));
    inst->menu = NULL;
  }

  evas_object_del(inst->o_button);

  E_FREE(inst);
}

static void
_gc_orient(E_Gadcon_Client *gcc, E_Gadcon_Orient orient)
{
  e_gadcon_client_aspect_set (gcc, 16, 16);
  e_gadcon_client_min_size_set (gcc, 16, 16);
}

/*
 * This function sets the Gadcon name of the module,
 *  do not confuse this with E_Module_Api
 */
static const char *
_gc_label (const E_Gadcon_Client_Class *client_class)
{
  return "Clipboard";
}

/*
 * This functions sets the Gadcon icon, the icon you see when you go to add
 * the module to a Shelf or Gadgets.
 */
static Evas_Object *
_gc_icon(const E_Gadcon_Client_Class *client_class, Evas * evas)
{
  Evas_Object *o;
  o = e_icon_add(evas);
  e_icon_fdo_icon_set(o, "edit-paste");

  return o;
}

/*
 * This function sets the id for the module, so it is unique from other
 * modules
 */
static const char *
_gc_id_new (const E_Gadcon_Client_Class *client_class)
{
  return _gadcon_class.name;
}


/**
 * @brief  Generic call back function to Create Clipboard menu
 *
 * @param  inst, pointer to calling Instance
 *         event, pointer to Ecore Event which triggered call back
 *
 * @return void
 *
 *  Here we attempt to handle all Ecore events which trigger display of the
 *  clipboard menu: either right clicking a Clipboard gadget or pressing a key
 *  or key combination bound to Clipboard Show Float menu. The generic nature
 *  of this function is a preliminary atempt to avoid repeated code but at the
 *  same time keep the logic relatively simple.
 *
 */

static void
_cb_show_menu(void *data, Evas *evas, Evas_Object *obj, Evas_Event_Mouse_Down *event)
{
  Instance *inst = (Instance*)data;
  Evas_Coord x, y, w, h;
  int cx, cy, dir, event_type = ecore_event_current_type_get();
  E_Container *con;
  E_Manager *man;
  E_Menu_Item *mi;
  Eina_List *it;
  Clip_Data *clip;
  Eina_Bool initialize;

  if (!inst)
    return;

  if (event_type == ECORE_EVENT_MOUSE_BUTTON_DOWN){
    /* Ignore all mouse events but right clicks        */
    if ((((Evas_Event_Mouse_Down *) event)->button) != 1)
      return;

    initialize = ((((Evas_Event_Mouse_Down *) event)->button) == 1) && (!inst->menu);
  } else {
    inst->gcc = NULL;
    initialize = EINA_TRUE;
  }

  if (initialize) {
    if (event_type == ECORE_EVENT_MOUSE_BUTTON_DOWN){
      /* Coordinates and sizing */
      evas_object_geometry_get(inst->o_button, &x, &y, &w, &h);
      e_gadcon_canvas_zone_geometry_get(inst->gcc->gadcon, &cx, &cy,
                                          NULL, NULL);
      x += cx;
      y += cy;
    } else {
      /* Coordinates and sizing */
      man = e_manager_current_get();
      con = e_container_current_get(man);
      ecore_x_pointer_xy_get(con->win, &x, &y);

    }
    inst->menu = e_menu_new();
    if (event_type == ECORE_EVENT_KEY_DOWN){
      e_menu_post_deactivate_callback_set(inst->menu, _cb_menu_post_deactivate, inst);
    }
    if (!clip_inst->items)
      printf("FUCK \n");

    if (clip_inst->items){
      EINA_LIST_FOREACH(clip_inst->items, it, clip){
        mi = e_menu_item_new(inst->menu);
        e_menu_item_label_set(mi, clip->name);
        e_menu_item_callback_set(mi, (E_Menu_Cb)_cb_menu_item, clip);
      }
    }

    mi = e_menu_item_new(inst->menu);
    e_menu_item_separator_set(mi, EINA_TRUE);

    mi = e_menu_item_new(inst->menu);
    e_menu_item_label_set(mi, _("Add clipboard content"));
    e_util_menu_item_theme_icon_set(mi, "edit-paste");
    e_menu_item_callback_set(mi, (E_Menu_Cb)_cb_clipboard_request, inst);

    mi = e_menu_item_new(inst->menu);
    e_menu_item_separator_set(mi, EINA_TRUE);

    mi = e_menu_item_new(inst->menu);
    e_menu_item_label_set(mi, _("Clear"));
    e_util_menu_item_theme_icon_set(mi, "edit-clear");
    e_menu_item_callback_set(mi, (E_Menu_Cb)_clear_history, inst);

    mi = e_menu_item_new(inst->menu);
    e_menu_item_separator_set(mi, EINA_TRUE);

    mi = e_menu_item_new(inst->menu);
    e_menu_item_label_set(mi, _("Settings"));
    e_util_menu_item_theme_icon_set(mi, "preferences-system");

    if (event_type == ECORE_EVENT_MOUSE_BUTTON_DOWN){
      e_menu_post_deactivate_callback_set(inst->menu, _cb_menu_post_deactivate, inst);

      /* Proper menu orientation */
      switch (inst->gcc->gadcon->orient) {
        case E_GADCON_ORIENT_TOP:
        case E_GADCON_ORIENT_CORNER_TL:
        case E_GADCON_ORIENT_CORNER_TR:
          dir = E_MENU_POP_DIRECTION_DOWN;
          break;
        case E_GADCON_ORIENT_BOTTOM:
        case E_GADCON_ORIENT_CORNER_BL:
        case E_GADCON_ORIENT_CORNER_BR:
          dir = E_MENU_POP_DIRECTION_UP;
          break;

        case E_GADCON_ORIENT_LEFT:
        case E_GADCON_ORIENT_CORNER_LT:
        case E_GADCON_ORIENT_CORNER_LB:
          dir = E_MENU_POP_DIRECTION_RIGHT;
          break;

        case E_GADCON_ORIENT_RIGHT:
        case E_GADCON_ORIENT_CORNER_RT:
        case E_GADCON_ORIENT_CORNER_RB:
          dir = E_MENU_POP_DIRECTION_LEFT;
          break;

        case E_GADCON_ORIENT_FLOAT:
        case E_GADCON_ORIENT_HORIZ:
        case E_GADCON_ORIENT_VERT:
        default:
          dir = E_MENU_POP_DIRECTION_AUTO;
          break;
      }
      e_gadcon_locked_set(inst->gcc->gadcon, EINA_TRUE);

      /* We display not relatively to the gadget, but similarly to
       * the start menu - thus the need for direction etc.
       */
      e_menu_activate_mouse(inst->menu,
                          e_util_zone_current_get(e_manager_current_get()),
                    x, y, w, h, dir, ((Evas_Event_Mouse_Down *) event)->timestamp);
    } else {
      // e_gadcon_locked_set(inst->gcc->gadcon, EINA_TRUE);
       e_menu_activate_mouse(inst->menu, e_util_zone_current_get
                            (e_manager_current_get()),
                    x, y, 1, 1, 2, 1);
    }
  }
}

static Eina_Bool
_cb_event_selection(Instance *instance, int type, void *event)
{
  Ecore_X_Event_Selection_Notify *ev;
  Clip_Data *cd = NULL;
  const char *data;

  if (!instance)
    return EINA_TRUE;
  ev = event;

  if ((ev->selection == ECORE_X_SELECTION_CLIPBOARD) &&
      (strcmp(ev->target, ECORE_X_SELECTION_TARGET_UTF8_STRING) == 0)) {

    Ecore_X_Selection_Data_Text *text_data;
    text_data = ev->data;

    if ((text_data->data.content == ECORE_X_SELECTION_CONTENT_TEXT) &&
        (text_data->text)){
      char buf[MAGIC_LABEL_SIZE + 1];
      char *temp_buf, *strip_buf;

      if (text_data->data.length == 0)
        return EINA_TRUE;

      cd = E_NEW(Clip_Data, 1);
      asprintf(&cd->content, "%s", text_data->text);
      // get rid unwanted chars from string - spaces and tabs
      asprintf(&temp_buf,"%s",text_data->text);
      memset(buf, '\0', sizeof(buf));
      strip_buf = strip_whitespace(temp_buf);
      strncpy(buf, strip_buf, MAGIC_LABEL_SIZE);
      asprintf(&cd->name, "%s", buf);
      free(temp_buf);
      free(strip_buf);

      if (strcmp(text_data->text,TMP_text)!=0) {
        _clipboard_add_item(cd);
          asprintf(&TMP_text, "%s", text_data->text);
      }
    }
  }
  return ECORE_CALLBACK_DONE;
}

/* Updates clipboard content with the selected text of the modules Menu */
void
_x_clipboard_update(const char *text)
{
  EINA_SAFETY_ON_NULL_RETURN(clip_inst);
  EINA_SAFETY_ON_NULL_RETURN(text);

  ecore_x_selection_clipboard_set(clip_inst->win, text, strlen(text) + 1);
}

void _clipboard_add_item(Clip_Data *cd)
{
  Eina_List *it;
  Clip_Data *clip;
  if (!cd)
    return;
  /* Remove duplicate items in Eina list */
  EINA_LIST_FOREACH(clip_inst->items, it, clip) {
    if (strcmp(cd->content, clip->content)==0){
      clip_inst->items = eina_list_remove(clip_inst->items, clip);
      item_num--;
    }
  }
  /* adding item to the list */
  if (item_num < MAGIC_HIST_SIZE) {
    clip_inst->items = eina_list_prepend(clip_inst->items, cd);
    item_num++;
  }
  else {
    /* remove last item from the list */
    clip_inst->items = eina_list_remove_list(clip_inst->items, eina_list_last(clip_inst->items));
    /*  add clipboard data stored in cd to the list as a first item */
    clip_inst->items = eina_list_prepend(clip_inst->items, cd);
  }
  /* saving list to the file */
  save_history(clip_inst->items);
}

void _clear_history(Instance *inst)
{
  Eina_List *l;
  E_Gadcon_Client *gadget;

  if (!clip_inst)
    return;
  if (clip_inst->items)
    E_FREE_LIST(clip_inst->items, _free_clip_data);
  item_num = 0;
  clip_inst->items = NULL;

  /* Ensure clipboard is clear and save history */
  ecore_x_selection_clipboard_clear();
  save_history(clip_inst->items);
}

static Eina_Bool
_cb_clipboard_request(void *data)
{
  ecore_x_selection_clipboard_request(clip_inst->win, ECORE_X_SELECTION_TARGET_UTF8_STRING);
  return EINA_TRUE;
}

static void
_cb_menu_item(Clip_Data *selected_clip)
{
  _x_clipboard_update(selected_clip->content);
}


static void
_cb_menu_post_deactivate(void *data, E_Menu *menu)
{
  Instance *inst = data;

  if (!inst) return;
  inst->menu = NULL;
}

static void _free_clip_data(Clip_Data *cd)
{
  free(cd->name);
  free(cd->content);
  free(cd);
}

/* This is needed to advertise a label for the module IN the code (not just
 * the .desktop file) but more specifically the api version it was compiled
 * for so E can skip modules that are compiled for an incorrect API version
 * safely) */
EAPI E_Module_Api e_modapi = {
  E_MODULE_API_VERSION,
  "Clipboard"
};

/*
 * This is the first function called by e17 when you load the module
 */
EAPI void *
e_modapi_init (E_Module * m)
{
  e_gadcon_provider_register(&_gadcon_class);

  e_configure_registry_item_add("preferences/clipboard", 10,
            "Clipboard Settings", NULL,
            "edit-paste", _config_clipboard_module);

  conf_item_edd = E_CONFIG_DD_NEW("Clipboard_Config_Item", Config_Item);
#undef T
#undef D
#define T Config_Item
#define D conf_item_edd
  E_CONFIG_VAL(D, T, id, STR);
  conf_edd = E_CONFIG_DD_NEW("Clipboard_Config", Config);
#undef T
#undef D
#define T Config
#define D conf_edd
  E_CONFIG_LIST(D, T, items, conf_item_edd);
  E_CONFIG_VAL(D, T, clip_copy, INT);
  E_CONFIG_VAL(D, T, clip_select, INT);
  E_CONFIG_VAL(D, T, persistence, INT);
  E_CONFIG_VAL(D, T, hist_reverse, INT);
  E_CONFIG_VAL(D, T, hist_items, INT);
  E_CONFIG_VAL(D, T, label_length, INT);
  E_CONFIG_VAL(D, T, trim_ws, INT);
  E_CONFIG_VAL(D, T, trim_nl, INT);
  E_CONFIG_VAL(D, T, confirm_clear, INT);

  clipboard_config = e_config_domain_load("module.clipboard", conf_edd);
  if (!clipboard_config) {
    clipboard_config = E_NEW(Config, 1);
    clipboard_config->clip_copy     = CONFIG_DEFAULT_CLIP_COPY;
    clipboard_config->clip_select   = CONFIG_DEFAULT_CLIP_SELECT;
    clipboard_config->persistence   = CONFIG_DEFAULT_CLIP_PERSISTANCE;
    clipboard_config->hist_reverse  = CONFIG_DEFAULT_CLIP_HIST_REVERSE;
    clipboard_config->hist_items    = CONFIG_DEFAULT_CLIP_HIST_ITEMS;
    clipboard_config->label_length  = CONFIG_DEFAULT_CLIP_LABEL_LENGTH;
    clipboard_config->trim_ws       = CONFIG_DEFAULT_CLIP_TRIM_WS;
    clipboard_config->trim_nl       = CONFIG_DEFAULT_CLIP_NL;
    clipboard_config->confirm_clear = CONFIG_DEFAULT_CLIP_COMFIRM_CLEAR;
  }
  E_CONFIG_LIMIT(clipboard_config->clip_copy, 0, 1);
  E_CONFIG_LIMIT(clipboard_config->clip_select, 0, 1);
  E_CONFIG_LIMIT(clipboard_config->persistence, 0, 1);
  E_CONFIG_LIMIT(clipboard_config->hist_reverse, 0, 1);
  E_CONFIG_LIMIT(clipboard_config->hist_items, 5, 50);
  E_CONFIG_LIMIT(clipboard_config->label_length, 5, 50);
  E_CONFIG_LIMIT(clipboard_config->trim_ws, 0, 1);
  E_CONFIG_LIMIT(clipboard_config->trim_nl, 0, 1);
  E_CONFIG_LIMIT(clipboard_config->confirm_clear, 0, 1);

  item_num=0;
  clipboard_config->module = m;
  //e_module_delayed_set(m, 1);

  clip_inst = E_NEW(Mod_Inst, 1);
  /* Create an invisible window for clipboard input purposes
   *   It is my understanding this should not displayed.*/
  clip_inst->win = ecore_x_window_input_new(0, 10, 10, 100, 100);

  E_LIST_HANDLER_APPEND(clip_inst->handle, ECORE_X_EVENT_SELECTION_NOTIFY, _cb_event_selection, clip_inst);
  ecore_x_selection_clipboard_request(clip_inst->win, ECORE_X_SELECTION_TARGET_UTF8_STRING);
  clip_inst->check_timer = ecore_timer_add(TIMEOUT_1, _cb_clipboard_request, clip_inst);
  read_history(&(clip_inst->items));

  act = e_action_add("clipboard");
  if (act) {
    act->func.go = (void *) _cb_show_menu;
    e_action_predef_name_set("Clipboard","Show float menu", "clipboard", "<none>", NULL, 0);
  }
  return m;
}

/*
 * This function is called by e17 when you unload the module,
 * here you should try to free all resources used while the module was enabled.
 */
EAPI int
e_modapi_shutdown (E_Module * m)
{
  Instance *inst;
  Config_Item *ci;

  while((clipboard_config->config_dialog = e_config_dialog_get("E", "preferences/clipboard")))
    e_object_del(E_OBJECT(clipboard_config->config_dialog));

  e_configure_registry_item_del("preferences/clipboard");

  if(clipboard_config->config_dialog)
    e_object_del(E_OBJECT(clipboard_config->config_dialog));
  E_FREE(clipboard_config->config_dialog);

  if(clipboard_config){
    EINA_LIST_FREE(clipboard_config->items, ci){
      eina_stringshare_del(ci->id);
      free(ci);
    }
    clipboard_config->module = NULL;
    E_FREE(clipboard_config);
  }
  E_CONFIG_DD_FREE(conf_edd);
  E_CONFIG_DD_FREE(conf_item_edd);

  if (act) {
    e_action_predef_name_del("Clipboard", "Show float menu");
    e_action_del("clipboard");
    act = NULL;
  }

  if (clip_inst->win)
    ecore_x_window_free(clip_inst->win);
  E_FREE_LIST(clip_inst->handle, ecore_event_handler_del);
  clip_inst->handle = NULL;
  ecore_timer_del(clip_inst->check_timer);
  clip_inst->check_timer = NULL;
  E_FREE_LIST(clip_inst->items, _free_clip_data);

  e_gadcon_provider_unregister(&_gadcon_class);

  return 1;
}

/*
 * This function is used to save and store configuration info on local
 * storage
 */
EAPI int
e_modapi_save(E_Module * m)
{
  e_config_domain_save("module.clipboard", conf_edd, clipboard_config);
  return 1;
}
