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

int init_dbus(char *request_bus_name, DBusConnection **conn)
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
/****************************************************************************************/
// DBUS_NAME_FLAG_ALLOW_REPLACEMENT  1     DBUS_NAME_FLAG_REPLACE_EXISTING 2    DBUS_NAME_FLAG_DO_NOT_QUEUE 4
/*  注册公共名 */
	dbus_error_init(&dbus_error);
	int ret = dbus_bus_request_name(connection, request_bus_name, DBUS_NAME_FLAG_REPLACE_EXISTING, &dbus_error);
    if (dbus_error_is_set(&dbus_error)) {
        fprintf(stderr, "[dbus_base]: Name Error (%s)\n", dbus_error.message);
        dbus_error_free(&dbus_error);
        return -1;
    }
    if (DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER != ret) {
        fprintf(stderr, "[dbus_base]: Not Primary Owner (%d)\n", ret);
        return -1;
    }

    *conn = connection;
    return 0;
}

int send_dbus_data(void) 
{

}