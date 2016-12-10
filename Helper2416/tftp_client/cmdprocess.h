int  stripcmd(char *s,char cmd[][256]);	/*分开输入的命令语句的命令及参数*/
void parsecmd(char *s);					/*解析命令,回调相应函数*/
int  getcmdsubscript(char *s);			/*获取命令数组的下标*/

/*定义一个存储回调函数的命令代码数组，其值与不同的命令相对应*/
CMDNUM cmdlist[] = {
	{"help"   ,1,0,showhelp},
	{"connect",2,1,connectto},
	{"get"    ,3,1,getfile}};

//分开输入的命令字符串得到命令代码及参数
int stripcmd(char *s,char cmd[][256])
{
	int i=0;
	char *token=NULL;
	char seps[] = " ";		
	token = strtok( s, seps );		/*断开命令与参数*/
	while(token!=NULL){
		if (i>2) break;	/*因为回调函数的参数最多为3个，当识别出的参数多于2个时，忽略多于的*/ 
		strcpy(cmd[i],token);
		token = strtok( NULL, seps );/*从s开头开始的一个个被分割的串。当没有被分割的串时则返回NULL。
　　                          所有seps中包含的字符都会被滤掉，并将被滤掉的地方设为一处分割的节点*/
		i++;
	}
	return i; 
}

#if 0
//* 获取命令行
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

//解析命令,回调相应命令处理函数
void parsecmd(char *s)
{
	char cmd[3][256];
	int pcount = 0;
	int subscript = -1;
	pcount = stripcmd(s,cmd);		/*分开命令与参数，返回值为参数的个数加1*/
	subscript=getcmdsubscript(cmd[0]);	/*获取下标*/
	if(subscript==-1){
		Uart_Printf("No such commond \n");
		return;
	}
	else{
		cmdlist[subscript].callback(cmd,pcount-1);/*根据命令回调相应的函数*/
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


//获取命令数组的下标
int  getcmdsubscript(char *s)
{
	int i = 0;
	for(i=0;i<sizeof(cmdlist)/sizeof(CMDNUM);i++){/*这里sizeof(cmdlist)/sizeof(CMDNUM)==5*/
		if(stricmp(s,cmdlist[i].cmd)==0){
			return i;
		}
	}
	return -1;  
}
