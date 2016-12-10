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


#define  CMD_READ1 0x00 //页读命令周期1
#define  CMD_READ2 0x30 //页读命令周期2
#define  CMD_READID 0x90 //读ID命令
#define  CMD_WRITE1 0x80 //页写命令周期1
#define  CMD_WRITE2 0x10 //页写命令周期2
#define  CMD_ERASE1 0x60 //块擦除命令周期1
#define  CMD_ERASE2 0xd0 //块擦除命令周期2
#define  CMD_STATUS 0x70 //读状态命令
#define  CMD_RESET 0xff //复位
#define  CMD_RANDOMREAD1 0x05 //随意读命令周期1
#define  CMD_RANDOMREAD2 0xE0 //随意读命令周期2
#define  CMD_RANDOMWRITE 0x85 //随意写命令


#define  NF_CMD(data) {rNFCMD =(data);} //传输命令
#define  NF_ADDR(addr) {rNFADDR=(addr);} //传输地址
#define  NF_RDDATA() (rNFDATA) //读32位数据
#define  NF_RDDATA8() (rNFDATA8) //读8位数据
#define  NF_WRDATA(data) {rNFDATA=(data);} //写32位数据
#define  NF_WRDATA8(data) {rNFDATA8=(data);} //写8位数据

#define NF_nFCE_L() {rNFCONT&=~(1<<1);}
#define NF_CE_L() NF_nFCE_L()//打开nandflash片选
#define NF_nFCE_H() {rNFCONT|= (1<<1);}
#define NF_CE_H() NF_nFCE_H()//关闭nandflash片选
#define NF_RSTECC() {rNFCONT |= (1<<4); }//复位ECC
#define NF_MECC_UnLock() {rNFCONT&=~(1<<5);} //解锁main区ECC
#define NF_MECC_Lock() {rNFCONT |= (1<<5); }//锁定main区ECC
#define NF_SECC_UnLock() {rNFCONT&=~(1<<6);} //解锁spare区ECC
#define NF_SECC_Lock() {rNFCONT |= (1<<6); }//锁定spare区ECC

#define NF_WAITRB() {while(!(rNFSTAT&(1<<0)));} //等待nandflash不忙
#define NF_CLEAR_RB() {rNFSTAT|= (1<<2);} //清除RnB信号
#define NF_DETECT_RB() wait_busy() //等待RnB信号变高，即不忙

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


