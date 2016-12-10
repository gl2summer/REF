#include <stdio.h>
#include "raw_api.h"
#include "makepack.h"
#include "def.h"
#include "uart.h"


typedef void (* CMDFUNC)(char [][256],int pcount);

typedef struct _cmdnum{
	char *cmd;			//命令代码
	int num;			//序号
	int paramcount;		//参数个数
	CMDFUNC callback;	//回调函数名
}CMDNUM,*PCMDNUM;

#define INVALID_SOCKET (-1)
		
char desthost[256] = "192.168.1.103";	//目的主机地址，默认为本机
int  filemode = TFTP_OCTET;			//发送模式为octet
static struct sockaddr_in dest_addr;

/*定义帮助文档*/
char *helptext = "help:    			显示帮助信息\n\
connect dest_ip:		连接服务器\n\
get filename:			下载文件到0x33400000并运行\n";

//extern void gorun(int);
void showhelp	(char cmd[][256],int pcount);	//显示帮助信息
void connectto	(char cmd[][256],int pcount);	//连接目的主机
void getfile	(char cmd[][256],int pcount);	//下载文件
//void putfile	(char cmd[][256],int pcount);	//上传文件

//提供帮助信息
void showhelp(char cmd[][256],int pcount)
{
	Uart_Printf(helptext);
}

//设置目的主机地址
void connectto(char cmd[][256],int pcount)
{
	if(pcount<1){
		Uart_Printf("格式: connect remoteip \n");
		return;
	}
	strcpy(desthost,cmd[1]);
	memset(&dest_addr, 0, sizeof(dest_addr));
	dest_addr.sin_len=0;
	dest_addr.sin_family=AF_INET;
	dest_addr.sin_port=htons(69);
	dest_addr.sin_addr.s_addr=inet_addr(desthost);

}


static char send_buffer[2048];
static char recv_buffer[2048] ;
//下载文件
unsigned long download_addr;

void getfile(char cmd[][256],int pcount)
{


	
	int sock_1 = INVALID_SOCKET;

	socklen_t sour_len = 0;
	int ret = 0;
	int len = 0 ;
	int flen = 0;
	unsigned short lastdata = 0;
	struct sockaddr_in clientst;
	//unsigned char  *addr = (unsigned char *)0x31000000;
	unsigned char *addr;

#if 1
	Uart_Printf("cmd[0] %s\n", cmd[0]);
	Uart_Printf("cmd[1] %s\n", cmd[1]);
	Uart_Printf("cmd[2] %s\n", cmd[2]);

	Uart_Printf("pcount is %d\n", pcount);
#endif
	
	addr = (unsigned char *)download_addr;
	if(pcount!=1){						//输入的参数个数不合要求
		Uart_Printf("格式: get filename\n");
		return;
	}

	
    sock_1 = socket(AF_INET,SOCK_DGRAM,0);	
	if(sock_1 == INVALID_SOCKET)
	{
		Uart_Printf("创建SOCKET失败\n");
		RAW_ASSERT(0);
	}
	Uart_Printf("create socket success!!  sock_1 = %d\n", sock_1);
	    
    //客户端自身信息

    //bzero(&tftpClient, sizeof (tftpClient));
	memset(&clientst, 0, sizeof(clientst));
    clientst.sin_family = AF_INET;
    clientst.sin_addr.s_addr = INADDR_ANY;
    clientst.sin_port = htons(0);

    //绑定socket到本机IP地址

    if(bind(sock_1, (struct sockaddr *)&clientst, sizeof (clientst)) < 0)
    {
        Uart_Printf("Can not bind socket!\n");
        RAW_ASSERT(0);
    }
	
	//填充请求字段，返回请求包长度 
	len = makereq(TFTP_RRQ,filemode,cmd[1],send_buffer,sizeof(send_buffer));
	//发送请求
	ret = sendto(sock_1,send_buffer,len,0,(struct sockaddr *)&dest_addr,sizeof(dest_addr));

	while(TRUE){                                //接收数据

		sour_len = sizeof(struct sockaddr);
		raw_memset(recv_buffer, 0, sizeof(recv_buffer));
		Uart_Printf("1\n");
		ret = recvfrom(sock_1,recv_buffer,sizeof(recv_buffer),0,(struct sockaddr *)&dest_addr,&sour_len);
		Uart_Printf("ret is %d\n", ret);
	   // 成功则返回接收到的字符数,失败返回-1.
		if(TFTP_ERROR==recv_buffer[1]){
			switch(recv_buffer[3]){// recv_buffer[3]中存放错误信息 
				case Not_defined:
					Uart_Printf("Not defined\n");//未定义 
					return;
				case File_not_found:
					Uart_Printf("File not found\n");//没发现此文件 
					return;
				case Access_violation:
					Uart_Printf("Access violation\n");// 违法进入 
					return;
				case Disk_full:
					Uart_Printf("Disk full\n");//硬盘已满 
					return;
				case Illegal_TFTP_operation:
					Uart_Printf("Illegal TFTP operation\n");//非法操作 
					return;
				case Unknown_port:
					Uart_Printf("Unknown port\n");//未知端口 
					return;
				case File_already_exists:
					Uart_Printf("file already exists\n");//文件已存在 
					return;
				case No_such_user:
					Uart_Printf("No such user\n");//没有此用户 
					return;
				case Time_out:
					Uart_Printf("Time out\n");//超时 
					return;
				case Read_file_Error:
					Uart_Printf("Read file error\n");//读文件错误 
					return;
				case Cannot_create_file:
					Uart_Printf("Can't create file\n");//无法创建文件 
					return;
				default :
					Uart_Printf("unknown error\n");//无法预料的错误 
					return;
			}
		}

		if(TFTP_DATA==recv_buffer[1]){//若是数据文件 
					
			lastdata = MAKEWORD(recv_buffer[3],recv_buffer[2]);
            // makeword是将两个byte型合并成一个word型，一个在高8位，一个在低8位

			//发送ACK
			len = makeack(lastdata,send_buffer,lastdata);
			sendto(sock_1,send_buffer,len,0,(struct sockaddr *)&dest_addr,sizeof(dest_addr));
			
			if(ret < TFTP_NOTEND_DATALEN){
				memcpy(addr,&recv_buffer[4], ret-4);//* copy to 0x33000000
				flen = flen + ret -4;
				Uart_Printf("\n*****传输结束,共收到 %d 字节*****\n",flen);
				raw_sleep(200);
				closesocket(sock_1);
//				gorun(0x34000000);
				return;
			}
			else{
				memcpy(addr,&recv_buffer[4], 512);//* copy to 0x33000000
				addr += 512;
				flen = flen + 512;
				if((flen & 0xffff) == 0x6400)
					Uart_0_SendByte('.');
			}
		}
	}
	
}


