#include "e_mod_main.h"
#include "x_clipboard.h"

struct _E_Config_Dialog_Data
{
  E_Config_Dialog *cfd;
  Evas_Object *obj;
  Evas_Object *sync_widget;  /* Used to toogle Config Dialog
                              *   Synchronize option on and off */

  /* Store some initial states of clipboard configuration we will need    */

  double init_label_length;  /* Initial label length.                     */
  struct {                   /* Structure used to determine whether to    */
    int copy;                /*  toggle Config Dialog Synchronize option  */
    int select;              /*  on and off                               */
    int sync;
  } sync_state;
  /* Actual options user can change */
  int   clip_copy;     /* Clipboard to use                                */
  int   clip_select;   /* Clipboard to use                                */
  int   sync;          /* Synchronize clipboards flag                     */
  int   persistence;   /* History file persistance                        */
  int   hist_reverse;  /* Order to display History                        */
  double hist_items;   /* Number of history items to store                */
  double label_length; /* Number of characters of item to display         */
  int   trim_ws;       /* Should we trim White space from selection       */
  int   trim_nl;       /* Should we trim new lines from selection         */
  int   confirm_clear; /* Display history confirmation dialog on deletion */
};

static int           _basic_apply_data(E_Config_Dialog *cfd __UNUSED__, E_Config_Dialog_Data *cfdata);
static void         *_create_data(E_Config_Dialog *cfd __UNUSED__);
static int           _basic_check_changed(E_Config_Dialog *cfd __UNUSED__, E_Config_Dialog_Data *cfdata);
static void          _fill_data(E_Config_Dialog_Data *cfdata);
void                 _free_data(E_Config_Dialog *cfd __UNUSED__, E_Config_Dialog_Data *cfdata);
static Evas_Object  *_basic_create_widgets(E_Config_Dialog *cfd __UNUSED__, Evas *evas, E_Config_Dialog_Data *cfdata);
static int           _update_widget(E_Config_Dialog_Data *cfdata);
static Eina_Bool     _sync_state_changed(E_Config_Dialog_Data *cfdata);
extern Mod_Inst     *clip_inst; /* Found in e_mod_main.c */

static void *
_create_data(E_Config_Dialog *cfd __UNUSED__)
{
  E_Config_Dialog_Data *cfdata = E_NEW(E_Config_Dialog_Data, 1);
  _fill_data(cfdata);
  return cfdata;
}

void
_free_data(E_Config_Dialog *cfd __UNUSED__, E_Config_Dialog_Data *cfdata)
{
  EINA_SAFETY_ON_NULL_RETURN(clipboard_config);
  clipboard_config->config_dialog = NULL;
  E_FREE(cfdata);
}

static void
_fill_data(E_Config_Dialog_Data *cfdata)
{
  cfdata->sync_state.copy   = clipboard_config->clip_copy;
  cfdata->sync_state.select = clipboard_config->clip_select;
  cfdata->sync_state.sync   = clipboard_config->sync;
  cfdata->init_label_length = clipboard_config->label_length;

  cfdata->clip_copy     = clipboard_config->clip_copy;
  cfdata->clip_select   = clipboard_config->clip_select;
  cfdata->sync          = clipboard_config->sync;
  cfdata->persistence   = clipboard_config->persistence;
  cfdata->hist_reverse  = clipboard_config->hist_reverse;
  cfdata->hist_items    = clipboard_config->hist_items;
  cfdata->label_length  = clipboard_config->label_length;
  cfdata->trim_ws       = clipboard_config->trim_ws;
  cfdata->trim_nl       = clipboard_config->trim_nl;
  cfdata->confirm_clear = clipboard_config->confirm_clear;
}

static int
_basic_apply_data(E_Config_Dialog *cfd __UNUSED__, E_Config_Dialog_Data *cfdata)
{
  clipboard_config->clip_copy     = cfdata->clip_copy;
  clipboard_config->clip_select   = cfdata->clip_select;
  clipboard_config->sync          = cfdata->sync;
  clipboard_config->persistence   = cfdata->persistence;
  clipboard_config->hist_reverse  = cfdata->hist_reverse;
  /* Do we need to Truncate our history list? */
  if (clipboard_config->hist_items   != cfdata->hist_items)
    truncate_history(cfdata-> hist_items);
  clipboard_config->hist_items    = cfdata->hist_items;
  /* Has clipboard label name length changed ? */
  if (cfdata->label_length != cfdata->init_label_length) {
    clipboard_config->label_length_changed = EINA_TRUE;
    cfdata->init_label_length = cfdata->label_length;
  }
  clipboard_config->label_length  = cfdata->label_length;

  clipboard_config->trim_ws       = cfdata->trim_ws;
  clipboard_config->trim_nl       = cfdata->trim_nl;
  clipboard_config->confirm_clear = cfdata->confirm_clear;

  /* Be sure we set our clipboard 'object'with new configuration */
  init_clipboard_struct(clipboard_config);

  /* and Update our Widget sync state info */
  cfdata->sync_state.copy   = clipboard_config->clip_copy;
  cfdata->sync_state.select = clipboard_config->clip_select;
  cfdata->sync_state.sync   = clipboard_config->sync;

  /* Now save configuration */
  e_config_save_queue();
  return 1;
}

static Evas_Object *
_basic_create_widgets(E_Config_Dialog *cfd __UNUSED__, Evas *evas, E_Config_Dialog_Data *cfdata)
{
  Evas_Object *o, *ob, *of;

  o = e_widget_list_add(evas, 0, 0);
  /* Clipboard Config Section     */
  of = e_widget_framelist_add(evas, _("Clipboards"), 0);
  ob = e_widget_check_add(evas, _("Use Copy (Ctrl-C)"), &(cfdata->clip_copy));
  e_widget_framelist_object_append(of, ob);

  ob = e_widget_check_add(evas, _("Use Primary (Selection)"), &(cfdata->clip_select));
  e_widget_framelist_object_append(of, ob);

  ob = e_widget_check_add(evas, _("Synchronize Clipboards"), &(cfdata->sync));
  if ( !(cfdata->clip_copy && cfdata->clip_select))
    e_widget_disabled_set(ob, EINA_TRUE);
  cfdata->sync_widget = ob;
  e_widget_framelist_object_append(of, ob);

  e_widget_list_object_append(o, of, 1, 0, 0.5);
  /* History Config Section       */
  of = e_widget_framelist_add(evas, _("History"), 0);
  ob = e_widget_check_add(evas, _("Save History"), &(cfdata->persistence));
  e_widget_framelist_object_append(of, ob);

  ob = e_widget_check_add(evas, _("Reverse order"), &(cfdata->hist_reverse));
  e_widget_framelist_object_append(of, ob);

   ob = e_widget_label_add(evas, _("Items in history"));
   e_widget_framelist_object_append(of, ob);
   ob = e_widget_slider_add(evas, 1, 0, "%2.0f", 5.0, 50.0, 1.0, 0, &(cfdata->hist_items), NULL, 40);
   e_widget_framelist_object_append(of, ob);

   ob = e_widget_label_add(evas, _("Items label length"));
   e_widget_framelist_object_append(of, ob);
   ob = e_widget_slider_add(evas, 1, 0, "%2.0f", 5.0, 50.0, 1.0, 0, &(cfdata->label_length), NULL, 40);
   e_widget_framelist_object_append(of, ob);

  e_widget_list_object_append(o, of, 1, 0, 0.5);

  /* Miscellaneous Config Section */
  of = e_widget_framelist_add(evas, _("Miscellaneous"), 0);
  ob = e_widget_check_add(evas, _("Trim Whitespace"), &(cfdata->trim_ws));
  e_widget_framelist_object_append(of, ob);

  ob = e_widget_check_add(evas, _("Trim Newlines"), &(cfdata->trim_nl));
  e_widget_framelist_object_append(of, ob);

  ob = e_widget_check_add(evas, _("Confirm before clearing history"), &(cfdata->confirm_clear));
  e_widget_framelist_object_append(of, ob);

  e_widget_list_object_append(o, of, 1, 0, 0.5);
  return o;
}

E_Config_Dialog *
_config_clipboard_module(E_Container *con, const char *params __UNUSED__)
{
  E_Config_Dialog *cfd;
  E_Config_Dialog_View *v;

  if(e_config_dialog_find("E", "settings/clipboard")) return NULL;
  v = E_NEW(E_Config_Dialog_View, 1);
  v->create_cfdata = _create_data;
  v->free_cfdata = _free_data;
  v->basic.create_widgets = _basic_create_widgets;
  v->basic.apply_cfdata = _basic_apply_data;
  v->basic.check_changed = _basic_check_changed;

  cfd = e_config_dialog_new(con, "Clipboard Settings",
            "E", "preferences/clipboard",
            "preferences-engine", 0, v, NULL);
  clipboard_config->config_dialog = cfd;
  return cfd;
}

static int
_basic_check_changed(E_Config_Dialog *cfd __UNUSED__, E_Config_Dialog_Data *cfdata)
{
  if (_sync_state_changed(cfdata))
    return _update_widget(cfdata);
  if (clipboard_config->clip_copy     != cfdata->clip_copy) return 1;
  if (clipboard_config->clip_select   != cfdata->clip_select) return 1;
  if (clipboard_config->sync          != cfdata->sync) return 1;
  if (clipboard_config-> persistence  != cfdata->persistence) return 1;
  if (clipboard_config-> hist_reverse != cfdata->hist_reverse) return 1;
  if (clipboard_config-> hist_items   != cfdata->hist_items) return 1;
  if (clipboard_config-> label_length != cfdata->label_length) return 1;
  if (clipboard_config->trim_ws       != cfdata->trim_ws) return 1;
  if (clipboard_config->trim_nl       != cfdata->trim_nl) return 1;
  if (clipboard_config->confirm_clear != cfdata->confirm_clear) return 1;
  return 0;
}

static Eina_Bool
_sync_state_changed(E_Config_Dialog_Data *cfdata)
{
  if ((cfdata->sync_state.copy   != cfdata->clip_copy) ||
      (cfdata->sync_state.select != cfdata->clip_select) ||
      (cfdata->sync_state.sync   != cfdata->sync)) {
    cfdata->sync_state.copy   = cfdata->clip_copy;
    cfdata->sync_state.select = cfdata->clip_select;
    cfdata->sync_state.sync   = cfdata->sync;
    return EINA_TRUE;
  }
  return EINA_FALSE;
}

static int
_update_widget(E_Config_Dialog_Data *cfdata)
{
  if(cfdata->clip_copy && cfdata->clip_select) {
    e_widget_disabled_set(cfdata->sync_widget, EINA_FALSE);
  } else {
    e_widget_check_checked_set(cfdata->sync_widget, 0);
    e_widget_disabled_set(cfdata->sync_widget, EINA_TRUE);
  }
  return 1;
}

Eet_Error
truncate_history(const unsigned int n)
{
  Eet_Error err = EET_ERROR_NONE;

  EINA_SAFETY_ON_NULL_RETURN_VAL(clip_inst, EET_ERROR_BAD_OBJECT);
  if (clip_inst->items) {
    if (eina_list_count(clip_inst->items) > n) {
      Eina_List *last, *discard;
      last = eina_list_nth_list(clip_inst->items, n-1);
      clip_inst->items = eina_list_split_list(clip_inst->items, last, &discard);
      if (discard)
        E_FREE_LIST(discard, free_clip_data);
      err = clip_save(clip_inst->items);
    }
  }
  else
    err = EET_ERROR_EMPTY;
  return err;
}
