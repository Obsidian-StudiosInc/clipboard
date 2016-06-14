#ifndef CLIPBOARD_HISTORY_H
#define CLIPBOARD_HISTORY_H

#include "common.h"
#include "utility.h"

Eet_Error read_history(Instance* inst);
Eet_Error save_history(Instance* inst);

#endif
