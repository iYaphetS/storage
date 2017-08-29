#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <string.h>


#include "make_log.h"
#include "util.h"
#include "select_file.h"

#include "fcgi_stdio.h"
#include "fcgi_config.h"
extern char** environ;

#define FCGI "test"
#define DATA "data_cgi"
#define VALUE_LEN 128

int main ()
{
    char cmd_value[VALUE_LEN] = {0};
    char fromId_value[VALUE_LEN] = {0};
    char count_value[VALUE_LEN] = {0};

    char *query_string = NULL;
    int ret = 0;
    while (FCGI_Accept() >= 0) {
        printf("Content-type: text/html\r\n"
                "\r\n");
        query_string = getenv("QUERY_STRING");
        LOG(FCGI, DATA, "%s", query_string);
        ret = query_parse_key_value(query_string, "cmd", cmd_value, NULL);
        if(ret != 0)
        {
            LOG(FCGI, DATA, "query_parse_key_value error");
        }

        char *json_str = malloc(4096);
        if(strcmp(cmd_value, "newFile") == 0)
        {
            query_parse_key_value(query_string, "fromId", fromId_value, NULL);
            query_parse_key_value(query_string, "count", count_value, NULL);
            select_file(atoi(fromId_value), atoi(count_value), json_str);
            LOG(FCGI, DATA, "json_str:%s", json_str);
            printf("%s", json_str);
        }
        
        if(json_str != NULL)
        {
            free(json_str);
        }

#if 0
        //测试静态的json数据返回给前端
        FILE *fp = fopen("json_test_data.json", "r");

        memset(json_str, 0, 4096);

        fread(json_str, 4096, 1, fp);
        fclose(fp);
        printf("%s", json_str);
#endif

    } /* while */

    return 0;
}
