#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <dbus/dbus.h>

/* define bus name for someip */
#define ESOMEIP_DBUS_NAME							"dbus.method.esomeip"
#define ESOMEIP_SD_DBUS_NAME						"dbus.method.daemon1"

/* define bus object for someip */
#define ESOMEIP_DBUS_PATH							"/esomeip/method/Object"

/* define bus interface for someip */
#define ESOMEIP_DBUS_I_APP				"esomeip.app"

/* define bus method for someip */
#define ESOMEIP_DBUS_M_REGISTER						"register"

/* define bus method for someip sd*/
#define ESOMEIP_DBUS_M_ON_OFFER						"on_offer"

typedef struct _register_app_t {
	unsigned int app_ip;
	char bus_id[12];
	unsigned int  size;
}register_app_t;

typedef struct _offer_service_t {
	unsigned int app_ip;
	unsigned short service_id;
	unsigned short instance_id;
	unsigned char  ttl[3];
	unsigned char  size;
}offer_service_t;

//int init_dbus(char *request_bus_name, DBusConnection **conn);
int init_dbus(char *request_bus_name, DBusConnection **conn, char **unique_bus_name);