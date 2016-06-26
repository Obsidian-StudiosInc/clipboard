#ifndef CLIPBOARD_HISTORY_H
#define CLIPBOARD_HISTORY_H

#include "common.h"
#include "utility.h"

int item_num;

Eet_Error read_history(Instance* inst);
Eet_Error save_history(Instance* inst);

#endif
