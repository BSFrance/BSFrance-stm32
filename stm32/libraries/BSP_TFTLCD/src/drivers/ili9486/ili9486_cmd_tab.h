/**
*ILI9486L command list TAB
*Author: SVCHAO
*Created: 2015/4/22
*
*
* BCTRL:Brightness Control.  0: brightness control block OFF. 
                             1: brightness control block ON.
							 
* BL:   Backlight Control.   0: backlight control OFF.
                             1: backlight control ON. 
							 
* DD:   Display Dimming Control.  0: display dimming OFF
                                  1: display dimming ON]
								  
* CABC:  Content Adaptive Brightness Control.	
* EPL :  DE polarity.
* DPL :  PCLK polarity.
* HSPL:  HSYNC polarity.
* VSPL:  VSYNC polarity.
* SDA_EN: 3/4 write serial interfac selection.
* MADCTL: Memory Access Control.							  
								  
*/
//Regular Command Set
#define ILI_CMD_NOP                  (0x00) //NOP
#define ILI_CMD_SOFT_RES             (0x01) //soft reset
#define ILI_CMD_READ_IDENT_INFO      (0X04) //read display identification information
#define ILI_CMD_READ_NERR_ON_DSI     (0x05) //Read Number of the Errors on DSI
#define ILI_CMD_READ_DISP_STATUS     (0x09) //Read Display Status 
#define ILI_CMD_READ_DISP_PWRMODE    (0x0A) //Read Display Power Mode
#define ILI_CMD_READ_PIXEL_FORMAT    (0x0C) //Read Pixel Format
#define ILI_CMD_READ_DISP_IMG_MODE   (0x0D) //Read display image mode
#define ILI_CMD_READ_DISP_SIG_MODE   (0x0E) //read display signal mode
#define ILI_CMD_READ_DISP_SELF_RESULT (0x0F) //read display self-diagnostic result
#define ILI_CMD_SLEEP_IN             (0x10) //sleep in(enter the minimum power consumption mode.)
#define ILI_CMD_SLEEP_OUT            (0x11) //sleep out
#define ILI_CMD_PARTIAL_MODE         (0x12) //partial mode ON
#define ILI_CMD_NORMAL_MODE          (0x13) //normal display mode ON
#define ILI_CMD_DISP_INV_OFF         (0x20) //display inversion OFF
#define ILI_CMD_DISP_INV_ON          (0x21) //display inversion ON
#define ILI_CMD_DISP_OFF             (0x28) //display OFF
#define ILI_CMD_DISP_ON              (0x29) //display ON
#define ILI_CMD_COL_ADDR_SET         (0x2A) //column address set 
#define ILI_CMD_PAGE_ADDR_SET        (0x2B) //page address set 
#define ILI_CMD_W_TO_MEM             (0x2C) //memory write
#define ILI_CMD_W_FROM_MEM           (0x2E) //memory read
#define ILI_CMD_PART_AREA            (0x30) //partial Area
#define ILI_CMD_VSCROLL              (0x33) //vertical scrolling definition ´¹Ö±¹ö¶¯
#define ILI_CMD_TEL_OFF              (0x34) //tearing effect line OFF
#define ILI_CMD_TEL_ON               (0x35) //tearing effect line ON
#define ILI_CMD_MEM_ACCESS_CTRL      (0x36) //memory access control
#define ILI_CMD_VSCROLL_START_ADDR   (0x37) //vertical scrolling start address
#define ILI_CMD_IDLE_MODE_OFF        (0x38) //idle mode OFF
#define ILI_CMD_IDLE_MODE_ON         (0x39) //idle mode ON
#define ILI_CMD_IF_PIXEL_FORMAT      (0x3A) //interfac pixel format
#define ILI_CMD_MEM_WRITE_CONTINUE   (0x3C) //memory  wrtie continue 
#define ILI_CMD_MEM_READ_CONTINUE    (0x3E) //memory read continue
#define ILI_CMD_WRITE_TEAR_SCAN_LINE (0x44) //write tear  scan line
#define ILI_CMD_READ_TEAR_SCAN_LINE  (0x45) //read tear scan line 
#define ILI_CMD_WRITE_BRIGHTNESS_VAL (0x51) //write display brightness value 
#define ILI_CMD_READ_BRIGHTNESS_VAL  (0x52) //read display brightness value 
#define ILI_CMD_WRITE_CTRL_VAL       (0x53) //control BCTRL,DD,BL.
#define ILI_CMD_READ_CTRL_VAL        (0x54) //read  BCTRL,DD,BL.
#define ILI_CMD_WRITE_ADAPTIVE_VAL   (0x55) //write content adaptive brightness control value.
#define ILI_CMD_READ_ADAPTIVAL_VAL   (0x56) //read content adaptive brightness control value.
#define ILI_CMD_READ_ID1             (0xDA) //read ID1
#define ILI_CMD_READ_ID2             (0xDB) //read ID2
#define ILI_CMD_READ_ID3             (0xDC) //read ID3
//Extended Command Set
#define ILI_CMD_IF_MODE_CTRL          (0xB0) //interfac mode control 
#define ILI_CMD_FRAME_RATE_IN_NORMAL  (0xB1) //frame rate control(in normal model/full color).
#define ILI_CMD_FRAME_RATE_IN_IDEL    (0xB2) //...(in idle mode/8 color).
#define ILI_CMD_FRAME_RATE_IN_PARTIAL (0xB3) //...(in partial mode/full color).
#define ILI_CMD_DISP_INV_CTRL         (0xB4) //display inversion control.
#define ILI_CMD_BLANKING_PORCH_CTRL   (0xB5) //blanking porch control.
#define ILI_CMD_DISP_FUNC_CTRL        (0xB6) //display function control.
#define ILI_CMD_SET_ENTRY_MODE        (0xB7) //entry mode set.
#define ILI_CMD_PWR_CTRL1             (0xC0) //power control 1.
#define ILI_CMD_PWR_CTRL2             (0xC1) //....2.
#define ILI_CMD_PWR_CTRL3             (0xC2) //...3.
#define ILI_CMD_PWR_CTRL4             (0xC3) //...4.
#define ILI_CMD_PWR_CTRL5             (0xC4) //...5.
#define ILI_CMD_VCOM_CTRL             (0xC5) //VCOM control.
#define ILI_CMD_CABC_CTRL1            (0xC6) //CABC control.
#define ILI_CMD_CABC_CTRL2            (0xC8) //..
#define ILI_CMD_CABC_CTRL3            (0xC9) //..
#define ILI_CMD_CABC_CTRL4            (0xCA) //..
#define ILI_CMD_CABC_CTRL5            (0xCB) //..
#define ILI_CMD_CABC_CTRL6            (0xCC) //..
#define ILI_CMD_CABC_CTRL7            (0xCD) //..
#define ILI_CMD_CABC_CTRL8            (0xCE) //..
#define ILI_CMD_CABC_CTRL9            (0xCF) //..
#define ILI_CMD_WRITE_NV_MEM          (0xD0) //NV Memory Write.
#define ILI_CMD_WRITE_NV_MEM_KEY      (0xD1) //NV Memory Protection Key.
#define ILI_CMD_READ_NV_MEM_STATUS    (0xD2) //NV Memory Status Read.
#define ILI_CMD_READ_ID4              (0xD3) //Read ID4.
#define ILI_CMD_PGAM_CTRL             (0xE0) //PGRAM control (positive gamma contrl).
#define ILI_CMD_NGAM_CTRL             (0xE1) //NGAM control (negative gamma control).
#define ILI_CMD_DIGITAL_GAMMA_CTRL1   (0xE2) //digital gamma control.
#define ILI_CMD_DIGITAL_GAMMA_CTRL2   (0xE3) //..
#define ILI_CMD_SPI_READCMD_SET       (0xFB) //SPI read command setting.
//end























