/*
     raw os - Copyright (C)  Lingjun Chen(jorya_txj).

    This file is part of raw os.

    raw os is free software; you can redistribute it it under the terms of the 
    GNU General Public License as published by the Free Software Foundation; 
    either version 3 of the License, or  (at your option) any later version.

    raw os is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
    without even the implied warranty of  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
    See the GNU General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program. if not, write email to jorya.txj@gmail.com
                                      ---

    A special exception to the LGPL can be applied should you wish to distribute
    a combined work that includes raw os, without being obliged to provide
    the source code for any proprietary components. See the file exception.txt
    for full details of how and when the exception can be applied.
*/
/*	
 * 2012/10/08		nand.h 
 *			by Nie Qiang <sddzycnq@gmail.com>
 */

#ifndef __NAND_H__
#define __NAND_H__

//*---------------------------------------- stuctrue and macro-----------------------------------------

// HCLK=100Mhz
#define TACLS				0	// 1-clk(10ns) 
#define TWRPH0				3	// 3-clk(25ns)
#define TWRPH1				0	// 1-clk(10ns)  //TACLS+TWRPH0+TWRPH1>=50ns


#define  CMD_READ1 0x00 //ҳ����������1
#define  CMD_READ2 0x30 //ҳ����������2
#define  CMD_READID 0x90 //��ID����
#define  CMD_WRITE1 0x80 //ҳд��������1
#define  CMD_WRITE2 0x10 //ҳд��������2
#define  CMD_ERASE1 0x60 //�������������1
#define  CMD_ERASE2 0xd0 //�������������2
#define  CMD_STATUS 0x70 //��״̬����
#define  CMD_RESET 0xff //��λ
#define  CMD_RANDOMREAD1 0x05 //�������������1
#define  CMD_RANDOMREAD2 0xE0 //�������������2
#define  CMD_RANDOMWRITE 0x85 //����д����


#define  NF_CMD(data) {rNFCMD =(data);} //��������
#define  NF_ADDR(addr) {rNFADDR=(addr);} //�����ַ
#define  NF_RDDATA() (rNFDATA) //��32λ����
#define  NF_RDDATA8() (rNFDATA8) //��8λ����
#define  NF_WRDATA(data) {rNFDATA=(data);} //д32λ����
#define  NF_WRDATA8(data) {rNFDATA8=(data);} //д8λ����

#define NF_nFCE_L() {rNFCONT&=~(1<<1);}
#define NF_CE_L() NF_nFCE_L()//��nandflashƬѡ
#define NF_nFCE_H() {rNFCONT|= (1<<1);}
#define NF_CE_H() NF_nFCE_H()//�ر�nandflashƬѡ
#define NF_RSTECC() {rNFCONT |= (1<<4); }//��λECC
#define NF_MECC_UnLock() {rNFCONT&=~(1<<5);} //����main��ECC
#define NF_MECC_Lock() {rNFCONT |= (1<<5); }//����main��ECC
#define NF_SECC_UnLock() {rNFCONT&=~(1<<6);} //����spare��ECC
#define NF_SECC_Lock() {rNFCONT |= (1<<6); }//����spare��ECC

#define NF_WAITRB() {while(!(rNFSTAT&(1<<0)));} //�ȴ�nandflash��æ
#define NF_CLEAR_RB() {rNFSTAT|= (1<<2);} //���RnB�ź�
#define NF_DETECT_RB() wait_busy() //�ȴ�RnB�źű�ߣ�����æ

//*-------------------------------------------- function prototype-----------------------------------------

void delay(RAW_U32 num);
void NF_Init(void);
void rNF_Reset(void);
RAW_U8 rNF_ReadID(void);
RAW_U8 rNF_ReadPage(RAW_U32 page_number, RAW_U8 *nand_buffer);

RAW_U8 rNF_WritePage(RAW_U32 page_number,RAW_U8 *pbuf);
RAW_U8 rNF_EraseBlock(RAW_U32 block_number);
RAW_U8 rNF_RandomRead(RAW_U32 page_number,RAW_U32 add);
RAW_U8 rNF_RandomWrite(RAW_U32 page_number, RAW_U32 add, RAW_U8 dat);
RAW_U8 rNF_IsBadBlock(RAW_U32 block);
RAW_U8 rNF_MarkBadBlock(RAW_U32 block);

RAW_U8 random_continue_read(RAW_U32 page_number,RAW_U32 add, RAW_U8 *dat, RAW_U8 count);
RAW_U8 random_continue_write(RAW_U32 page_number, RAW_U32 add, RAW_U8 *dat, RAW_U8 count);





#endif

//*-------------------------------------------- end -----------------------------------------


