/* This file is part of herzi's playground
 *
 * Copyright (C) 2010  Sven Herzberg
 *
 * This work is provided "as is"; redistribution and modification
 * in whole or in part, in any medium, physical or electronic is
 * permitted without restriction.
 *
 * This work is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * In no event shall the authors or contributors be liable for any
 * direct, indirect, incidental, special, exemplary, or consequential
 * damages (including, but not limited to, procurement of substitute
 * goods or services; loss of use, data, or profits; or business
 * interruption) however caused and on any theory of liability, whether
 * in contract, strict liability, or tort (including negligence or
 * otherwise) arising in any way out of the use of this software, even
 * if advised of the possibility of such damage.
 */

#include <dbus/dbus-glib.h>

int
main (int   argc,
      char**argv)
{
  guint16  port;
  GError * error = NULL;
  gchar  * command;

  command = g_strdup_printf ("./client %s %d",
                             "localhost",
                             port);
  if (!g_spawn_command_line_async (command, &error))
    {
      g_warning ("error spawning child: %s",
                 error->message);
      g_clear_error (&error);
    }
  if (!g_spawn_command_line_async (command, &error))
    {
      g_warning ("error spawning child: %s",
                 error->message);
      g_clear_error (&error);
    }
  g_free (command);

  return 0;
}

/* vim:set et sw=2 cino=t0,f0,(0,{s,>2s,n-1s,^-1s,e2s: */
