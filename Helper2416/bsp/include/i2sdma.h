#ifndef I2SDMA_H
#define I2SDMA_H
	#include <stdint.h>
	
	void wm8731_write(unsigned int reg, unsigned int value);
	extern unsigned char IIC_init(unsigned int freq);
	extern unsigned char IIC_open(void);
	extern unsigned char  IIC_close( void);

	extern void *raw_memset(void *src, uint8_t byte, uint32_t count);
	extern void *raw_memcpy(void *dest, const void *src, uint32_t count);
	extern uint32_t get_PCLK(void);
	extern void Uart_Printf(char *fmt,...);


	typedef struct{
			uint8_t ckID[4];	//fmt
			uint32_t cksize;	//16 18 or 40
			uint16_t wFormatTag;
			uint16_t nChannels;
			uint32_t nSamplesPerSec;
			uint32_t nAvgBytesPerSec;
			uint16_t nBlockAlign;
			uint16_t wBitsPerSample;
		
			uint16_t cbSize;	//0 or 22
		
			uint16_t wValidBitsPerSample;
			uint32_t dwChannelMask;
			uint8_t SubFormat[16];
	}__attribute__ ((packed)) FMTCHUNK ;
	typedef struct{
			uint8_t ckID[4];		//fact
			uint32_t cksize;
			uint32_t dwSampleLength;
	}__attribute__ ((packed)) FACTCHUNK;
	typedef struct{
			uint8_t ckID[4];		//data
			uint32_t cksize;
			uint8_t *sampled;
	}__attribute__ ((packed)) DATACHUNK;

	void i2s_start(void);
	uint8_t i2s_getflag(void);
	void i2s_filldata(uint32_t *buf,uint32_t count);
	void i2s_pause(void);
	
	void i2s_power_init(void);
	void i2s_dma_init(uint32_t fs,uint8_t bitlen,uint8_t channel,uint32_t datalen);
	void wm8731_init(uint8_t bitlen,uint8_t ch);
	void wm8731_dac_open(void);
	void wm8731_dac_close(void);
	
	uint8_t wave_checkid(uint8_t *ckID,char *str);;
	uint32_t wave_format(uint8_t *addr,FMTCHUNK *fmt, FACTCHUNK *fact,DATACHUNK *data,uint8_t *ret)  ;

#endif

