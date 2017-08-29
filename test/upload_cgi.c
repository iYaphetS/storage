#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <string.h>


#include "upload_file.h"
#include "make_log.h"
#include "util.h"

#include "fcgi_stdio.h"
#include "fcgi_config.h"
extern char** environ;

#define FCGI "test"
#define UPLOAD_FILE "upload_cgi"


int main ()
{
    int ret = 0;
    while (FCGI_Accept() >= 0) {
        //客户端post数据的长度
        char *contentLength = getenv("CONTENT_LENGTH");
        int len;

        printf("Content-type: text/html\r\n"
                "\r\n");

        //判断是否有post数据,len就是post数据的长度
        if (contentLength != NULL) {
            len = strtol(contentLength, NULL, 10);
        }
        else {
            len = 0;
        }

        if (len <= 0) {
            printf("No data from standard input.<p>\n");
        }
        else {
            //len > 0
            int i, ch;
            int fd;
            char *pStart,*pEnd,*p = NULL;
            
            char boundary[256] = {0};
            char filename[256] = {0};

            char *buf = malloc(len);
            memset(buf, 0, len);
            p = buf;//p--->buf

            //将前部客户端的post数据写到p指向的buf中
            for (i = 0; i < len; i++) {
                //从前端读一个字节
                if ((ch = getchar()) < 0) {
                    printf("Error: Not enough bytes received on standard input<p>\n");
                    break;
                }
                //putchar(ch);
                *p = ch;
                p++;
            }
            FILE *fp = fopen("upload_data", "w");
            fwrite(buf, 1, len, fp);
            fclose(fp);

            //p已经指向了buf的尾部，需要将p拉回
            p = buf;
            //1.解析第一行，得到格式中边界字符串
            pStart = p;
            pEnd = strstr(pStart, "\r\n");
            if(pEnd == NULL)
            {
                LOG(FCGI, UPLOAD_FILE, "strstr error");
                goto END;
            }
            strncpy(boundary, pStart, pEnd - pStart);
            printf("bondary = %s\n", boundary);

            //将pStart,pEnd辅助指针移动到下一行
            pEnd += 2;//第一行\r\n
            pStart = pEnd;

            //2.解析第2行，取出文件名
            pEnd = strstr(pStart, "filename=");
            pEnd += strlen("filename=");
            pEnd += 1;//filename=后面的"
            pStart = pEnd;
            pEnd = strchr(pStart, '"');

            strncpy(filename, pStart, pEnd - pStart);
            printf("filename = %s\n", filename);
            
            //辅助指针移动到第3行
            pEnd += 2;//第2行\r\n
            pStart = pEnd;

            //3.辅助指针跳过无效信息第3行，和第4空行
            pEnd = strstr(pStart, "\r\n");
            pEnd += 2;//第3行尾部\r\n

            pEnd += 2;//第4行\r\n
            pStart = pEnd;

            //剩余的文件长度
            len -= pEnd - p;

            //4.从第5行开始解析，获得post正文有效数据内容
            pEnd = memstr(pStart, len, boundary);
            if( pEnd == NULL)
            {
                LOG(FCGI, UPLOAD_FILE, "memstr error");
                goto END;
            }

            pEnd -= 2;//去掉post正文尾部\r\n

            fd = open(filename, O_CREAT|O_WRONLY|O_TRUNC, 0664);
            if(fd == -1)
            {
                LOG(FCGI, UPLOAD_FILE, "open error");
                goto END;
            }
            printf("%d", pEnd - pStart);
            ftruncate(fd, pEnd - pStart);
            write(fd, pStart, pEnd - pStart);
            //5.上传文件到fdfs和redis数据库
            ret = upload_file(filename);
            if(ret != 0)
            {
                LOG(FCGI, UPLOAD_FILE, "upload_file error");
                goto END;
            }


END:
            memset(boundary, 0, 256);
            memset(filename, 0, 256);
            close(fd);
            if(buf != NULL)
                free(buf);    
            buf = NULL;
            //删除临时文件
            unlink(filename);
        }

    } /* while */

    return 0;
}
