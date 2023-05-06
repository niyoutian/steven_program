#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "dbus_func.h"

pthread_t g_server_recv_id;
pthread_t g_server_send_id;

char *g_client_bus_id[10];

struct server_dbus_data
{
    char  *bus_name;
	char  *object_path_name;
	char  *interface_name;
	char  *method_name;
	char  *request_bus_name;
    char  *unique_bus_name;
    
	DBusError dbus_error;				
	DBusConnection *connection;
    DBusMessage *message;

    /*pthread_mutex_t dbus_recv_mutex;
    uint8_t recvdata[RECV_DATA_SIZE];
    int32_t recvpos;
    int32_t recvlen;*/
};

/*
https://www.freesion.com/article/8257927105/
https://www.codeprj.com/blog/edc6071.html

dbus_bool_t dbus_connection_read_write	(	DBusConnection * 	connection, int 	timeout_milliseconds )	
Parameters
connection	the connection
timeout_milliseconds	max time to block or -1 for infinite
https://dbus.freedesktop.org/doc/api/html/group__DBusConnection.html#ga371163b4955a6e0bf0f1f70f38390c14
https://vimsky.com/examples/detail/cpp-ex-----dbus_message_get_path-function.html



*/
struct server_dbus_data g_server_data;

void save_client_bus_id(char *bus_id) 
{
    int found = 0;
    int add = 0;
    int i = 0;

    for(i = 0; i < 10; i++) {
        if (g_client_bus_id[i] == NULL) {
            continue;
        }
        if (strncmp(g_client_bus_id[i], bus_id, 12) == 0) {
            found = 1;
        }
    }

    if (found == 1) {
        printf("client id:%s already exist\n",bus_id);
    } else {
        add = 0;
        for(i = 0; i < 10; i++) {
            if (g_client_bus_id[i] == NULL) {
                g_client_bus_id[i] = (char *)malloc(11);
                strncpy(g_client_bus_id[i], bus_id, 10);
                add = 1;
                break;
            }
        }
        if(add ==1) {
            printf("index %d add client bus id:%s\n",i, g_client_bus_id[i]);
        } else{
            printf("add client bus id:%s failed\n",bus_id);
        }
    }

}
void *server_dbus_recv(void *arg)
{
    DBusError dbus_error;	
    DBusMessage* msg = NULL;
	DBusMessageIter ags;
	DBusMessageIter subags;
	void* ptr;
    const char * sender = NULL;
    const char * dst = NULL;
    u_int32_t serial = 0;
    const char * path = NULL;
    const char * intf = NULL;
    const char * name = NULL;
    register_app_t *register_app = NULL;
    int n_elements = 0;
    int ret = 0;

    struct server_dbus_data *server_data = (struct server_dbus_data *)arg;

    dbus_error_init(&dbus_error); 
	while (1) {
        //0 非阻塞监听信号; -1 阻塞
		dbus_connection_read_write(g_server_data.connection, 0);
		msg = dbus_connection_pop_message(g_server_data.connection);
		if (NULL == msg) {
            sleep(1);
			continue;
		}
        sender = dbus_message_get_sender(msg);
        printf("****************sender %s\n",sender);

        dst = dbus_message_get_destination(msg);
        if (dst != NULL) {
            printf("destination %s\n",dst);
        } else {
             printf("destination is NULL\n");
        }

        serial = dbus_message_get_serial(msg);
        printf("serial %d\n",serial);

        path = dbus_message_get_path(msg);
        if(NULL==path) {
            continue;
        }
        printf("server recv path %s\n",path);
        // if (strncmp(path, g_server_data.object_path_name, strlen(g_server_data.object_path_name))) {
        //     continue;
        // }

        intf = dbus_message_get_interface(msg);
        if(NULL==intf) {
            continue;
        }
        printf("server recv interface %s\n",intf);

        name = dbus_message_get_member(msg);
        if(NULL==name) {
            continue;
        }
        printf("server recv member %s\n",name);

        if (dbus_message_is_signal(msg, DBUS_INTERFACE_LOCAL,"Disconnected")) {
            printf("Disconnected\n");
        }
        else if (dbus_message_is_signal(msg, DBUS_INTERFACE_DBUS,"NameOwnerChanged")) {

            printf("NameOwnerChanged\n");
			if (!dbus_message_iter_init(msg, &ags)) {
				fprintf(stderr, "Message has no arguments\n");
			}
            do {
                ret = dbus_message_iter_get_arg_type(&ags);
                printf("message type:%d\n",ret);
                if (ret != DBUS_TYPE_STRING) {
                    printf("message type error:%d\n",ret);
                }
                char *pdata = NULL;
                dbus_message_iter_get_basic(&ags, &pdata);
                printf("data len:%d\n",strlen(pdata));
                if (pdata == NULL) {
                    printf("data is NULL\n");
                } else {
                    printf("data:%s\n",pdata);
                }
            } while(dbus_message_iter_next(&ags));
        }
        else if(dbus_message_is_method_call(msg,g_server_data.interface_name ,g_server_data.method_name)) {
            //printf("receive method call %s\n",g_server_data.method_name);

			if (!dbus_message_iter_init(msg, &ags)) {
				fprintf(stderr, "Message has no arguments\n");
			}
			dbus_message_iter_recurse(&ags, &subags);
			dbus_message_iter_get_fixed_array(&subags, &ptr, &n_elements);
			register_app = (register_app_t*)ptr;
			printf("[register_app]: receive a struct : app_ip = 0x%04x, bus_id=%s, size = %d\n",
                     register_app->app_ip,  register_app->bus_id, register_app->size);
            save_client_bus_id(register_app->bus_id);
            
        }
        dbus_message_unref(msg);
    }  

}

//SD-->SOMEIP
void *server_dbus_send(void *arg)
{
    DBusMessage *msg = NULL;
    DBusPendingCall *pending;
    DBusMessageIter sendIter;
	DBusMessageIter arrayIter;
    offer_service_t app_data = {0,0};
    offer_service_t *p_data = NULL;
    bool ret = false;
    unsigned char count = 0;

    

    while (1) {
        for (int i = 0; i< 10; i++) {
            if (g_client_bus_id[i] == NULL) {
                continue;
            }
            //Constructs a new message to invoke a method on a remote object.    
            msg = dbus_message_new_method_call(g_client_bus_id[i], ESOMEIP_DBUS_PATH, ESOMEIP_DBUS_I_APP, ESOMEIP_DBUS_M_ON_OFFER);
            if(msg == NULL){
                fprintf(stderr, "MessageNULL");
                return NULL;
            }

            app_data.app_ip = 0x12;
            app_data.service_id = 0x4001;
            app_data.instance_id = 1;
            app_data.ttl[0] = count++;
            app_data.ttl[1] = 0;
            app_data.ttl[2] = 0;
            app_data.size = sizeof(app_data);
            p_data = &app_data;
            //为消息添加参数。Appendarguments
            dbus_message_set_no_reply(msg, true);
            dbus_message_iter_init_append(msg, &sendIter);
            ret = dbus_message_iter_open_container(&sendIter, DBUS_TYPE_ARRAY, DBUS_TYPE_BYTE_AS_STRING, &arrayIter);
            ret = dbus_message_iter_append_fixed_array(&arrayIter, DBUS_TYPE_BYTE, &p_data, sizeof(app_data));
            ret = dbus_message_iter_close_container(&sendIter, &arrayIter);
            if (!dbus_connection_send(g_server_data.connection , msg, NULL)) {
                fprintf(stderr, "[dbus_base]: esomeip_send_msg Out Of Memory!\n");
            }
            //printf("server_dbus_send 4\n");
            dbus_connection_flush(g_server_data.connection);
            //printf("server_dbus_send 5\n");
            dbus_message_unref(msg);
            printf("send data to %s,%s,%s,%s\n",g_client_bus_id[i],ESOMEIP_DBUS_PATH,ESOMEIP_DBUS_I_APP,ESOMEIP_DBUS_M_ON_OFFER);
        }
        sleep(5);
    }
}


void test_add_watch(void)
{
    DBusMessage *msg = NULL;
    DBusMessageIter sendIter;
    //const char *watch ="type='signal',sender='org.freedesktop.DBus',interface='org.freedesktop.DBus',member='NameOwnerChanged'";
    //const char *watch ="type='signal',interface='org.freedesktop.DBus',member='NameOwnerChanged'";   //OK
    //const char *watch ="type='signal',sender='org.freedesktop.DBus',path='/org/freedesktop/DBus',interface='org.freedesktop.DBus',member='NameOwnerChanged'";
    const char *watch ="type='signal',sender='org.freedesktop.DBus',path='/org/freedesktop/DBus',interface='org.freedesktop.DBus',member='NameOwnerChanged'";

    msg = dbus_message_new_method_call(DBUS_SERVICE_DBUS, DBUS_PATH_DBUS, DBUS_INTERFACE_DBUS, "AddMatch");
    if(msg == NULL){
        fprintf(stderr, "MessageNULL");
        return NULL;
    }
    dbus_message_set_no_reply(msg, true);
    dbus_message_iter_init_append(msg, &sendIter);/*  为消息添加参数           */
    dbus_message_iter_append_basic(&sendIter, DBUS_TYPE_STRING, &watch);
    if (!dbus_connection_send(g_server_data.connection , msg, NULL)) {
        fprintf(stderr, "[dbus_base]: esomeip_send_msg Out Of Memory!\n");
    }

    dbus_connection_flush(g_server_data.connection);
    dbus_message_unref(msg);   /*  释放消息              */
    printf("send data \n");

}

static dbus_bool_t become_monitor(DBusConnection *connection, int numFilters, const char * const *filters)
{
  DBusError error = DBUS_ERROR_INIT;
  DBusMessage *m;
  DBusMessage *r;
  int i;
  dbus_uint32_t zero = 0;
  DBusMessageIter appender, array_appender;

  m = dbus_message_new_method_call (DBUS_SERVICE_DBUS,
      DBUS_PATH_DBUS, DBUS_INTERFACE_MONITORING, "BecomeMonitor");

  if (m == NULL)
    printf("becoming a monitor");

  dbus_message_iter_init_append (m, &appender);

  if (!dbus_message_iter_open_container (&appender, DBUS_TYPE_ARRAY, "s",
        &array_appender))
    printf ("opening string array");

  for (i = 0; i < numFilters; i++)
    {
      if (!dbus_message_iter_append_basic (&array_appender, DBUS_TYPE_STRING,
            &filters[i]))
        printf ("adding filter to array");
    }

  if (!dbus_message_iter_close_container (&appender, &array_appender) ||
      !dbus_message_iter_append_basic (&appender, DBUS_TYPE_UINT32, &zero))
    printf ("finishing arguments");

  r = dbus_connection_send_with_reply_and_block (connection, m, -1, &error);

  if (r != NULL)
    {
      dbus_message_unref (r);
    }
  else if (dbus_error_has_name (&error, DBUS_ERROR_UNKNOWN_INTERFACE))
    {
      fprintf (stderr, "dbus-monitor: unable to enable new-style monitoring, "
          "your dbus-daemon is too old. Falling back to eavesdropping.\n");
      dbus_error_free (&error);
    }
  else
    {
      fprintf (stderr, "dbus-monitor: unable to enable new-style monitoring: "
          "%s: \"%s\". Falling back to eavesdropping.\n",
          error.name, error.message);
      dbus_error_free (&error);
    }

  dbus_message_unref (m);

  return (r != NULL);
}
// dbus-send --session --print-reply --type=method_call --dest=dbus.method.daemon1 /esomeip/method/Object  esomeip.app.register  string:'abc'
// dbus-send --session --print-reply --type=method_call --dest=dbus.method.daemon1 /esomeip/method/Object  esomeip.app.register  byte:0x01
// dbus-send [--system | --session] --type=method_call --print-reply --dest=连接名 对象路径 接口名.方法名 参数类型:参数值 参数类型:参数值
// dbus-send支持的参数类型包括：string, int32, uint32, double, byte, boolean。
// dbus-send --session --type=method_call --print-reply --dest=org.freedesktop.DBus / org.freedesktop.DBus.Introspectable.Introspect
// dbus-send --session --type=method_call --print-reply --dest=org.freedesktop.DBus / org.freedesktop.DBus.ListNames
int test_server_case1(void)
{
    g_server_data.request_bus_name = ESOMEIP_SD_DBUS_NAME;
    g_server_data.object_path_name = ESOMEIP_DBUS_PATH;
    g_server_data.interface_name = ESOMEIP_DBUS_I_APP;
    g_server_data.method_name = ESOMEIP_DBUS_M_REGISTER;
    init_dbus(g_server_data.request_bus_name, &g_server_data.connection, &g_server_data.unique_bus_name);
    DBusError dbus_error;	
    dbus_error_init(&dbus_error); 
    dbus_bus_add_match (g_server_data.connection, "type='signal',interface='org.freedesktop.DBus',member='NameOwnerChanged'", &dbus_error);
    //test_add_watch();

    //dbus_connection_set_route_peer_messages (g_server_data.connection, TRUE);

    // bool b_ret = false;
    // int numFilters = 0;
    // char **filters = NULL;
    // b_ret = become_monitor(g_server_data.connection, numFilters, (const char * const *)filters);
    // printf("b_ret=%d\n",b_ret);

    pthread_create(&g_server_recv_id, NULL, server_dbus_recv, &g_server_data);
    // pthread_create(&g_server_send_id, NULL, server_dbus_send, &g_server_data);
    pthread_join(g_server_recv_id, NULL);
    // pthread_join(g_server_send_id, NULL);
    printf("exit \n");

    return 0;
}

int main(int argc, char *const argv[])
{
    printf("enter dbus server\n");

    test_server_case1();

    return 0;
}