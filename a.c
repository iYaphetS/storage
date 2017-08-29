#include <stdio.h>
#include <string.h>

#include "framework.h"//包含CSocketProtocol  CEncDesProtocol
#include "CSckImp1.h"//包含socket_init, send_data, recv_data, socket_destory声明
#include "CHwImp.h"//包含enc_file, des_file声明


int main()
{
	CSocketProtocol sp;
	setSP(&sp, socket_init, send_data, recv_data, socket_destory);
	CEncDesProtocol ed;
	setED(&ed, enc_file, des_file);

	//报文为简单的字符串
	char send_buf[] = "hello";
	int send_len = strlen(send_buf);

	char recv_buf[1024] = {0};
	int recv_len = 0;

	int ret = framework(&sp, &ed, send_buf, send_len, recv_buf, &recv_len);
	if (ret != 0)
	{
		printf("framework err:%d\n", ret);
		return -1;
	}

	printf("接受到的报文信息：%s, 长度：%d\n", recv_buf, recv_len);

	return 0;
}