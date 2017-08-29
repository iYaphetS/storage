#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include "redis_op.h"
#include "make_log.h"

#define LOG_MOUDLE "test"//目录名
#define LOG_PROC "test_redis_op"//文件名

int main(int argc, char *argv[])
{
    redisContext *conn = NULL;
    int ret = 0;
    //set value
    char value[VALUES_ID_SIZE] = {0};
    //list value arr
    RVALUES list_value_arr = NULL; 
    int list_count = 0;
    int i = 0;

    /*
     *链接数据库，得到一个链接句柄
     * */
    conn = rop_connectdb_nopwd("127.0.0.1", "6379");
    if(NULL == conn)
    {
        printf("connecdb error\n");
        LOG(LOG_MOUDLE, LOG_PROC, "connect error");
        return -1;
    }
#if 1
    /*
     *添加一个集合
     * */
    ret = rop_set_string(conn, "foo", "hello world");
    if(ret != 0)
    {
        printf("set_string error\n");
        LOG(LOG_MOUDLE, LOG_PROC, "set_string error");
        return -2;
    }

    /*
     *得到一个集合
     * */
    ret = rop_get_string(conn, "foo", value);
    if(ret != 0)
    {
        printf("get_string error\n");
        LOG(LOG_MOUDLE, LOG_PROC, "get_string error");
        return -3; 
    }
    printf("value = %s\n", value);

    
    /*
     *链表添加元素
     * */
    ret = rop_list_push(conn, "my_list_key", "zhang3");
    if(ret != 0)
    {
        printf("list_push error\n");
        LOG(LOG_MOUDLE, LOG_PROC, "list_push error");
        return -4; 
    }
    ret = rop_list_push(conn, "my_list_key", "li4");
    if(ret != 0)
    {
        printf("list_push error\n");
        LOG(LOG_MOUDLE, LOG_PROC, "list_push error");
        return -4; 
    }
    ret = rop_list_push(conn, "my_list_key", "wang5");
    if(ret != 0)
    {
        printf("list_push error\n");
        LOG(LOG_MOUDLE, LOG_PROC, "list_push error");
        return -4; 
    }

    /*
     *得到链表元素
     * */
    list_count = 3;
    list_value_arr = malloc(list_count * VALUES_ID_SIZE);//VALUES_ID_SIZE = sizeof(RVALUES)
    ret = rop_range_list(conn, "my_list_key", 0, list_count, list_value_arr, &list_count);
    if(ret != 0)
    {
        printf("range_list error\n");
        LOG(LOG_MOUDLE, LOG_PROC, "range_list error");
        return -4; 
    }

    for(i = 0; i < list_count; ++i)
    {
        printf("value[%d] = %s\n", i, list_value_arr[i]);
    }

    rop_disconnect(conn);
    free(list_value_arr);
    return 0;
#endif
}
