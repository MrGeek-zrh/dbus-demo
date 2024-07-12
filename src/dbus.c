#include <dbus/dbus.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;
    DBusError dbus_error;
    DBusConnection *dbus_conn;
    DBusMessage *dbus_msg;
    DBusMessage *dbus_reply;
    const char *dbus_result;
    const char *checkpoint_arg = "com.example.SystemService"; // Example argument for the checkpoint function

    // Initialize D-Bus error
    dbus_error_init(&dbus_error);

    // Connect to D-Bus
    dbus_conn = dbus_bus_get(DBUS_BUS_SYSTEM, &dbus_error);
    if (dbus_conn == NULL) {
        fprintf(stderr, "Connection Error (%s): %s\n", dbus_error.name, dbus_error.message);
        dbus_error_free(&dbus_error);
        return 1;
    }

    // Compose remote procedure call
    dbus_msg = dbus_message_new_method_call("org.freedesktop.DBus", "/", "org.freedesktop.DBus.Introspectable",
                                            "checkpoint");
    if (dbus_msg == NULL) {
        dbus_connection_unref(dbus_conn);
        fprintf(stderr, "ERROR: dbus_message_new_method_call - Unable to allocate memory for the message!\n");
        return 1;
    }

    // Append arguments to the message
    if (!dbus_message_append_args(dbus_msg, DBUS_TYPE_STRING, &checkpoint_arg, DBUS_TYPE_INVALID)) {
        dbus_message_unref(dbus_msg);
        dbus_connection_unref(dbus_conn);
        fprintf(stderr, "ERROR: dbus_message_append_args - Unable to append arguments to the message!\n");
        return 1;
    }

    // Invoke remote procedure call, block for response
    dbus_reply = dbus_connection_send_with_reply_and_block(dbus_conn, dbus_msg, DBUS_TIMEOUT_USE_DEFAULT, &dbus_error);
    if (dbus_reply == NULL) {
        dbus_message_unref(dbus_msg);
        dbus_connection_unref(dbus_conn);
        fprintf(stderr, "Reply Error (%s): %s\n", dbus_error.name, dbus_error.message);
        dbus_error_free(&dbus_error);
        return 1;
    }

    // Parse response
    if (!dbus_message_get_args(dbus_reply, &dbus_error, DBUS_TYPE_STRING, &dbus_result, DBUS_TYPE_INVALID)) {
        dbus_message_unref(dbus_msg);
        dbus_message_unref(dbus_reply);
        dbus_connection_unref(dbus_conn);
        fprintf(stderr, "Argument Error (%s): %s\n", dbus_error.name, dbus_error.message);
        dbus_error_free(&dbus_error);
        return 1;
    }

    // Work with the results of the remote procedure call
    printf("Connected to D-Bus as \"%s\".\n", dbus_bus_get_unique_name(dbus_conn));
    printf("Introspection Result:\n\n");
    printf("%s\n", dbus_result);

    dbus_message_unref(dbus_msg);
    dbus_message_unref(dbus_reply);
    dbus_connection_unref(dbus_conn);

    return 0;
}
