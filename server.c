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

#include <stdlib.h>
#include <dbus/dbus-glib-lowlevel.h>

typedef GObject      P2PObject;
typedef GObjectClass P2PObjectClass;

static GType    p2p_object_get_type (void); /* exported type */
static gboolean p2p_object_echo     (P2PObject* self,
                                     gchar    * text_in,
                                     gchar    **text_out,
                                     GError   **error);

#include "glue.h"

/* stolen from dbus-glib */
#define DBUS_G_CONNECTION_FROM_CONNECTION(x)     ((DBusGConnection*) _DBUS_POINTER_SHIFT(x))
#define _DBUS_POINTER_SHIFT(p)                   ((void*) (((char*)p) + sizeof (void*)))

static void
new_connection_cb (DBusServer    * server,
                   DBusConnection* connection,
                   gpointer        user_data)
{
  dbus_int32_t  slot = -1;
  GObject     * object;
  if (!dbus_connection_allocate_data_slot (&slot))
    {
      g_warning ("error allocating data slot for DBusConnection");
      dbus_connection_close (connection);
      return;
    }

  dbus_connection_ref (connection);

  dbus_connection_set_allow_anonymous (connection, TRUE);
  dbus_connection_setup_with_g_main (connection, NULL);

  object = g_object_new (p2p_object_get_type (), NULL);
  dbus_g_connection_register_g_object (DBUS_G_CONNECTION_FROM_CONNECTION (connection),
                                       "/", object);
  dbus_connection_set_data (connection, slot,
                            object, g_object_unref);
}

guint16
my_dbus_server_get_port (DBusServer* server)
{
  DBusAddressEntry**entries;
  DBusError         error;
  guint16           port = 0;
  char            * address = dbus_server_get_address (server);
  int               n_entries;
  int               i;

  dbus_error_init (&error);
  if (!dbus_parse_address (address, &entries, &n_entries, &error))
    {
      g_warning ("error parsing server address: %s",
                 error.message);
      dbus_error_free (&error);
      free (address);
      exit (1);
      return 0;
    }

  for (i = 0; i < n_entries; i++)
    {
      if (!strcmp ("tcp", dbus_address_entry_get_method (entries[i])))
        {
          port = atoi (dbus_address_entry_get_value (entries[i], "port"));
          break;
        }
    }

  dbus_address_entries_free (entries);
  free (address);
  return port;
}

int
main (int   argc,
      char**argv)
{
  DBusServer* server;
  GMainLoop * loop;
  DBusError   derror;
  guint16     port;
  GError    * error = NULL;
  gchar     * command;

  g_type_init ();

  dbus_error_init (&derror);
  server = dbus_server_listen("tcp:bind=*", &derror);
  if (dbus_error_is_set (&derror))
    {
      g_warning ("error setting up peer-to-peer server: %s",
                 derror.message);
      dbus_error_free (&derror);
      return 1;
    }

  dbus_server_set_new_connection_function(server, &new_connection_cb,
                                          NULL, NULL);
  port = my_dbus_server_get_port (server);

  command = g_strdup_printf ("./client %s %d",
                             "localhost",
                             port);
  if (argc > 1)
    {
      g_print ("starting \"%s\" twice:\n", command);
      if (!g_spawn_command_line_async (command, &error))
        {
          g_warning ("error spawning child: %s",
                     error->message);
          g_clear_error (&error);
          return 1;
        }
      if (!g_spawn_command_line_async (command, &error))
        {
          g_warning ("error spawning child: %s",
                     error->message);
          g_clear_error (&error);
          return 1;
        }
    }
  else
    {
      g_print ("start the clients like this: \"%s\" (replace \"localhost\" with the IP address)\n",
               command);
    }
  g_free (command);

  dbus_server_setup_with_g_main (server, NULL);

  loop = g_main_loop_new (NULL, FALSE);
  g_timeout_add (argc > 1 ? 1000 : 60 * 1000, (GSourceFunc)g_main_loop_quit, loop);
  g_main_loop_run (loop);
  g_main_loop_unref (loop);

  dbus_server_unref (server);

  return 0;
}

/* implementation of the exported type */
G_DEFINE_TYPE (P2PObject, p2p_object, G_TYPE_OBJECT);

static void
p2p_object_init (P2PObject* self)
{}

static void
p2p_object_class_init (P2PObjectClass* self_class)
{
  dbus_g_object_type_install_info (G_TYPE_FROM_CLASS (self_class),
                                   &dbus_glib_p2p_object_object_info);
}

static gboolean
p2p_object_echo (P2PObject* self,
                 gchar    * text_in,
                 gchar    **text_out,
                 GError   **error)
{
  if (!G_TYPE_CHECK_INSTANCE_TYPE (self, p2p_object_get_type ()))
    {
      g_set_error (error, 0, 0, "invalid object type");
      return FALSE;
    }

  *text_out = g_strdup_printf ("%1$s %1$s", text_in);
  return TRUE;
}

/* vim:set et sw=2 cino=t0,f0,(0,{s,>2s,n-1s,^-1s,e2s: */
