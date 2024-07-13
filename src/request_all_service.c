#include <dbus/dbus.h>
#include <stdio.h>
#include <stdlib.h>

void list_names(DBusConnection *connection) {
    DBusMessage *message;
    DBusMessage *reply;
    DBusError error;
    DBusMessageIter args;

    // 初始化错误
    dbus_error_init(&error);

    // 创建一个方法调用消息，调用 "ListNames" 方法
    message = dbus_message_new_method_call("org.freedesktop.DBus",
                                           "/org/freedesktop/DBus",
                                           "org.freedesktop.DBus",
                                           "ListNames");

    if (message == NULL) {
        fprintf(stderr, "Message Null\n");
        exit(1);
    }

    // 发送消息并等待回复
    reply = dbus_connection_send_with_reply_and_block(connection, message, -1, &error);

    // 释放消息
    dbus_message_unref(message);

    // 检查错误
    if (dbus_error_is_set(&error)) {
        fprintf(stderr, "Error: %s\n", error.message);
        dbus_error_free(&error);
        exit(1);
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
}

int main() {
    DBusConnection *connection;
    DBusError error;

    // 初始化错误
    dbus_error_init(&error);

    // 连接到系统总线
    connection = dbus_bus_get(DBUS_BUS_SYSTEM, &error);

    if (dbus_error_is_set(&error)) {
        fprintf(stderr, "Connection Error: %s\n", error.message);
        dbus_error_free(&error);
        exit(1);
    }

    if (connection == NULL) {
        fprintf(stderr, "Connection Null\n");
        exit(1);
    }

    // 列出所有服务
    list_names(connection);

    // 关闭连接
    dbus_connection_unref(connection);

    return 0;
}
