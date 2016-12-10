#include <stdio.h>
#include "raw_api.h"
#include "makepack.h"
#include "def.h"
#include "uart.h"


typedef void (* CMDFUNC)(char [][256],int pcount);

typedef struct _cmdnum{
	char *cmd;			//�������
	int num;			//���
	int paramcount;		//��������
	CMDFUNC callback;	//�ص�������
}CMDNUM,*PCMDNUM;

#define INVALID_SOCKET (-1)
		
char desthost[256] = "192.168.1.103";	//Ŀ��������ַ��Ĭ��Ϊ����
int  filemode = TFTP_OCTET;			//����ģʽΪoctet
static struct sockaddr_in dest_addr;

/*��������ĵ�*/
char *helptext = "help:    			��ʾ������Ϣ\n\
connect dest_ip:		���ӷ�����\n\
get filename:			�����ļ���0x33400000������\n";

//extern void gorun(int);
void showhelp	(char cmd[][256],int pcount);	//��ʾ������Ϣ
void connectto	(char cmd[][256],int pcount);	//����Ŀ������
void getfile	(char cmd[][256],int pcount);	//�����ļ�
//void putfile	(char cmd[][256],int pcount);	//�ϴ��ļ�

//�ṩ������Ϣ
void showhelp(char cmd[][256],int pcount)
{
	Uart_Printf(helptext);
}

//����Ŀ��������ַ
void connectto(char cmd[][256],int pcount)
{
	if(pcount<1){
		Uart_Printf("��ʽ: connect remoteip \n");
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
//�����ļ�
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
	if(pcount!=1){						//����Ĳ�����������Ҫ��
		Uart_Printf("��ʽ: get filename\n");
		return;
	}

	
    sock_1 = socket(AF_INET,SOCK_DGRAM,0);	
	if(sock_1 == INVALID_SOCKET)
	{
		Uart_Printf("����SOCKETʧ��\n");
		RAW_ASSERT(0);
	}
	Uart_Printf("create socket success!!  sock_1 = %d\n", sock_1);
	    
    //�ͻ���������Ϣ

    //bzero(&tftpClient, sizeof (tftpClient));
	memset(&clientst, 0, sizeof(clientst));
    clientst.sin_family = AF_INET;
    clientst.sin_addr.s_addr = INADDR_ANY;
    clientst.sin_port = htons(0);

    //��socket������IP��ַ

    if(bind(sock_1, (struct sockaddr *)&clientst, sizeof (clientst)) < 0)
    {
        Uart_Printf("Can not bind socket!\n");
        RAW_ASSERT(0);
    }
	
	//��������ֶΣ�������������� 
	len = makereq(TFTP_RRQ,filemode,cmd[1],send_buffer,sizeof(send_buffer));
	//��������
	ret = sendto(sock_1,send_buffer,len,0,(struct sockaddr *)&dest_addr,sizeof(dest_addr));

	while(TRUE){                                //��������

		sour_len = sizeof(struct sockaddr);
		raw_memset(recv_buffer, 0, sizeof(recv_buffer));
		Uart_Printf("1\n");
		ret = recvfrom(sock_1,recv_buffer,sizeof(recv_buffer),0,(struct sockaddr *)&dest_addr,&sour_len);
		Uart_Printf("ret is %d\n", ret);
	   // �ɹ��򷵻ؽ��յ����ַ���,ʧ�ܷ���-1.
		if(TFTP_ERROR==recv_buffer[1]){
			switch(recv_buffer[3]){// recv_buffer[3]�д�Ŵ�����Ϣ 
				case Not_defined:
					Uart_Printf("Not defined\n");//δ���� 
					return;
				case File_not_found:
					Uart_Printf("File not found\n");//û���ִ��ļ� 
					return;
				case Access_violation:
					Uart_Printf("Access violation\n");// Υ������ 
					return;
				case Disk_full:
					Uart_Printf("Disk full\n");//Ӳ������ 
					return;
				case Illegal_TFTP_operation:
					Uart_Printf("Illegal TFTP operation\n");//�Ƿ����� 
					return;
				case Unknown_port:
					Uart_Printf("Unknown port\n");//δ֪�˿� 
					return;
				case File_already_exists:
					Uart_Printf("file already exists\n");//�ļ��Ѵ��� 
					return;
				case No_such_user:
					Uart_Printf("No such user\n");//û�д��û� 
					return;
				case Time_out:
					Uart_Printf("Time out\n");//��ʱ 
					return;
				case Read_file_Error:
					Uart_Printf("Read file error\n");//���ļ����� 
					return;
				case Cannot_create_file:
					Uart_Printf("Can't create file\n");//�޷������ļ� 
					return;
				default :
					Uart_Printf("unknown error\n");//�޷�Ԥ�ϵĴ��� 
					return;
			}
		}

		if(TFTP_DATA==recv_buffer[1]){//���������ļ� 
					
			lastdata = MAKEWORD(recv_buffer[3],recv_buffer[2]);
            // makeword�ǽ�����byte�ͺϲ���һ��word�ͣ�һ���ڸ�8λ��һ���ڵ�8λ

			//����ACK
			len = makeack(lastdata,send_buffer,lastdata);
			sendto(sock_1,send_buffer,len,0,(struct sockaddr *)&dest_addr,sizeof(dest_addr));
			
			if(ret < TFTP_NOTEND_DATALEN){
				memcpy(addr,&recv_buffer[4], ret-4);//* copy to 0x33000000
				flen = flen + ret -4;
				Uart_Printf("\n*****�������,���յ� %d �ֽ�*****\n",flen);
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


