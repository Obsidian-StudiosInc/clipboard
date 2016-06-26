#include "e_mod_main.h"

struct _E_Config_Dialog_Data
{
  E_Config_Dialog *cfd;
  Evas_Object *obj;
  int clip_copy;
  int clip_select;
  int hist_reverse;
  char *label_length;
  char *hist_items;
  int persistence;
  int trim_ws;
  int trim_nl;
  int confirm_clear;
};

static int           _basic_apply_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata);
static void *        _create_data(E_Config_Dialog *cfd);
static void          _free_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata);
static int           _basic_check_changed(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata);
static void          _fill_data(E_Config_Dialog_Data *cfdata);
static Evas_Object * _basic_create_widgets(E_Config_Dialog *cfd , Evas *evas, E_Config_Dialog_Data *cfdata);


static void *
_create_data(E_Config_Dialog *cfd)
{
  E_Config_Dialog_Data *cfdata;

  cfdata = E_NEW(E_Config_Dialog_Data, 1);

  _fill_data(cfdata);
  return cfdata;
}

static void
_free_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata)
{
  if (!clipboard_config)
    return;
  clipboard_config->config_dialog = NULL;
  E_FREE(cfdata);
}

static void
_fill_data(E_Config_Dialog_Data *cfdata)
{
  char buf[1024];

  cfdata->clip_copy     = clipboard_config->clip_copy;
  cfdata->clip_select   = clipboard_config->clip_select;
  cfdata->persistence   = clipboard_config->persistence;
  cfdata->hist_reverse  = clipboard_config->hist_reverse;
  snprintf (buf,sizeof (buf), "%d",clipboard_config->hist_items );
  cfdata->hist_items = strdup (buf);
  snprintf (buf,sizeof (buf), "%d",clipboard_config->label_length );
  cfdata->label_length = strdup (buf);
  cfdata->trim_ws       = clipboard_config->trim_ws;
  cfdata->trim_nl       = clipboard_config->trim_nl;
  cfdata->confirm_clear = clipboard_config->confirm_clear;
}

static int
_basic_apply_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata)
{
  clipboard_config->clip_copy     = cfdata->clip_copy;
  clipboard_config->clip_select   = cfdata->clip_select;
  clipboard_config->persistence   = cfdata->persistence;
  clipboard_config->hist_reverse  = cfdata->hist_reverse;
  clipboard_config->hist_items    = atoi(cfdata->hist_items);
  clipboard_config->label_length   = atoi(cfdata->label_length);
  clipboard_config->trim_ws       = cfdata->trim_ws;
  clipboard_config->trim_nl       = cfdata->trim_nl;
  clipboard_config->confirm_clear = cfdata->confirm_clear;

  e_config_save_queue();
   return 1;
}

static Evas_Object *
_basic_create_widgets(E_Config_Dialog *cfd , Evas *evas, E_Config_Dialog_Data *cfdata)
{
  Evas_Object *o, *ob, *of;
  //~ cfdata->cfd = cfd;

  o = e_widget_list_add(evas, 0, 0);
  /* Clipboard Config Section     */
  of = e_widget_framelist_add(evas, "Clipboards", 0);
  ob = e_widget_check_add(evas, "Use Copy (Ctrl-C)", &(cfdata->clip_copy));
  e_widget_framelist_object_append(of, ob);

  ob = e_widget_check_add(evas, "Use Primary (Selection)", &(cfdata->clip_select));
  e_widget_framelist_object_append(of, ob);

  e_widget_list_object_append(o, of, 1, 0, 0.5);
  /* History Config Section       */
  of = e_widget_framelist_add(evas, "History", 0);
  ob = e_widget_check_add(evas, "Save History", &(cfdata->persistence));
  e_widget_framelist_object_append(of, ob);

  ob = e_widget_check_add(evas, "Reverse order", &(cfdata->hist_reverse));
  e_widget_framelist_object_append(of, ob);

   ob = e_widget_label_add(evas, "Items in history (5-50):");
   e_widget_framelist_object_append(of, ob);
   ob = e_widget_entry_add(evas, &(cfdata->hist_items), NULL, NULL, NULL);
   e_widget_size_min_set(ob, 30, 28);
   e_widget_framelist_object_append(of, ob);

   ob = e_widget_label_add(evas, "Items label length (5-50):");
   e_widget_framelist_object_append(of, ob);
   ob = e_widget_entry_add(evas, &(cfdata->label_length), NULL, NULL, NULL);
   e_widget_size_min_set(ob, 30, 28);
   e_widget_framelist_object_append(of, ob);


  e_widget_list_object_append(o, of, 1, 0, 0.5);

  /* Miscellaneous Config Section */
  of = e_widget_framelist_add(evas, "Miscellaneous", 0);
  ob = e_widget_check_add(evas, "Trim Whitespace", &(cfdata->trim_ws));
  e_widget_framelist_object_append(of, ob);

  ob = e_widget_check_add(evas, "Trim Newlines", &(cfdata->trim_nl));
  e_widget_framelist_object_append(of, ob);

  ob = e_widget_check_add(evas, "Confirm before clearing history", &(cfdata->confirm_clear));
  e_widget_framelist_object_append(of, ob);

  e_widget_list_object_append(o, of, 1, 0, 0.5);
  return o;
}

E_Config_Dialog *
_config_clipboard_module(E_Container *con, const char *params )
{
  E_Config_Dialog *cfd;
  E_Config_Dialog_View *v;

  if(e_config_dialog_find("E", "settings/clipboard"))
    return NULL;
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
_basic_check_changed(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata)
{
  if (clipboard_config->clip_copy     != cfdata->clip_copy) return 1;
  if (clipboard_config->clip_select   != cfdata->clip_select) return 1;
  if (clipboard_config-> persistence  != cfdata-> persistence) return 1;
  if (clipboard_config-> hist_reverse != cfdata-> hist_reverse) return 1;
  if (clipboard_config-> hist_items   != atoi(cfdata-> hist_items)) return 1;
  if (clipboard_config-> label_length  != atoi(cfdata-> label_length)) return 1;
  if (clipboard_config->trim_ws       != cfdata->trim_ws) return 1;
  if (clipboard_config->trim_nl       != cfdata->trim_nl) return 1;
  if (clipboard_config->confirm_clear != cfdata->confirm_clear) return 1;
  return 0;
}

