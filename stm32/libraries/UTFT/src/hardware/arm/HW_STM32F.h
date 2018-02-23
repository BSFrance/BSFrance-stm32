// *** Hardwarespecific functions ***

#define FSMC_TFT
#ifdef __GNUC__
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wunused-parameter"
#endif

typedef struct
{
	volatile uint16_t  REG;
	volatile uint16_t  RAM;
} LCD_IO_TypeDef;


#if defined(STM32GENERIC)

#if __has_include("bsp.h")
#  include "bsp.h"
#endif

#define   TFT_LCD             ((LCD_IO_TypeDef *) TFT_LCD_BASE)

void UTFT::_hw_special_init(void)
{
  STM_FSMC_LCD_Init();
}

#elif defined(ARDUINO_ARCH_MAPLE_STM32F1)||defined(ARDUINO_ARCH_MAPLE_STM32F4)
// FSMC_NOR_PSRAM_REGION4                           ((void*)0x6C000000)
#define LCD_REG              (*((volatile unsigned short *) 0x6C000000))  /* RS = 0 */
#define LCD_RAM              (*((volatile unsigned short *) 0x6C000002))  /* RS = 1 */
void UTFT::_hw_special_init()
{
//    pinMode(PA1, OUTPUT); digitalWrite(PA1, HIGH); // Set the backlighe HIGH move to class init
    fsmc_nor_psram_reg_map *regs;
//    fsmc_sram_init_gpios();   //已在board.cpp 初始化norsram nand时初始化
//    rcc_clk_enable(RCC_FSMC); //已在board.cpp 打开时钟

	regs = FSMC_NOR_PSRAM4_BASE;  //nand blank4 PG12/FSMC_NE4
    regs->BCR = (FSMC_BCR_WREN  | FSMC_BCR_MWID_16BITS | FSMC_BCR_MTYP_SRAM |
                 FSMC_BCR_MBKEN);
    fsmc_nor_psram_set_addset(regs, 0);
    fsmc_nor_psram_set_datast(regs, 3);	
}

#else
	#error "arch err"
#endif

//VH==0 REG
void UTFT::LCD_Writ_Bus(int VH,int VL, byte mode) 
{   
     if (VH == 0) TFT_LCD->REG = VL;
     else TFT_LCD->RAM = VL; 
}

void UTFT::_set_direction_registers(byte mode)
{

}

void UTFT::_fast_fill_16(int ch, int cl, long pix)
{ 
    volatile uint32_t i;
	for(i=0;i<pix;i++)
	{
		LCD_Write_DATA(ch,cl);		  
	}
}

void UTFT::_fast_fill_8(int ch, long pix)
{
	volatile uint32_t  i; 
	for(i=0;i<pix;i++)
	{
		LCD_Write_DATA(ch);		  
	}

}

/*******************************************************************************
* Function Name  : LCD_WriteReg
* Description    : controller command
* Input          : - cmd: writes command.
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
__inline void UTFT::LCD_Write_COM(int com1)
{
	/* Write cmd */
	TFT_LCD->REG = com1;
}

/*******************************************************************************
* Function Name  : LCD_WriteRAM
* Description    : Writes to the LCD RAM.
* Input          : - data: the pixel color in RGB mode (5-6-5).
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
__inline void UTFT::LCD_Write_DATA(int VL)					 
{
	/* Write 8-bit data */
	TFT_LCD->RAM = VL;
}

__inline void UTFT::LCD_Write_DATA(int VH,int VL)
{
	/* Write 16-bit data */
	TFT_LCD->RAM = (uint16_t)((VH<<8)+VL);
}

__inline void UTFT::LCD_Write_COM_DATA(int com1,int dat1)
{
     TFT_LCD->REG = com1;
	 TFT_LCD->RAM = dat1;
}
/*******************************************************************************
* Function Name  : LCD_ReadRAM
* Description    : Reads the LCD RAM.
* Input          : None
* Output         : None
* Return         : LCD RAM Value.
* Attention		 : None
*******************************************************************************/
__inline uint16_t LCD_ReadData(void)
{  
	/* Read 16-bit data */
	return TFT_LCD->RAM;
}

#if defined ( __GNUC__ )
#pragma GCC diagnostic pop
#endif
