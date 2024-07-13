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

    // 解析回复消息
    if (dbus_message_iter_init(reply, &args)) {
        if (dbus_message_iter_get_arg_type(&args) == DBUS_TYPE_ARRAY) {
            DBusMessageIter sub;
            dbus_message_iter_recurse(&args, &sub);

            while (dbus_message_iter_get_arg_type(&sub) != DBUS_TYPE_INVALID) {
                if (dbus_message_iter_get_arg_type(&sub) == DBUS_TYPE_STRING) {
                    const char *name;
                    dbus_message_iter_get_basic(&sub, &name);
                    printf("Service: %s\n", name);
                }
                dbus_message_iter_next(&sub);
            }
        }
    }

    // 释放回复消息
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
