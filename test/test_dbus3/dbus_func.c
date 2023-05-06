#include "dbus_func.h"

#if 0
struct DBusError
{
  const char *name;    /**< public error name field */
  const char *message; /**< public error message field */

  unsigned int dummy1 : 1; /**< placeholder */
  unsigned int dummy2 : 1; /**< placeholder */
  unsigned int dummy3 : 1; /**< placeholder */
  unsigned int dummy4 : 1; /**< placeholder */
  unsigned int dummy5 : 1; /**< placeholder */

  void *padding1; /**< placeholder */
};
#endif
/*  
 * void dbus_error_init (DBusError *error)
 * 
*/

int init_dbus(char *request_bus_name, DBusConnection **conn, char **unique_bus_name)
{
    DBusError dbus_error;
    DBusConnection *connection = NULL;

/****************************************************************************************/
/*  创建 session D-Bus*/
    dbus_error_init(&dbus_error);
    connection = dbus_bus_get_private(DBUS_BUS_SESSION, &dbus_error);

	if (dbus_error_is_set(&dbus_error)) {
		fprintf(stderr, "[dbus_base]: Connection Error(%s)\n", dbus_error.message);
		dbus_error_free(&dbus_error);
        return -1;
	}
	if (NULL == connection) {
		fprintf(stderr, "[dbus_base]: connection point to NULL\n");
        return -1;
	}

    //dbus_bus_add_match (connection, "type='signal',interface='org.freedesktop.DBus',member='NameOwnerChanged'", &dbus_error);

    char *unique_name = NULL;
    unique_name = dbus_bus_get_unique_name(connection);
    printf("init_dbus unique name:%s\n",unique_name);
    *unique_bus_name = unique_name;
    
/****************************************************************************************/
/*  注册公共名 */
    if (request_bus_name != NULL) {
        printf("request bus name:%s\n",request_bus_name);
	    dbus_error_init(&dbus_error);
        int ret = dbus_bus_request_name(connection, request_bus_name, DBUS_NAME_FLAG_ALLOW_REPLACEMENT, &dbus_error);
        if (dbus_error_is_set(&dbus_error)) {
            fprintf(stderr, "[dbus_base]: Name Error (%s)\n", dbus_error.message);
            dbus_error_free(&dbus_error);
            return -1;
        }
        if (DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER != ret) {
            fprintf(stderr, "[dbus_base]: Not Primary Owner (%d)\n", ret);
            return -1;
        }
    }

    *conn = connection;
    return 0;
}

int send_dbus_data(void) 
{

}