#include <e.h>
#include "e_mod_main.h"

/* This function adapted from Elive's e17 module productivity.
 *   https://github.com/Elive/emodule-productivity
 *   Thanks to princeamd for implementing this.
 * 
 * The MIT License:
 *   https://opensource.org/licenses/MIT
 */

void
e_mod_log_cb(const Eina_Log_Domain *d, Eina_Log_Level level, const char *file,
              const char *fnc, int line, const char *fmt, void *data EINA_UNUSED, va_list args)
{
  if ((d->name) && (d->namelen == sizeof(clipboard_config->log_name) - 1) &&
       (memcmp(d->name, clipboard_config->log_name, sizeof(clipboard_config->log_name) - 1) == 0))
  {
    const char *prefix;
    Eina_Bool use_color = !eina_log_color_disable_get();

    if (use_color)
      fputs(eina_log_level_color_get(level), stderr);

    switch (level) {
      case EINA_LOG_LEVEL_CRITICAL:
        prefix = "Critical. ";
        break;
      case EINA_LOG_LEVEL_ERR:
        prefix = "Error. ";
        break;
      case EINA_LOG_LEVEL_WARN:
        prefix = "Warning. ";
        break;
      default:
        prefix = "";
    }
    fprintf(stderr, "%s: %s", "E_CLIPBOARD", prefix);

    if (use_color)
      fputs(EINA_COLOR_RESET, stderr);

    vfprintf(stderr, fmt, args);
    putc('\n', stderr);
  } else
    eina_log_print_cb_stderr(d, level, file, fnc, line, fmt, NULL, args);
}


 
