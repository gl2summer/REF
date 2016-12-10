#include <raw_api.h>
#include <yaffsfs.h>

#include <timer.h>
#include <uart.h>
#include <lib_string.h>
#include <rsh.h>
#include <cmd.h>

#include <mm/raw_malloc.h>
#include <mm/raw_page.h>
#include <iic.h>
#include <m25p20.h>
#include <pwm.h>
#include <nand.h>


static unsigned char test_nand_buffer[4096];
static unsigned char nand_data[4096];

static void nand_test(void)
{

	NF_Init();
	rNF_Reset();

	Uart_Printf("1111**************%x\n",rNF_ReadID());

	test_nand_buffer[0] = 0x11;
	test_nand_buffer[1] = 0x22;
	test_nand_buffer[2] = 0x33;
	test_nand_buffer[3] = 0x44;
	test_nand_buffer[4] = 0x55;
	test_nand_buffer[5] = 0x66;
	test_nand_buffer[6] = 0x77;
	test_nand_buffer[7] = 0x88;
	test_nand_buffer[8] = 0x99;
	test_nand_buffer[9] = 0xaa;

	rNF_EraseBlock(0);

	rNF_WritePage(0, test_nand_buffer);
	rNF_ReadPage(0, nand_data);

	Uart_Printf("11* is %x\n", nand_data[0]);
	Uart_Printf("22* is %x\n", nand_data[1]);
	Uart_Printf("33* is %x\n", nand_data[2]);
	Uart_Printf("44* is %x\n", nand_data[3]);
	Uart_Printf("55* is %x\n", nand_data[4]);
	Uart_Printf("66* is %x\n", nand_data[5]);
	Uart_Printf("77* is %x\n", nand_data[6]);
	Uart_Printf("88* is %x\n", nand_data[7]);
	Uart_Printf("99* is %x\n", nand_data[8]);
	Uart_Printf("00* is %x\n", nand_data[9]);
	
	rNF_EraseBlock(0);
	
	random_continue_write(0, 2068, test_nand_buffer, 10);
	random_continue_read(0,2068, nand_data, 10);

	Uart_Printf("11 is %x\n", nand_data[0]);
	Uart_Printf("22 is %x\n", nand_data[1]);
	Uart_Printf("33 is %x\n", nand_data[2]);
	Uart_Printf("44 is %x\n", nand_data[3]);
	Uart_Printf("55 is %x\n", nand_data[4]);
	Uart_Printf("66 is %x\n", nand_data[5]);
	Uart_Printf("77 is %x\n", nand_data[6]);
	Uart_Printf("88 is %x\n", nand_data[7]);
	Uart_Printf("99 is %x\n", nand_data[8]);
	Uart_Printf("00 is %x\n", nand_data[9]);


}


void yaffs_space_erase(void)
{
	RAW_U8 i;

	for (i = 0; i <= 50; i++)
		rNF_EraseBlock(1000 + i);

}


void yaffs_test(void)
{
	int f;
	int r;
	RAW_U8 i;

	NF_Init();
	rNF_Reset();

	//yaffs_space_erase();
	
	yaffs_start_up();


	Uart_Printf("result is %x\n", yaffs_mount("nand"));


	f = yaffs_open("/nand/b1", O_CREAT | O_RDWR,S_IREAD | S_IWRITE);

	Uart_Printf("open /nand/b1 O_CREAT, f=%d\n",f);


	r= yaffs_lseek(f,0,SEEK_SET);


	Uart_Printf("r is %d\n", r);

	Uart_Printf("failure1111 is %d\n", yaffsfs_GetLastError());


	
	r = yaffs_write(f,(void *)0x30800000, 1024);

	
	Uart_Printf("failure2222 is %d\n", yaffsfs_GetLastError());
	
	Uart_Printf("write %d attempted to write to a read-only file\n",r);

	Uart_Printf("freespace is %d\n", yaffs_freespace("nand"));
	Uart_Printf("totalspace is %d\n", yaffs_totalspace("nand"));

	r= yaffs_lseek(f,0,SEEK_SET);

	r= yaffs_read(f, &i, 1);

	Uart_Printf("i is %d\n", i);

	Uart_Printf("ch is %d\n", *((RAW_U8 *)0x30800000));
		
	r = yaffs_close(f);

	Uart_Printf("close %d\n",r);

	yaffs_unmount("nand");



}



RAW_S32 cmd_yaffs_test(RAW_S8 *pcWriteBuffer, RAW_U32 xWriteBufferLen, const RAW_S8 * const pcCommandString)
{


	yaffs_test();

	return 1;
}


RAW_S32 cmd_nand_test(RAW_S8 *pcWriteBuffer, RAW_U32 xWriteBufferLen, const RAW_S8 * const pcCommandString)
{


	nand_test();

	return 1;
}


