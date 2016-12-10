/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

#include "gfx.h"

#if GFX_USE_GDISP

#if defined(GDISP_SCREEN_HEIGHT)
	#warning "GDISP: This low level driver does not support setting a screen size. It is being ignored."
	#undef GISP_SCREEN_HEIGHT
#endif
#if defined(GDISP_SCREEN_WIDTH)
	#warning "GDISP: This low level driver does not support setting a screen size. It is being ignored."
	#undef GDISP_SCREEN_WIDTH
#endif

#define GDISP_DRIVER_VMT			GDISPVMT_ILI9341
#include "drivers/gdisp/ILI9341/gdisp_lld_config.h"
#include "src/gdisp/gdisp_driver.h"

#include "board_ILI9341.h"

/*===========================================================================*/
/* Driver local definitions.                                                 */
/*===========================================================================*/

#ifndef GDISP_SCREEN_HEIGHT
	#define GDISP_SCREEN_HEIGHT		320
#endif
#ifndef GDISP_SCREEN_WIDTH
	#define GDISP_SCREEN_WIDTH		240
#endif
#ifndef GDISP_INITIAL_CONTRAST
	#define GDISP_INITIAL_CONTRAST	50
#endif
#ifndef GDISP_INITIAL_BACKLIGHT
	#define GDISP_INITIAL_BACKLIGHT	100
#endif

#include "drivers/gdisp/ILI9341/ILI9341.h"

#define ILI9340_NOP     0x00
#define ILI9340_SWRESET 0x01
#define ILI9340_RDDID   0x04
#define ILI9340_RDDST   0x09

#define ILI9340_SLPIN   0x10
#define ILI9340_SLPOUT  0x11
#define ILI9340_PTLON   0x12
#define ILI9340_NORON   0x13

#define ILI9340_RDMODE  0x0A
#define ILI9340_RDMADCTL  0x0B
#define ILI9340_RDPIXFMT  0x0C
#define ILI9340_RDIMGFMT  0x0A
#define ILI9340_RDSELFDIAG  0x0F

#define ILI9340_INVOFF  0x20
#define ILI9340_INVON   0x21
#define ILI9340_GAMMASET 0x26
#define ILI9340_DISPOFF 0x28
#define ILI9340_DISPON  0x29

#define ILI9340_CASET   0x2A
#define ILI9340_PASET   0x2B
#define ILI9340_RAMWR   0x2C
#define ILI9340_RAMRD   0x2E

#define ILI9340_PTLAR   0x30
#define ILI9340_MADCTL  0x36


#define ILI9340_MADCTL_MY  0x80
#define ILI9340_MADCTL_MX  0x40
#define ILI9340_MADCTL_MV  0x20
#define ILI9340_MADCTL_ML  0x10
#define ILI9340_MADCTL_RGB 0x00
#define ILI9340_MADCTL_BGR 0x08
#define ILI9340_MADCTL_MH  0x04

#define ILI9340_PIXFMT  0x3A

#define ILI9340_FRMCTR1 0xB1
#define ILI9340_FRMCTR2 0xB2
#define ILI9340_FRMCTR3 0xB3
#define ILI9340_INVCTR  0xB4
#define ILI9340_DFUNCTR 0xB6

#define ILI9340_PWCTR1  0xC0
#define ILI9340_PWCTR2  0xC1
#define ILI9340_PWCTR3  0xC2
#define ILI9340_PWCTR4  0xC3
#define ILI9340_PWCTR5  0xC4
#define ILI9340_VMCTR1  0xC5
#define ILI9340_VMCTR2  0xC7

#define ILI9340_RDID1   0xDA
#define ILI9340_RDID2   0xDB
#define ILI9340_RDID3   0xDC
#define ILI9340_RDID4   0xDD

#define ILI9340_GMCTRP1 0xE0
#define ILI9340_GMCTRN1 0xE1
/*
#define ILI9340_PWCTR6  0xFC

*/

// Color definitions
#define ILI9340_BLACK   0x0000
#define ILI9340_BLUE    0x001F
#define ILI9340_RED     0xF800
#define ILI9340_GREEN   0x07E0
#define ILI9340_CYAN    0x07FF
#define ILI9340_MAGENTA 0xF81F
#define ILI9340_YELLOW  0xFFE0  
#define ILI9340_WHITE   0xFFFF

/*===========================================================================*/
/* Driver local functions.                                                   */
/*===========================================================================*/

// Some common routines and macros
#define dummy_read(g)				{ volatile uint16_t dummy; dummy = read_data(g); (void) dummy; }
#define write_reg(g, reg, data)		{ write_index(g, reg); write_data(g, data); }
#define write_data16(g, data)		{ write_data(g, data >> 8); write_data(g, (uint8_t)data); }
#define delay(us)					gfxSleepMicroseconds(us)
#define delayms(ms)					gfxSleepMilliseconds(ms)

static void set_viewport(GDisplay *g) {
	write_index(g, 0x2A);
	write_data(g, (g->p.x >> 8));
	write_data(g, (uint8_t) g->p.x);
	write_data(g, (g->p.x + g->p.cx - 1) >> 8);
	write_data(g, (uint8_t) (g->p.x + g->p.cx - 1));

	write_index(g, 0x2B);
	write_data(g, (g->p.y >> 8));
	write_data(g, (uint8_t) g->p.y);
	write_data(g, (g->p.y + g->p.cy - 1) >> 8);
	write_data(g, (uint8_t) (g->p.y + g->p.cy - 1));
}

/*===========================================================================*/
/* Driver exported functions.                                                */
/*===========================================================================*/

LLDSPEC bool_t gdisp_lld_init(GDisplay *g) {
	// No private area for this controller
	g->priv = 0;

	// Initialise the board interface
	init_board(g);

	// Hardware reset
	setpin_reset(g, TRUE);
	gfxSleepMilliseconds(20);
	setpin_reset(g, FALSE);
	gfxSleepMilliseconds(20);

	// Get the bus for the following initialisation commands
	acquire_bus(g);
/*
    write_index(g, 0xEF);
    write_data(g, 0x03);
    write_data(g, 0x80);
    write_data(g, 0x02);

    write_index(g, 0xCF);  
    write_data(g, 0x00); 
    write_data(g, 0xC1); 
    write_data(g, 0x30); 

    write_index(g, 0xED);  
    write_data(g, 0x64); 
    write_data(g, 0x03); 
    write_data(g, 0x12); 
    write_data(g, 0x81); 
 
    write_index(g, 0xE8);  
    write_data(g, 0x85); 
    write_data(g, 0x00); 
    write_data(g, 0x78); 

    write_index(g, 0xCB);  
    write_data(g, 0x39); 
    write_data(g, 0x2C); 
    write_data(g, 0x00); 
    write_data(g, 0x34); 
    write_data(g, 0x02); 
 
    write_index(g, 0xF7);  
    write_data(g, 0x20); 

    write_index(g, 0xEA);  
    write_data(g, 0x00); 
    write_data(g, 0x00); 
 
    write_index(g, ILI9340_PWCTR1);    //Power control 
    write_data(g, 0x23);   //VRH[5:0] 
 
    write_index(g, ILI9340_PWCTR2);    //Power control 
    write_data(g, 0x10);   //SAP[2:0];BT[3:0] 
 
    write_index(g, ILI9340_VMCTR1);    //VCM control 
    write_data(g, 0x3e); //???????
    write_data(g, 0x28); 
  
    write_index(g, ILI9340_VMCTR2);    //VCM control2 
    write_data(g, 0x86);  //--
 
    write_index(g, ILI9340_MADCTL);    // Memory Access Control 
    write_data(g, ILI9340_MADCTL_MX | ILI9340_MADCTL_BGR);

    write_index(g, ILI9340_PIXFMT);    
    write_data(g, 0x55); 
  
    write_index(g, ILI9340_FRMCTR1);    
    write_data(g, 0x00);  
    write_data(g, 0x18); 
 
    write_index(g, ILI9340_DFUNCTR);    // Display Function Control 
    write_data(g, 0x08); 
    write_data(g, 0x82);
    write_data(g, 0x27);  
 
    write_index(g, 0xF2);    // 3Gamma Function Disable 
    write_data(g, 0x00); 
 
    write_index(g, ILI9340_GAMMASET);    //Gamma curve selected 
    write_data(g, 0x01); 
 
    write_index(g, ILI9340_GMCTRP1);    //Set Gamma 
    write_data(g, 0x0F); 
    write_data(g, 0x31); 
    write_data(g, 0x2B); 
    write_data(g, 0x0C); 
    write_data(g, 0x0E); 
    write_data(g, 0x08); 
    write_data(g, 0x4E); 
    write_data(g, 0xF1); 
    write_data(g, 0x37); 
    write_data(g, 0x07); 
    write_data(g, 0x10); 
    write_data(g, 0x03); 
    write_data(g, 0x0E); 
    write_data(g, 0x09); 
    write_data(g, 0x00); 
  
    write_index(g, ILI9340_GMCTRN1);    //Set Gamma 
    write_data(g, 0x00); 
    write_data(g, 0x0E); 
    write_data(g, 0x14); 
    write_data(g, 0x03); 
    write_data(g, 0x11); 
    write_data(g, 0x07); 
    write_data(g, 0x31); 
    write_data(g, 0xC1); 
    write_data(g, 0x48); 
    write_data(g, 0x08); 
    write_data(g, 0x0F); 
    write_data(g, 0x0C); 
    write_data(g, 0x31); 
    write_data(g, 0x36); 
    write_data(g, 0x0F); 

    write_index(g, ILI9340_SLPOUT);    //Exit Sleep 
    gfxSleepMilliseconds(120);       
    write_index(g, ILI9340_DISPON);    //Display on 
*/

	write_index(g, 0x01); //software reset
	gfxSleepMilliseconds(5);
	write_index(g, 0x28);
	// display off
	//---------------------------------------------------------
	// magic?
	write_index(g, 0xcf);
	write_data(g, 0x00);
	write_data(g, 0x83);
	write_data(g, 0x30);

	write_index(g, 0xed);
	write_data(g, 0x64);
	write_data(g, 0x03);
	write_data(g, 0x12);
	write_data(g, 0x81);
	write_index(g, 0xe8);
	write_data(g, 0x85);
	write_data(g, 0x01);
	write_data(g, 0x79);
	write_index(g, 0xcb);
	write_data(g, 0x39);
	write_data(g, 0x2c);
	write_data(g, 0x00);
	write_data(g, 0x34);
	write_data(g, 0x02);
	write_index(g, 0xf7);
	write_data(g, 0x20);
	write_index(g, 0xea);
	write_data(g, 0x00);
	write_data(g, 0x00);
	//------------power control------------------------------
	write_index(g, 0xc0); //power control
	write_data(g, 0x26);
	write_index(g, 0xc1); //power control
	write_data(g, 0x11);
	//--------------VCOM
	write_index(g, 0xc5); //vcom control
	write_data(g, 0x35);//35
	write_data(g, 0x3e);//3E
	write_index(g, 0xc7); //vcom control
	write_data(g, 0xbe); // 0x94
	//------------memory access control------------------------
	write_index(g, 0x36);
	// memory access control
	write_data(g, 0x48); //0048 my,mx,mv,ml,BGR,mh,0.0
	write_index(g, 0x3a); // pixel format set
	write_data(g, 0x55);//16bit /pixel
	//----------------- frame rate------------------------------
	write_index(g, 0xb1);
	// frame rate
	write_data(g, 0x00);
	write_data(g, 0x1B); //70
	//----------------Gamma---------------------------------
	write_index(g, 0xf2); // 3Gamma Function Disable
	write_data(g, 0x08);
	write_index(g, 0x26);
	write_data(g, 0x01); // gamma set 4 gamma curve 01/02/04/08

	write_index(g, 0xE0); //positive gamma correction
	write_data(g, 0x1f);
	write_data(g, 0x1a);
	write_data(g, 0x18);
	write_data(g, 0x0a);
	write_data(g, 0x0f);
	write_data(g, 0x06);
	write_data(g, 0x45);
	write_data(g, 0x87);
	write_data(g, 0x32);
	write_data(g, 0x0a);
	write_data(g, 0x07);
	write_data(g, 0x02);
	write_data(g, 0x07);
	write_data(g, 0x05);
	write_data(g, 0x00);
	write_index(g, 0xE1); //negamma correction
	write_data(g, 0x00);
	write_data(g, 0x25);
	write_data(g, 0x27);
	write_data(g, 0x05);
	write_data(g, 0x10);
	write_data(g, 0x09);
	write_data(g, 0x3a);
	write_data(g, 0x78);
	write_data(g, 0x4d);
	write_data(g, 0x05);
	write_data(g, 0x18);
	write_data(g, 0x0d);
	write_data(g, 0x38);
	write_data(g, 0x3a);
	write_data(g, 0x1f);
	//--------------ddram ---------------------
	write_index(g, 0x2a);
	// column set
	// size = 239
	write_data(g, 0x00);
	write_data(g, 0x00);
	write_data(g, 0x00);
	write_data(g, 0xEF);
	write_index(g, 0x2b);
	// page address set
	// size = 319
	write_data(g, 0x00);
	write_data(g, 0x00);
	write_data(g, 0x01);
	write_data(g, 0x3F);
	// write_index(g, 0x34);
	//write_index(g, 0x35);
	// tearing effect off
	// tearing effect on
	// write_index(g, 0xb4); // display inversion
	// write_data(g, 0x00);
	write_index(g, 0xb7); //entry mode set
	write_data(g, 0x07);
	//-----------------display---------------------
	write_index(g, 0xb6);
	// display function control
	write_data(g, 0x0a);
	write_data(g, 0x82);
	write_data(g, 0x27);
	write_data(g, 0x00);
	write_index(g, 0x11); //sleep out
	gfxSleepMilliseconds(100);
	write_index(g, 0x29); // display on
	gfxSleepMilliseconds(100);

  // Finish Init
  post_init_board(g);

 	// Release the bus
	release_bus(g);
	
	/* Turn on the back-light */
	set_backlight(g, GDISP_INITIAL_BACKLIGHT);

	/* Initialise the GDISP structure */
	g->g.Width = GDISP_SCREEN_WIDTH;
	g->g.Height = GDISP_SCREEN_HEIGHT;
	g->g.Orientation = GDISP_ROTATE_0;
	g->g.Powermode = powerOn;
	g->g.Backlight = GDISP_INITIAL_BACKLIGHT;
	g->g.Contrast = GDISP_INITIAL_CONTRAST;
	return TRUE;
}

#if GDISP_HARDWARE_STREAM_WRITE
	LLDSPEC	void gdisp_lld_write_start(GDisplay *g) {
		acquire_bus(g);
		set_viewport(g);
		write_index(g, 0x2C);
	}
	LLDSPEC	void gdisp_lld_write_color(GDisplay *g) {
		write_data16(g, gdispColor2Native(g->p.color));
	}
	LLDSPEC	void gdisp_lld_write_stop(GDisplay *g) {
		release_bus(g);
	}
#endif

#if GDISP_HARDWARE_STREAM_READ
	LLDSPEC	void gdisp_lld_read_start(GDisplay *g) {
		acquire_bus(g);
		set_viewport(g);
		write_index(g, 0x2E);
		setreadmode(g);
		dummy_read(g);
	}
	LLDSPEC	color_t gdisp_lld_read_color(GDisplay *g) {
		uint16_t	data;

		data = read_data(g);
		return gdispNative2Color(data);
	}
	LLDSPEC	void gdisp_lld_read_stop(GDisplay *g) {
		setwritemode(g);
		release_bus(g);
	}
#endif

#if GDISP_NEED_CONTROL && GDISP_HARDWARE_CONTROL
	LLDSPEC void gdisp_lld_control(GDisplay *g) {
		switch(g->p.x) {
		case GDISP_CONTROL_POWER:
			if (g->g.Powermode == (powermode_t)g->p.ptr)
				return;
			switch((powermode_t)g->p.ptr) {
			case powerOff:
			case powerSleep:
			case powerDeepSleep:
				acquire_bus(g);
				write_reg(g, 0x0010, 0x0001);	/* enter sleep mode */
				release_bus(g);
				break;
			case powerOn:
				acquire_bus(g);
				write_reg(g, 0x0010, 0x0000);	/* leave sleep mode */
				release_bus(g);
				break;
			default:
				return;
			}
			g->g.Powermode = (powermode_t)g->p.ptr;
			return;

		case GDISP_CONTROL_ORIENTATION:
			if (g->g.Orientation == (orientation_t)g->p.ptr)
				return;
			switch((orientation_t)g->p.ptr) {
			case GDISP_ROTATE_0:
				acquire_bus(g);
				write_reg(g, 0x36, 0x48);	/* X and Y axes non-inverted */
				release_bus(g);
				g->g.Height = GDISP_SCREEN_HEIGHT;
				g->g.Width = GDISP_SCREEN_WIDTH;
				break;
			case GDISP_ROTATE_90:
				acquire_bus(g);
				write_reg(g, 0x36, 0xE8);	/* Invert X and Y axes */
				release_bus(g);
				g->g.Height = GDISP_SCREEN_WIDTH;
				g->g.Width = GDISP_SCREEN_HEIGHT;
				break;
			case GDISP_ROTATE_180:
				acquire_bus(g);
				write_reg(g, 0x36, 0x88);		/* X and Y axes non-inverted */
				release_bus(g);
				g->g.Height = GDISP_SCREEN_HEIGHT;
				g->g.Width = GDISP_SCREEN_WIDTH;
				break;
			case GDISP_ROTATE_270:
				acquire_bus(g);
				write_reg(g, 0x36, 0x28);	/* Invert X and Y axes */
				release_bus(g);
				g->g.Height = GDISP_SCREEN_WIDTH;
				g->g.Width = GDISP_SCREEN_HEIGHT;
				break;
			default:
				return;
			}
			g->g.Orientation = (orientation_t)g->p.ptr;
			return;

        case GDISP_CONTROL_BACKLIGHT:
            if ((unsigned)g->p.ptr > 100)
            	g->p.ptr = (void *)100;
            set_backlight(g, (unsigned)g->p.ptr);
            g->g.Backlight = (unsigned)g->p.ptr;
            return;

		//case GDISP_CONTROL_CONTRAST:
        default:
            return;
		}
	}
#endif

#endif /* GFX_USE_GDISP */
