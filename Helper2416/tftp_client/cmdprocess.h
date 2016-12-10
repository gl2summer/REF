int  stripcmd(char *s,char cmd[][256]);	/*�ֿ���������������������*/
void parsecmd(char *s);					/*��������,�ص���Ӧ����*/
int  getcmdsubscript(char *s);			/*��ȡ����������±�*/

/*����һ���洢�ص�����������������飬��ֵ�벻ͬ���������Ӧ*/
CMDNUM cmdlist[] = {
	{"help"   ,1,0,showhelp},
	{"connect",2,1,connectto},
	{"get"    ,3,1,getfile}};

//�ֿ�����������ַ����õ�������뼰����
int stripcmd(char *s,char cmd[][256])
{
	int i=0;
	char *token=NULL;
	char seps[] = " ";		
	token = strtok( s, seps );		/*�Ͽ����������*/
	while(token!=NULL){
		if (i>2) break;	/*��Ϊ�ص������Ĳ������Ϊ3������ʶ����Ĳ�������2��ʱ�����Զ��ڵ�*/ 
		strcpy(cmd[i],token);
		token = strtok( NULL, seps );/*��s��ͷ��ʼ��һ�������ָ�Ĵ�����û�б��ָ�Ĵ�ʱ�򷵻�NULL��
����                          ����seps�а������ַ����ᱻ�˵����������˵��ĵط���Ϊһ���ָ�Ľڵ�*/
		i++;
	}
	return i; 
}

#if 0
//* ��ȡ������
int GetParameter(char *str, int cnt)
{
	int i; 
	char key;	
	RAW_U16 err;
	
	i = 0;
	while(1)
	{
		err = raw_semaphore_get(&uart_0_recv_sem, RAW_WAIT_FOREVER);
		if (err == RAW_SUCCESS)
			fifo_out(&uart_0_recv_fifo, &key, 1);
		//key = Uart_Getch();
		if(key)
		{
			if(key == ENTER_KEY)
			{
				str[i] = 0;
				Uart_SendByte('\n');
//				Uart_Printf("%s\n",str);
				
				return i;
			}
			else if(key == BACK_KEY && i>0)
			{
				i--;
				Uart_Printf("\b \b");
			}
			else if(key == 0x1b)
			{
				//Uart_Getch();
				//Uart_Getch();
				return -1;
			}
			else if(key>=0x20 && key<=0x7e && i<cnt)
			{
				str[i++] = key;
				Uart_SendByte(key);
			}				
		}

	}	
}
#endif

//��������,�ص���Ӧ�������
void parsecmd(char *s)
{
	char cmd[3][256];
	int pcount = 0;
	int subscript = -1;
	pcount = stripcmd(s,cmd);		/*�ֿ����������������ֵΪ�����ĸ�����1*/
	subscript=getcmdsubscript(cmd[0]);	/*��ȡ�±�*/
	if(subscript==-1){
		Uart_Printf("No such commond \n");
		return;
	}
	else{
		cmdlist[subscript].callback(cmd,pcount-1);/*��������ص���Ӧ�ĺ���*/
	}
}

int stricmp(const char * dst, const char * src)
{
    int f,l;

	do {
	    if ( ((f = (unsigned char)(*(dst++))) >= 'A') && (f <= 'Z') )
	        f -= ('A' - 'a');

	    if ( ((l = (unsigned char)(*(src++))) >= 'A') && (l <= 'Z') )
	        l -= ('A' - 'a');
	} while ( f && (f == l) );


        return(f - l);
}


//��ȡ����������±�
int  getcmdsubscript(char *s)
{
	int i = 0;
	for(i=0;i<sizeof(cmdlist)/sizeof(CMDNUM);i++){/*����sizeof(cmdlist)/sizeof(CMDNUM)==5*/
		if(stricmp(s,cmdlist[i].cmd)==0){
			return i;
		}
	}
	return -1;  
}
