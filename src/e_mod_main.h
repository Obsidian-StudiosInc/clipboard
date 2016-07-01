#ifndef E_MOD_MAIN_H
#define E_MOD_MAIN_H

#include "common.h"
#include "e_mod_config.h"
#include "history.h"
#include "utility.h"

/* Module Requirements */
EAPI extern E_Module_Api e_modapi;

EAPI void *e_modapi_init     (E_Module *m);
EAPI int   e_modapi_shutdown (E_Module *m __UNUSED__);
EAPI int   e_modapi_save     (E_Module *m __UNUSED__);

#endif
