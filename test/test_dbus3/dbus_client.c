#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "dbus_func.h"

pthread_t g_client_recv_id;
pthread_t g_client_send_id;


struct client_dbus_data
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

struct client_dbus_data g_client_data;

void *client_dbus_recv(void *arg)
{
    DBusError dbus_error;	
    DBusMessage* msg = NULL;
	DBusMessageIter ags;
	DBusMessageIter subags;
	void* ptr;
    const char * path = NULL;
    const char * intf = NULL;
    const char * name = NULL;
    offer_service_t *register_app = NULL;
    int n_elements = 0;

    struct client_dbus_data *server_data = (struct client_dbus_data *)arg;

    dbus_error_init(&dbus_error); 
	while (1) {
        //0 非阻塞监听信号; -1 阻塞
		dbus_connection_read_write(g_client_data.connection, 10);
		msg = dbus_connection_pop_message(g_client_data.connection);
		if (NULL == msg) {
			continue;
		}

        path = dbus_message_get_path(msg);
        if(NULL==path) {
            continue;
        }
        //printf("client recv path %s\n",path);
        if (strncmp(path, g_client_data.object_path_name, strlen(g_client_data.object_path_name))) {
            continue;
        }

        intf = dbus_message_get_interface(msg);
        if(NULL==intf) {
            continue;
        }
        //printf("client recv interface %s\n",intf);

        name = dbus_message_get_interface(msg);
        if(NULL==name) {
            continue;
        }
        //printf("client recv name %s\n",name);


        if(dbus_message_is_method_call(msg,g_client_data.interface_name ,g_client_data.method_name)){
            //printf("receive method call %s\n",g_client_data.method_name);
             /*  获取参数                */
			if (!dbus_message_iter_init(msg, &ags)) {
				fprintf(stderr, "Message has no arguments\n");
			}
			dbus_message_iter_recurse(&ags, &subags);
			dbus_message_iter_get_fixed_array(&subags, &ptr, &n_elements);
			register_app = (offer_service_t*)ptr;
			printf("[on_offer]: receive a struct : app_ip = 0x%04x, service_id = 0x%04x, instance_id = 0x%04x, ttl0 = %d, size = %d\n",
                    register_app->app_ip, register_app->service_id, register_app->instance_id, register_app->ttl[0], register_app->size);
            dbus_message_unref(msg);
        }
    }  

}

void *client_dbus_sent(void *arg)
{
    DBusMessage *msg = NULL;
    DBusPendingCall *pending;
    DBusMessageIter sendIter;
	DBusMessageIter arrayIter;
    register_app_t app_data = {0,0};
    register_app_t *p_data = NULL;
    bool ret = false;

    while (1) {
        memset(&app_data,0,sizeof(app_data));
        //Constructs a new message to invoke a method on a remote object.
         /*  创建一个函数调用消息    */
        msg = dbus_message_new_method_call(ESOMEIP_SD_DBUS_NAME, ESOMEIP_DBUS_PATH, ESOMEIP_DBUS_I_APP, ESOMEIP_DBUS_M_REGISTER);
        if(msg == NULL){
            fprintf(stderr, "MessageNULL");
            return NULL;
        }

        app_data.app_ip = 0x4001;
        printf("my bus_id = %s\n",g_client_data.unique_bus_name);
        strncpy(app_data.bus_id,g_client_data.unique_bus_name,sizeof(app_data.bus_id));
        app_data.size = sizeof(app_data);
        p_data = &app_data;
        //为消息添加参数。Appendarguments
        dbus_message_set_no_reply(msg, true);
        dbus_message_iter_init_append(msg, &sendIter);/*  为消息添加参数           */
        ret = dbus_message_iter_open_container(&sendIter, DBUS_TYPE_ARRAY, DBUS_TYPE_BYTE_AS_STRING, &arrayIter);
        ret = dbus_message_iter_append_fixed_array(&arrayIter, DBUS_TYPE_BYTE, &p_data, sizeof(app_data));
        ret = dbus_message_iter_close_container(&sendIter, &arrayIter);
/*         这个接口是非阻塞式的，单纯的只是把当前要发送的消息添加到系统发送队列然后返回。
        如果你想马上把消息发送出去可以调用dbus_connection_flush();该函数阻塞直到发送队列为空，这两个函数的结合恰好是dbus_connection_send_with_reply_and_block接口的作用。 */
        if (!dbus_connection_send(g_client_data.connection , msg, NULL)) {
            fprintf(stderr, "[dbus_base]: esomeip_send_msg Out Of Memory!\n");
        }

        dbus_connection_flush(g_client_data.connection);
        dbus_message_unref(msg);   /*  释放消息              */
        printf("send data to %s,%s,%s,%s from %s\n",ESOMEIP_SD_DBUS_NAME,ESOMEIP_DBUS_PATH,
                ESOMEIP_DBUS_I_APP,ESOMEIP_DBUS_M_REGISTER,app_data.bus_id);
        sleep(5);
    }
}

int test_client_case1(void)
{

    //g_client_data.request_bus_name = ESOMEIP_DBUS_NAME;
    g_client_data.object_path_name = ESOMEIP_DBUS_PATH;
    g_client_data.interface_name = ESOMEIP_DBUS_I_APP;
     g_client_data.method_name = ESOMEIP_DBUS_M_ON_OFFER;
    init_dbus(g_client_data.request_bus_name, &g_client_data.connection, &g_client_data.unique_bus_name);

    pthread_create(&g_client_recv_id, NULL, client_dbus_recv, &g_client_data);
    pthread_create(&g_client_send_id, NULL, client_dbus_sent, &g_client_data);
    pthread_join(g_client_recv_id, NULL);
    pthread_join(g_client_send_id, NULL);
    printf("exit \n");

    return 0;
}

int main(int argc, char *const argv[])
{
    printf("enter dbus client\n");

    test_client_case1();

    return 0;
}