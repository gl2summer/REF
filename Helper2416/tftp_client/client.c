#include <stdlib.h>
#include <stdio.h>

#include "lwip/opt.h"
#include "lwip/mem.h"
#include "lwip/raw.h"
#include "lwip/api.h"
#include "lwip/icmp.h"
#include "lwip/netif.h"
#include "lwip/sys.h"
#include "lwip/timers.h"
#include "lwip/inet_chksum.h"
#include "lwip/sockets.h"
#include "lwip/inet.h"
#include <raw_api.h>

#include "define.h"

#include <mm/raw_page.h>
#include <mm/raw_malloc.h>
#include <raw_work_queue.h>

#include <fifo.h>
#include <lib_string.h>

#include <uart.h>

#define INVALID_SOCKET (-1)
		
static int  filemode = TFTP_OCTET;			//send mode
static struct sockaddr_in dest_addr;		//destination address
static char send_buffer[2048];			//send buffer
static char recv_buffer[2048] ;			//receive buffer
static RAW_U32 download_addr;				//receive file SDRAM address


//ACK包填充
static int makeack(unsigned short num,char *buffer,int size )
{
	int pos = 0;
	buffer[pos] = 0;
	pos++;
	buffer[pos] = TFTP_ACK;    //opcode=04
	pos++;
	buffer[pos] = (char)(num >> 8);//块号2个字节
	pos++;
	buffer[pos] = (char)num;
	pos++;
	return pos;
}
//填充请求包
static int makereq(char type,int mode,char *filename,char *buffer,int size)
{
	int pos = 0;
	unsigned int i = 0;
	char *s = "octet";
//	char *s = "netascii";

	buffer[pos] = 0;
	pos++;
	buffer[pos] = type;// Opcode = 01(RRQ) or 02(WRQ)
	pos++;

	for(i=0;i<strlen(filename);i++){  // &buffer[3] = Filename
		buffer[pos] = filename[i];
		pos++;
	}
	buffer[pos] = 0;   //0
	pos++;
	for(i=0;i<strlen(s);i++){  //mode
		buffer[pos] = s[i];
		pos++;
	}
	buffer[pos] = 0;//0
	pos++;
	return pos;   //返回请求包长度
}


static void set_dest_ip_addr(char *ip_dst)
{
	raw_memset(&dest_addr, 0, sizeof(dest_addr));
	dest_addr.sin_len=0;
	dest_addr.sin_family=AF_INET;
	dest_addr.sin_port=htons(69);
	dest_addr.sin_addr.s_addr=inet_addr(ip_dst);
}

static void tftp_get_file(char cmd[][256],int pcount)
{
	int sock_1 = INVALID_SOCKET;
	int sock_opt;

	socklen_t sour_len = 0;
	int ret = 0;
	int len = 0 ;
	int flen = 0;
	unsigned short lastdata = 0;
	struct sockaddr_in clientst;
	unsigned char *addr;
	
	addr = (unsigned char *)download_addr;
	
	if(pcount!=1){						
		Uart_Printf("example: get filename\n");
		return;
	}
	
  sock_1 = socket(AF_INET,SOCK_DGRAM,0);	
	if(sock_1 == INVALID_SOCKET)
	{
		Uart_Printf("failed to create SOCKET!\n");		//system broken
		RAW_ASSERT(0);
	}
	//Uart_Printf("create socket success!!  sock_1 = %d\n", sock_1);
	    
   raw_memset(&clientst, 0, sizeof(clientst));			/* client configure */
    clientst.sin_family = AF_INET;
    clientst.sin_addr.s_addr = INADDR_ANY;
    clientst.sin_port = htons(0);

    if(bind(sock_1, (struct sockaddr *)&clientst, sizeof (clientst)) < 0)		/* bind ip*/
    {
        Uart_Printf("Can not bind socket!\n");
        RAW_ASSERT(0);
    }


	/* set socket option */												//added by laneyu
	sock_opt = 5000; /* 5 seconds */
	lwip_setsockopt(sock_1, SOL_SOCKET, SO_RCVTIMEO, &sock_opt, sizeof(sock_opt));
	
	//填充请求字段，返回请求包长度 
	len = makereq(TFTP_RRQ,filemode,cmd[1],send_buffer,sizeof(send_buffer));
	//发送请求
	ret = sendto(sock_1,send_buffer,len,0,(struct sockaddr *)&dest_addr,sizeof(dest_addr));

	while(1){                                //接收数据

		sour_len = sizeof(struct sockaddr);
		raw_memset(recv_buffer, 0, sizeof(recv_buffer));
		ret = recvfrom(sock_1,recv_buffer,sizeof(recv_buffer),0,(struct sockaddr *)&dest_addr,&sour_len);

		if (ret<=0) {
			break;
		}
		
	   // 成功则返回接收到的字符数,失败返回-1.
		if(TFTP_ERROR==recv_buffer[1]){
			switch(recv_buffer[3]){// recv_buffer[3]中存放错误信息 
				case Not_defined:
					Uart_Printf("Not defined\n");//未定义 
					break;
				case File_not_found:
					Uart_Printf("File not found\n");//没发现此文件 
					break;
				case Access_violation:
					Uart_Printf("Access violation\n");// 违法进入 
					break;
				case Disk_full:
					Uart_Printf("Disk full\n");//硬盘已满 
					break;
				case Illegal_TFTP_operation:
					Uart_Printf("Illegal TFTP operation\n");//非法操作 
					break;
				case Unknown_port:
					Uart_Printf("Unknown port\n");//未知端口 
					break;
				case File_already_exists:
					Uart_Printf("file already exists\n");//文件已存在 
					break;
				case No_such_user:
					Uart_Printf("No such user\n");//没有此用户 
					break;
				case Time_out:
					Uart_Printf("Time out\n");//超时 
					break;
				case Read_file_Error:
					Uart_Printf("Read file error\n");//读文件错误 
					break;
				case Cannot_create_file:
					Uart_Printf("Can't create file\n");//无法创建文件 
					break;
				default :
					Uart_Printf("unknown error\n");//无法预料的错误 
					break;
			}
		}

		if(TFTP_DATA==recv_buffer[1]){//若是数据文件 
					
			lastdata = MAKEWORD(recv_buffer[3],recv_buffer[2]);
            // makeword是将两个byte型合并成一个word型，一个在高8位，一个在低8位

			//发送ACK
			len = makeack(lastdata,send_buffer,lastdata);
			sendto(sock_1,send_buffer,len,0,(struct sockaddr *)&dest_addr,sizeof(dest_addr));
			
			if(ret < TFTP_NOTEND_DATALEN){

				/*user can copy the received data to other memory*/
				//raw_memcpy(addr,&recv_buffer[4], ret-4);//* copy to 0x33000000
				flen = flen + ret -4;
				Uart_Printf("\n*****传输结束,共收到 %d 字节*****\n",flen);
//				raw_sleep(200);
				closesocket(sock_1);
//				gorun(0x34000000);
				return;
			}
			else{
				
				/*user can copy the received data to other memory*/
				//raw_memcpy(addr,&recv_buffer[4], 512);//* copy to 0x33000000
				addr += 512;
				flen = flen + 512;
				if((flen & 0xfffff) == 0x10000)		/* 1 MByte */
					Uart_Printf(".");
			}
		}
	}

	if (ret<=0) {
		Uart_Printf("tftp_file_get timeout!\n");
	}

	lwip_close(sock_1);
	
}

extern WORK_QUEUE_STRUCT eth_wq;
extern RAW_U32 eth_ISR_cnt;
static void tftp_client_thread(void * arg)
{
	static unsigned long times=0;
	static RAW_MSG_INFO eth_wq_msg;

	Uart_Printf("start to test tftp, please wait...................\n");
	while(1)
	{
		char get_file_cmd[3][256]={"get", "test.avi"};

		//set_dest_mem_addr(0x34100000);		//set get file memory address
		set_dest_ip_addr("192.168.0.100");		//set TFTP server ip address

		raw_sleep(10);			//don't delete 
		tftp_get_file(get_file_cmd, 1);
		raw_queue_get_information(&eth_wq.queue, &eth_wq_msg);
		Uart_Printf("---------------------tftp running times: %d  peak: %d current: %d, eth_isr_cnt:%d----------------------\n", times++, eth_wq_msg.msg_q.peak_numbers, eth_wq_msg.msg_q.current_numbers, eth_ISR_cnt);
		eth_ISR_cnt = 0;
	}

}


void tftp_client_init(void)
{
	sys_thread_new("ping_thread", tftp_client_thread, NULL, DEFAULT_THREAD_STACKSIZE, DEFAULT_THREAD_PRIO);
}

