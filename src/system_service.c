#include <dbus/dbus.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void handle_method_call(DBusConnection *conn, DBusMessage *msg)
{
    if (dbus_message_is_method_call(msg, "com.example.SystemInterface", "TestMethod")) {
        DBusMessage *reply;
        DBusMessageIter args;
        const char *param = "Hello from System Bus";
        dbus_uint32_t serial = 0;

        reply = dbus_message_new_method_return(msg);
        dbus_message_iter_init_append(reply, &args);
        if (!dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &param)) {
            fprintf(stderr, "Out Of Memory!\n");
            exit(1);
        }

        if (!dbus_connection_send(conn, reply, &serial)) {
            fprintf(stderr, "Out Of Memory!\n");
            exit(1);
        }

        dbus_connection_flush(conn);
        dbus_message_unref(reply);
    }
}

int main()
{
    DBusError err;
    DBusConnection *conn;
    int ret;

    // Initialize the errors
    dbus_error_init(&err);

    // Connect to the system bus
    conn = dbus_connection_open_private("unix:path=/var/run/dbus/system_bus_socket", &err);
    if (dbus_error_is_set(&err)) {
        fprintf(stderr, "Connection Error (%s)\n", err.message);
        dbus_error_free(&err);
    }
    if (conn == NULL) {
        return 1;
    }

    if (!dbus_bus_register(conn, &err)) {
        fprintf(stderr, "Register Error (%s)\n", err.message);
        dbus_error_free(&err);
        return 1;
    }

    // Request a name on the bus
    // request a bus name?
    ret = dbus_bus_request_name(conn, "com.example.SystemService", DBUS_NAME_FLAG_REPLACE_EXISTING, &err);
    if (dbus_error_is_set(&err)) {
        fprintf(stderr, "Name Error (%s)\n", err.message);
        dbus_error_free(&err);
    }
    if (ret != DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER) {
        fprintf(stderr, "Not Primary Owner (%d)\n", ret);
        return 1;
    }

    // Loop, waiting for method calls
    while (dbus_connection_read_write(conn, -1)) {
        DBusMessage *msg;

        msg = dbus_connection_pop_message(conn);

        if (msg == NULL) {
            sleep(1);
            continue;
        }

        handle_method_call(conn, msg);
        dbus_message_unref(msg);
    }

    return 0;
}
