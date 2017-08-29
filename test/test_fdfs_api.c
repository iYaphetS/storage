#include <stdio.h>
#include "fdfs_api.h"
#include "make_log.h"


int main(int argc, char *argv[])
{
    if(argc < 2)
    {
        printf("please input ./main filename\n");
    }

    int ret = 0;
    char fileid[FILE_ID_LEN] = {0};

    //ret = fdfs_upload_by_filename(argv[1], fileid);
    ret = fdfs_upload_by_filename1(argv[1], fileid);
    if(ret != 0)
    {
        printf("upload file error\n");
        LOG("main", "call upload_file", "upload file error\n");
    }
    else
    {
        printf("%s\n", fileid);
        LOG("main", "call upload_file", "%s\n", fileid);
    }

    return 0;

}
