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

#include "bind.h"

static GMainLoop* loop = NULL;

static gboolean
send_echo (gpointer  connection)
{
  DBusGProxy* proxy = dbus_g_proxy_new_for_peer (connection, "/", "eu.herzi.P2PObject");
  GError    * error = NULL;
  gchar     * echo;

  if (!eu_herzi_P2PObject_echo (proxy, "hello", &echo, &error))
    {
      g_warning ("error calling echo: %s", error->message);
      g_error_free (error);
    }
  else
    {
      g_print ("\"hello\" => \"%s\"\n", echo);
      g_free (echo);
    }
  g_object_unref (proxy);

  g_main_loop_quit (loop);

  return FALSE;
}

int
main (int   argc,
      char**argv)
{
  DBusGConnection* connection;
  GError         * error = NULL;
  gchar          * server_path;

  if (argc < 3)
    {
      g_error ("usage:\n  %s <HOST> <PORT>\n");
    }

  g_type_init ();

  server_path = g_strdup_printf ("tcp:host=%s,port=%s", argv[1], argv[2]);
  connection = dbus_g_connection_open (server_path, &error);
  g_free (server_path);

  if (error)
    {
      g_warning ("error opening connection to %s:%s: %s",
                 argv[1], argv[2], error->message);
      g_error_free (error);
      return 1;
    }

  g_idle_add (send_echo, connection);
  loop = g_main_loop_new (NULL, FALSE);
  g_main_loop_run (loop);
  g_main_loop_unref (loop);

  g_object_unref (connection);
  return 0;
}

/* vim:set et sw=2 cino=t0,f0,(0,{s,>2s,n-1s,^-1s,e2s: */
