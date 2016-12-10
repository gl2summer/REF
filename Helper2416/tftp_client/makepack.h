#include "define.h"

int  makeack(unsigned short num,char *buffer,int size );
int  makereq(char type,int mode,char *filename,char *buffer,int size);
int  makedata(unsigned short num,char *data,int datasize,char *buffer,int bufsize);

//ACK�����
int makeack(unsigned short num,char *buffer,int size )
{
	int pos = 0;
	buffer[pos] = 0;
	pos++;
	buffer[pos] = TFTP_ACK;    //opcode=04
	pos++;
	buffer[pos] = (char)(num >> 8);//���2���ֽ�
	pos++;
	buffer[pos] = (char)num;
	pos++;
	return pos;
}
//��������
int makereq(char type,int mode,char *filename,char *buffer,int size)
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
	return pos;   //�������������
}

//Data�����
int makedata(unsigned short num,char *data,int datasize,char *buffer,int bufsize)
{
	int pos = 0;
	buffer[pos] = 0;
	pos++;
	buffer[pos] = TFTP_DATA;//opcode=3
	pos++;
	buffer[pos] = (char)(num>>8);//���
	pos++;
	buffer[pos] = (char)num;
	pos++;
	memcpy(&buffer[pos],data,datasize); //data
	pos = pos + datasize;
	return pos;
}
