#include "dbus/dbus-protocol.h"
#include <dbus/dbus.h>
#include <stdio.h>
#include <stdlib.h>

void get_pid_for_service(const char *service_name)
{
    DBusError err;
    DBusConnection *conn;
    DBusMessage *msg;
    DBusMessage *reply;

    // Initialize the error
    dbus_error_init(&err);

    // Connect to the system bus
    conn = dbus_bus_get(DBUS_BUS_SYSTEM, &err);
    if (dbus_error_is_set(&err)) {
        fprintf(stderr, "Connection Error (%s)\n", err.message);
        dbus_error_free(&err);
        return;
    }
    if (conn == NULL) {
        return;
    }

    // Create a new method call and check for errors
    msg = dbus_message_new_method_call("org.freedesktop.DBus", // target for the method call
                                       "/org/freedesktop/DBus", // object to call on
                                       "org.freedesktop.DBus", // interface to call on
                                       "Checkpoint"); // method name

    if (msg == NULL) {
        fprintf(stderr, "Message Null\n");
        return;
    }

    // Append arguments
    DBusMessageIter args;
    dbus_message_iter_init_append(msg, &args);
    if (!dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &service_name)) {
        fprintf(stderr, "Out Of Memory!\n");
        return;
    }

    // Send message and get a reply
    reply = dbus_connection_send_with_reply_and_block(conn, msg, -1, &err);
    dbus_message_unref(msg);

    if (dbus_error_is_set(&err)) {
        fprintf(stderr, "Error (%s)\n", err.message);
        dbus_error_free(&err);
        return;
    }

    char * fallback;
    // Read the reply
    if (!dbus_message_iter_init(reply, &args)) {
        fprintf(stderr, "Reply has no arguments!\n");
    } else if (DBUS_TYPE_STRING != dbus_message_iter_get_arg_type(&args)) {
        fprintf(stderr, "Argument is not string!\n");
    } else {
        dbus_message_iter_get_basic(&args, &fallback);
        printf("The PID of service '%s' is %s\n", service_name, fallback);
    }

    dbus_message_unref(reply);
    dbus_connection_unref(conn);
}

int main()
{
    // Replace this with the actual service name or connection name
    const char *service_name = "com.example.SystemService";
    get_pid_for_service(service_name);
    return 0;
}
