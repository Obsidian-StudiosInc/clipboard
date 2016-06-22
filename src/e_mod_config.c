#include "e_mod_main.h"

struct _E_Config_Dialog_Data
{
  E_Config_Dialog *cfd;
  Evas_Object *obj;
  int persistence;
};

static int           _basic_apply_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata);
static void *        _create_data(E_Config_Dialog *cfd);
static void          _free_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata);
static void          _cb_first_setting_change(void *data, Evas_Object *obj);
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
  E_FREE(cfdata);
}

static void
_fill_data(E_Config_Dialog_Data *cfdata)
{
  cfdata->persistence = clipboard_config->persistence;
}

static int
_basic_apply_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata)
{
   clipboard_config->persistence = cfdata->persistence;
   e_config_save();
   return 1;
}

static Evas_Object *
_basic_create_widgets(E_Config_Dialog *cfd , Evas *evas, E_Config_Dialog_Data *cfdata)
{
  Evas_Object *o, *ob, *of;

  o = e_widget_list_add(evas, 0, 0);

  of = e_widget_framelist_add(evas, "History", 0);
  ob = e_widget_check_add(evas, "Save History", &(cfdata->persistence));

  cfdata->obj = ob;
  e_widget_on_change_hook_set(ob, _cb_first_setting_change, cfdata);
  e_widget_framelist_object_append(of, ob);
  e_widget_list_object_append(o, of, 1, 0, 0.5);

  e_dialog_resizable_set(cfd->dia, 0);
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

  cfd = e_config_dialog_new(con, "Clipboard Settings",
            "E", "preferences/clipboard",
            "preferences-engine", 0, v, NULL);
  e_dialog_resizable_set(cfd->dia, 1);
  clipboard_config->cfd = cfd;

  return cfd;
}

static void
_cb_first_setting_change(void *data, Evas_Object *obj)
{
  E_Config_Dialog_Data *cfdata;
  int val;

  cfdata = data;
  val = e_widget_check_checked_get(obj);

  cfdata->persistence = val;
  e_widget_check_checked_set(cfdata->obj,val);
}
