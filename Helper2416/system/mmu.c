#include <raw_api.h>


#define SECTION_ENTRY(base,ap,d,c,b) \
	((base << 20)|(ap<<10)|(d<<5)|(1<<4)|(c<<3)|(b<<2)|(1<<1))


#define SECTION_ENTRY_CACHE(base,tex, ap,d,c,b) \
	((base << 20)|(tex << 12)|(ap<<10)|(d<<5)|(1<<4)|(c<<3)|(b<<2)|(1<<1))



#define MMU_IO_AREA_START	(0x00000000)

#define MMU_RESERVED1_START	(0x60000000)
#define MMU_MEM_AREA_START	(0xc0000000)
#define LCD_DMA_BUFFER_PA_BASE  (0x33800000)
#define LCD_DMA_BUFFER_PA_END   (0x33b00000)

#define MMU_RESERVED2_START	(0xc4000000)
#define MMU_EXCEPTION_START	(0xfff00000)

#define MMU_PAGE_TABLE_START   (0x30100000)
#define MMU_PHYSICAL_EXCEPTION_START	(0x300)


#define MMU_MEM_AREA_START_3	(0x30000000)

#define MMU_MEM_AREA_START_3_4	(0x34000000)



void make_mmu_table (void)
{
	int i;
	//unsigned int *addr = (unsigned int *)0x37ff4000;
	unsigned int *addr = (unsigned int *)MMU_PAGE_TABLE_START;

	
	/* 1:1 mapping  none cached*/
	for (i=(MMU_IO_AREA_START>>20); i<(MMU_MEM_AREA_START_3>>20); i++) {
		
		addr[i] = SECTION_ENTRY(i,1,1,0,0);
	}


	/* 1:1 mapping write back cached memory from start memory to lcd_dma address*/
	
	for (i=(MMU_MEM_AREA_START_3>>20); i<(LCD_DMA_BUFFER_PA_BASE >> 20); i++) {
			
	
		addr[i] = SECTION_ENTRY_CACHE(i,0,1,1,1,1);	
	}


	/*lcd cache can be write through cache*/
	for (i=(LCD_DMA_BUFFER_PA_BASE>>20); i<(LCD_DMA_BUFFER_PA_END >> 20); i++) {


		addr[i] = SECTION_ENTRY_CACHE(i,0,1,1,1,0);

	}


	/*the end of the memory is none cached for other device dma purpose*/
	for (i=(LCD_DMA_BUFFER_PA_END>>20); i<(MMU_MEM_AREA_START_3_4 >> 20); i++) {


		addr[i] = SECTION_ENTRY_CACHE(i,0,1,1,0,0);

	}


		/* 1:1 mapping */
	for (i=(MMU_MEM_AREA_START_3_4>>20); i<(MMU_RESERVED1_START>>20); i++) {
		
		addr[i] = SECTION_ENTRY(i,1,1,0,0);
	}

	

	/* disabled */
	for (i=(MMU_RESERVED1_START>>20); i<(MMU_MEM_AREA_START>>20); i++) {
		addr[i] = 0x00000000;
	}

	/* mapping system memory to 0xc0000000 */
	for (i=(MMU_MEM_AREA_START>>20); i<(MMU_RESERVED2_START>>20); i++) {
		addr[i] = SECTION_ENTRY((i-(0xc00-0x300)),1,1,0,0);
		
	}


	for (i=(MMU_RESERVED2_START>>20); i<0x1000; i++) {

		if (i == (MMU_EXCEPTION_START >> 20)) {

			addr[i] = SECTION_ENTRY((i-(0xfff-MMU_PHYSICAL_EXCEPTION_START)),1,1,0,0);
			break;

		}


		else {
			/* disabled */

			addr[i] = 0x00000000;
		}
		
	}

	

	
}

