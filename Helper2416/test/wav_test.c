#include <raw_api.h>
#include <yaffsfs.h>

#include <timer.h>
#include <uart.h>
#include <lib_string.h>
#include <rsh.h>
#include <cmd.h>

#include <mm/raw_malloc.h>
#include <mm/raw_page.h>
#include <nand.h>
#include <i2sdma.h>

extern unsigned char successwave[183340];
extern unsigned char r1wave[665018];
static uint32_t TmpBuffer[4096];

static FMTCHUNK fmtchunk;
static FACTCHUNK factchunk;
static DATACHUNK datachunk;
static void dplay_music(uint8_t *addr)  
{
		uint8_t ret;
		uint32_t i,j,dat;
		uint32_t maxcount;
		uint8_t *pbuf;
		uint8_t flag;
		wave_format(addr,&fmtchunk,&factchunk,&datachunk,&ret);
		if(ret == 1)
				Uart_Printf("Invalid Wave File Format\r\n");
		else if(ret == 2)
				Uart_Printf("Invalid PCM Format\r\n");
		else if(ret == 3)
				Uart_Printf("Invalid data\r\n");
		if(ret)
				return;
		
		i2s_dma_init(fmtchunk.nSamplesPerSec,fmtchunk.wBitsPerSample,fmtchunk.nChannels,0);
		wm8731_init(fmtchunk.wBitsPerSample,fmtchunk.nChannels);
		i2s_start();
		pbuf = datachunk.sampled;
		maxcount = datachunk.cksize;
		
		Uart_Printf("Size:%d\r\n",fmtchunk.cksize);
		Uart_Printf("FormatTag:%d\r\n",fmtchunk.wFormatTag);
		Uart_Printf("Channels:%d\r\n",fmtchunk.nChannels);
		Uart_Printf("SamplesPerSec:%d\r\n",fmtchunk.nSamplesPerSec);
		Uart_Printf("AvgBytesPerSec:%d\r\n",fmtchunk.nAvgBytesPerSec);
		Uart_Printf("BlockAlign:%d\r\n",fmtchunk.nBlockAlign);
		Uart_Printf("BitsPerSample:%d\r\n",fmtchunk.wBitsPerSample);
		Uart_Printf("VA:%04x\r\n",datachunk.sampled - addr);
		wm8731_dac_open();
		
		i=0;
		flag = 1;
		while(flag)
		{
				if(i2s_getflag() == 0)
				{
						for(j=0;j<4096;j++)
						{
								if(i>=maxcount)
								{
										flag = 0;
										break;
								}

										if(fmtchunk.wBitsPerSample == 8)
										{												
												if(fmtchunk.nChannels == 0)
														dat = (pbuf[i]<<7);
												else if(fmtchunk.nChannels == 1)
														dat = (pbuf[i]<<(16+7));
												else if(fmtchunk.nChannels == 2)
													dat = (pbuf[i]<<7) | (pbuf[i+1]<<(16+7));
										}
										else if(fmtchunk.wBitsPerSample == 16)
										{
												#if 1
												if(fmtchunk.nChannels == 0)
													dat = pbuf[i] | (pbuf[i+1]<<8);
												else if(fmtchunk.nChannels == 1)
													dat = ((pbuf[i]<<16) | (pbuf[i+1]<<24));
												else if(fmtchunk.nChannels == 2)
													dat = (pbuf[i+0]<<0) | (pbuf[i+1]<<8) | (pbuf[i+2]<<16) | (pbuf[i+3]<<24);
													//dat = (pbuf[i+0]<<0) | (pbuf[i+1]<<8);
												#endif
										}
										else if(fmtchunk.wBitsPerSample == 24)
										{
												
										}
										TmpBuffer[j] = dat;
										i+=fmtchunk.nBlockAlign;
						}
						if(j)
							i2s_filldata(TmpBuffer,j);
				}
		}
		wm8731_dac_close();
		i2s_pause();
}

RAW_S32 cmd_wave_test(RAW_S8 *pcWriteBuffer, RAW_U32 xWriteBufferLen, const RAW_S8 * const pcCommandString)
{
		RAW_S32 comleng;
    RAW_S8 *pcom=(RAW_S8 *)rsh_get_parameter( pcCommandString,1,&comleng ); 
		int time_cont = 0;
		if(*pcom == 'i')
    {	
				Uart_Printf("wav player init\n");
				i2s_power_init();
				Uart_Printf("wav player init end\n");
		}
		else if(*pcom == 'a')
		{
				Uart_Printf("play\r\n");
				//dplay_music(successwave);
				dplay_music(r1wave);
				Uart_Printf("play end\r\n");
		}
		else
		{
				Uart_Printf("help for wave player\r\n");
				Uart_Printf("wave i or wave a\r\n");
		}
		return 1;
}

