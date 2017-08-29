/*
 * 写一个应用程序，完成一个文件入库流程业务(只需入库redis和fastDFS即可)。

 业务如下。

 (1) 通过fdfs接口上传某个本地文件,得到fileid 

 (2) 通过上传文件得到的fileid 进行入库redis列表。

 (3) 其中fileid和文件相关属性 如下(相关表设计，自行设计)：

 fileid----  文件名称
             文件创建时间
             文件所属用户(暂时可以使用某默认值）
             文件当前下载量（默认填1即可）

  (4) 将fileid存入到 FILE_INFO_LIST列表中。
 * */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include "redis_op.h"
#include "make_log.h"
#include "fdfs_api.h"

#define LOG_MOUDLE "test"//目录名
#define LOG_PROC "test_redis_op"//文件名

#define FILE_INFO_LIST "file_info_list"
#define FILE_NAME_INFO "file_name_info"
#define FILE_TIME_INFO "file_time_info"
#define FILE_USER_INFO "file_user_info"
#define FILE_PV_INFO "file_pv_info"

#define TIME_STR_MAX 256

int main(int argc, char *argv[])
{
    redisContext *conn = NULL;
    int ret = 0;
    char fileid[FILE_ID_LEN] = {0};

    time_t now = 0;
    char time_str[TIME_STR_MAX] = {0};

    if(argc < 2)
    {
        printf("use:./savefile_2_redis [filename]");
        LOG(LOG_MOUDLE, LOG_PROC, "arg error");
        goto END;
    }
    /*
     *1.链接数据库，得到一个链接句柄
     * */
    conn = rop_connectdb_nopwd("127.0.0.1", "6379");
    if(NULL == conn)
    {
        printf("connecdb error\n");
        LOG(LOG_MOUDLE, LOG_PROC, "connect error");
        goto END;
    }
    //2.上传一个文件得到一个fileid
    ret = fdfs_upload_by_filename1(argv[1], fileid);
    if(ret != 0)
    {
        printf("uploadfile error\n");
        LOG(LOG_MOUDLE, LOG_PROC, "uploadfile error");
        goto END;
        
    }

    //3.建一个链表，将filed存放其中
    ret = rop_list_push(conn, FILE_INFO_LIST,
            fileid);
    if(ret != 0)
    {
        printf("list_push error\n");
        LOG(LOG_MOUDLE, LOG_PROC, "list_push error");
        goto END;
    }

    //4.创建4个hash表存放每个文件的属性
    /*
     *把每个熟悉都单独建立一张表的好处
     --如果把一个表的所有属性建立一张表
     10000个文件就要创建一万张表
     但是把有限4个的属性单独建立一张表
     1000个文件也只要4张表
     创建一个表远比向表中插入一行数据效率低
     * */
    ret = rop_hash_set(conn, FILE_NAME_INFO, fileid, argv[1]);
    if(ret != 0)
    {
        printf("hash_name error\n");
        LOG(LOG_MOUDLE, LOG_PROC, "hash_name error");
        goto END;
    }
    //得到当前时间
    now = time(NULL);//绝对时间
    strftime(time_str, TIME_STR_MAX,
            "%Y-%m-%d %H:%M:%S", localtime(&now));

    ret = rop_hash_set(conn, FILE_TIME_INFO, fileid, time_str);
    if(ret != 0)
    {
        printf("hash_time error\n");
        LOG(LOG_MOUDLE, LOG_PROC, "hash_time error");
        goto END;
    }


    ret = rop_hash_set(conn, FILE_USER_INFO, fileid, "zhang3");
    if(ret != 0)
    {
        printf("hash_user error\n");
        LOG(LOG_MOUDLE, LOG_PROC, "hash_user error");
        goto END;
    }

    ret = rop_hash_set(conn, FILE_PV_INFO, fileid, "1");
    if(ret != 0)
    {
        printf("hash_pv error\n");
        LOG(LOG_MOUDLE, LOG_PROC, "hash_pv error");
        goto END;
    }
END:
    rop_disconnect(conn);
    return ret;
}
