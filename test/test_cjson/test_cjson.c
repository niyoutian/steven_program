#include <stdio.h>
#include <string.h>
#include <getopt.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "cJSON.h"


int parse_config(cJSON *json_root) 
{
    cJSON *json_client = cJSON_GetObjectItem(json_root, "client");

    if (json_client) {
        cJSON *json_client_ip = cJSON_GetObjectItem(json_client, "ip");
        cJSON *json_client_port = cJSON_GetObjectItem(json_client, "port");
        printf("type=%d\n",json_client_port->type);
        printf("param_ip:%s\n", json_client_ip->valuestring);
        printf("param_port:%d\n", json_client_port->valueint);
    }

    cJSON *json_ecu_array = cJSON_GetObjectItem(json_root, "ecu");
    if (json_ecu_array) {
        int array_size = cJSON_GetArraySize(json_ecu_array);
        printf("array_size:%d\n",array_size);
        for (int i = 0; i < array_size; i++) {
            cJSON *json_ecu_array_item = cJSON_GetArrayItem(json_ecu_array, i);
            cJSON *json_ecu_channel_type = cJSON_GetObjectItem(json_ecu_array_item, "channel_type");
            cJSON *json_ecu_ip = cJSON_GetObjectItem(json_ecu_array_item, "ip");
            cJSON *json_ecu_project_dir = cJSON_GetObjectItem(json_ecu_array_item, "project_dir");
            cJSON *json_ecu_test_dir = cJSON_GetObjectItem(json_ecu_array_item, "test_dir");
            printf("i=%d channel_type=%s ip=%s project_dir=%s test_dir=%s\n", i, json_ecu_channel_type->valuestring, 
                    json_ecu_ip->valuestring, json_ecu_project_dir->valuestring, json_ecu_test_dir->valuestring);
        }
    }

    return 0;
}


int main(int argc, char* argv[])
{
    FILE *f = NULL;
    size_t len;
    char *data = NULL;
    cJSON *json_root = NULL;

    printf("test cjson\n");
    printf("argv[1]=%s\n",argv[1]);



    f = fopen(argv[1], "rb");  
    if (f == NULL) {
        printf("read file %s failed\n",argv[1]);
        return -1;
    }

    fseek(f, 0, SEEK_END);
    len = ftell(f);
    fseek(f, 0, SEEK_SET);
    data = (char *)malloc(len + 1);
    if (data == NULL)
    {
        printf("malloc data failed!");
        fclose(f);
        return -1;
    }
    fread(data, 1, len, f);
    fclose(f);

    json_root = cJSON_Parse(data);
    free(data);

    if (!json_root) {
        printf("Error before: [%s]\n",cJSON_GetErrorPtr());
        return -1;
    }

    char *out = cJSON_Print(json_root);
    printf("%s\n",out);
    free(out);

    parse_config(json_root);

    cJSON_Delete(json_root);

    return 0;
}