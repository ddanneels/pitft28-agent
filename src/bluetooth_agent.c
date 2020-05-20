/*
 * gcc `pkg-config --cflags glib-2.0 gio-2.0` -Wall -Wextra -o ./bin/agent ./agent.c `pkg-config --libs glib-2.0 gio-2.0`
 */
#include <glib.h>
#include <gio/gio.h>
#include <stdio.h>

GMainLoop *loop;
GDBusConnection *con;
#define AGENT_PATH  "/org/bluez/AutoPinAgent"

static void bluez_agent_method_call(GDBusConnection *conn,
                    const gchar *sender,
                    const gchar *path,
                    const gchar *interface,
                    const gchar *method,
                    GVariant *params,
                    GDBusMethodInvocation *invocation,
                    void *userdata)
{
    int pass;
    int entered;
    char *opath;
    GVariant *p= g_dbus_method_invocation_get_parameters(invocation);

    g_print("Agent method call: %s.%s()\n", interface, method);
    if(!strcmp(method, "RequestPinCode")) {
        ;
    }
    else if(!strcmp(method, "DisplayPinCode")) {
        ;
    }
    else if(!strcmp(method, "RequestPasskey")) {
        g_print("Getting the Pin from user: ");
        fscanf(stdin, "%d", &pass);
        g_print("\n");
        g_dbus_method_invocation_return_value(invocation, g_variant_new("(u)", pass));
    }
    else if(!strcmp(method, "DisplayPasskey")) {
        g_variant_get(params, "(ouq)", &opath, &pass, &entered);
        g_dbus_method_invocation_return_value(invocation, NULL);
    }
    else if(!strcmp(method, "RequestConfirmation")) {
        g_variant_get(params, "(ou)", &opath, &pass);
        g_dbus_method_invocation_return_value(invocation, NULL);
    }
    else if(!strcmp(method, "RequestAuthorization")) {
        ;
    }
    else if(!strcmp(method, "AuthorizeService")) {
        ;
    }
    else if(!strcmp(method, "Cancel")) {
        ;
    }
    else
        g_print("We should not come here, unknown method\n");
}

static const GDBusInterfaceVTable agent_method_table = {
    .method_call = bluez_agent_method_call,
};

int bluez_register_agent(GDBusConnection *con)
{
    GError *error = NULL;
    guint id = 0;
    GDBusNodeInfo *info = NULL;

    static const gchar bluez_agent_introspection_xml[] =
        "<node name='/org/bluez/SampleAgent'>"
        "   <interface name='org.bluez.Agent1'>"
        "       <method name='Release'>"
        "       </method>"
        "       <method name='RequestPinCode'>"
        "           <arg type='o' name='device' direction='in' />"
        "           <arg type='s' name='pincode' direction='out' />"
        "       </method>"
        "       <method name='DisplayPinCode'>"
        "           <arg type='o' name='device' direction='in' />"
        "           <arg type='s' name='pincode' direction='in' />"
        "       </method>"
        "       <method name='RequestPasskey'>"
        "           <arg type='o' name='device' direction='in' />"
        "           <arg type='u' name='passkey' direction='out' />"
        "       </method>"
        "       <method name='DisplayPasskey'>"
        "           <arg type='o' name='device' direction='in' />"
        "           <arg type='u' name='passkey' direction='in' />"
        "           <arg type='q' name='entered' direction='in' />"
        "       </method>"
        "       <method name='RequestConfirmation'>"
        "           <arg type='o' name='device' direction='in' />"
        "           <arg type='u' name='passkey' direction='in' />"
        "       </method>"
        "       <method name='RequestAuthorization'>"
        "           <arg type='o' name='device' direction='in' />"
        "       </method>"
        "       <method name='AuthorizeService'>"
        "           <arg type='o' name='device' direction='in' />"
        "           <arg type='s' name='uuid' direction='in' />"
        "       </method>"
        "       <method name='Cancel'>"
        "       </method>"
        "   </interface>"
        "</node>";

    info = g_dbus_node_info_new_for_xml(bluez_agent_introspection_xml, &error);
    if(error) {
        g_printerr("Unable to create node: %s\n", error->message);
        g_clear_error(&error);
        return 0;
    }

    id = g_dbus_connection_register_object(con, 
            AGENT_PATH,
            info->interfaces[0],
            &agent_method_table,
            NULL, NULL, &error);
    g_dbus_node_info_unref(info);
    //g_dbus_connection_unregister_object(con, id);
    /* call register method in AgentManager1 interface */
    return id;
}

static int bluez_agent_call_method(const gchar *method, GVariant *param)
{
    GVariant *result;
    GError *error = NULL;

    result = g_dbus_connection_call_sync(con,
            "org.bluez",
            "/org/bluez",
            "org.bluez.AgentManager1",
            method,
            param,
            NULL,
            G_DBUS_CALL_FLAGS_NONE,
            -1,
            NULL,
            &error);
    if(error != NULL) {
        g_print("Register %s: %s\n", AGENT_PATH, error->message);
        return 1;
    }

    g_variant_unref(result);
    return 0;
}

static int bluez_register_autopair_agent(const char *cap)
{
    int rc;

    rc = bluez_agent_call_method("RegisterAgent", g_variant_new("(os)", AGENT_PATH, cap));
    if(rc)
        return 1;

    rc = bluez_agent_call_method("RequestDefaultAgent", g_variant_new("(o)", AGENT_PATH));
    if(rc) {
        bluez_agent_call_method("UnregisterAgent", g_variant_new("(o)", AGENT_PATH));
        return 1;
    }

    return 0;
}


static void cleanup_handler(int signo)
{
    if (signo == SIGINT) {
        g_print("received SIGINT\n");
        g_main_loop_quit(loop);
    }
}

int main(int argc, char **argv)
{
    int id;
    int rc;

    if(argc < 2)
        return 1;

    if(signal(SIGINT, cleanup_handler) == SIG_ERR)
        g_print("can't catch SIGINT\n");

    con = g_bus_get_sync(G_BUS_TYPE_SYSTEM, NULL, NULL);
    if(con == NULL) {
        g_print("Not able to get connection to system bus\n");
        return 1;
    }

    loop = g_main_loop_new(NULL, FALSE);

    id = bluez_register_agent(con);
    if(id == 0)
        goto fail;

    rc = bluez_register_autopair_agent(argv[1]);
    if(rc) {
        g_print("Not able to register default autopair agent\n");
        goto fail;
    }

    g_main_loop_run(loop);

fail:
    g_dbus_connection_unregister_object(con, id);
    g_object_unref(con);
    return 0;
}