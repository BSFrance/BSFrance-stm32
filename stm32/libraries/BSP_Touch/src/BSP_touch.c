#include <Arduino.h>
#include "BSP_touch.h"
//#include "../LCD/BSP_tft_lcd.h"
#include "math.h"
//触摸屏驱动（支持ADS7843/7846/UH7843/7846/XPT2046/TSC2046/OTT2001A等） 代码	   
 
//电阻屏芯片连接引脚	   
#define PEN  		digitalRead(tp_dev.irq)  	    //T_PEN
#define DOUT 		digitalRead(tp_dev.miso)   	    //T_MISO
#define TDIN0 		digitalWrite(tp_dev.mosi,LOW)  	//T_MOSI
#define TDIN1 		digitalWrite(tp_dev.mosi,HIGH)  	//T_MOSI
#define TCLK0 		digitalWrite(tp_dev.sck,LOW)  	//T_SCK
#define TCLK1 		digitalWrite(tp_dev.sck,HIGH)  	//T_SCK
#define TCS0  		digitalWrite(tp_dev.cs,LOW)  	//T_CS
#define TCS1  		digitalWrite(tp_dev.cs,HIGH)  	//T_CS

#define delay_ms(x)  delay(x)
#define delay_us(x)  delayMicroseconds(x)

_m_tp_dev tp_dev=
{
	.init = TP_Init,
	.scan = TP_Scan,
	.adjust = TP_Adjust,
	.xfac = TOUCH_XFAC/1000.0,   /*xfac*/
	.yfac = TOUCH_YFAC/1000.0,   /*yfac*/
	.xoff = TOUCH_XOFF,   /*xoff*/
	.yoff = TOUCH_YOFF,   /*yoff*/
	.touchtype = 1,   /*touchtype*/
	
#ifndef TOUCH_XYCHG
	.CMD_RDX = 0XD0,
	.CMD_RDY = 0X90,
#elif (TOUCH_XYCHG==0)
	.CMD_RDX = 0XD0,
	.CMD_RDY = 0X90,
#else	
	.CMD_RDX = 0X90,
	.CMD_RDY = 0XD0,
#endif


#ifndef TOUCH_MISO
    .miso = TOUCH_MISO,
#endif
#ifndef TOUCH_MOSI
	.mosi = TOUCH_MOSI,
#endif
#ifndef TOUCH_SCK
	.sck = TOUCH_SCK,
#endif
#ifndef TOUCH_NCS
	.cs  = TOUCH_NCS,
#endif
#ifndef TOUCH_NIRQ
	.irq = TOUCH_NIRQ,
#endif
};

//SPI写数据
//向触摸屏IC写入1byte数据    
//num:要写入的数据
void TP_Write_Byte(uint8_t num)    
{  
	uint8_t count=0;   
	for(count=0;count<8;count++)  
	{ 	  
		if(num&0x80)digitalWrite(tp_dev.mosi,HIGH);  
		else digitalWrite(tp_dev.mosi,LOW);   
		num<<=1;    
		digitalWrite(tp_dev.sck,LOW); 	 
		digitalWrite(tp_dev.sck,HIGH);		//上升沿有效	        
	}		 			    
}

//SPI读数据 
//从触摸屏IC读取adc值
//CMD:指令
//返回值:读到的数据	   
uint16_t TP_Read_AD(uint8_t CMD)	  
{ 	 
	uint8_t count=0; 	  
	uint16_t Num=0; 
	digitalWrite(tp_dev.sck,LOW);		//先拉低时钟 	 
	TDIN0; 	//拉低数据线
	digitalWrite(tp_dev.cs,LOW); 		//选中触摸屏IC
	TP_Write_Byte(CMD);//发送命令字
	delay_us(6);//ADS7846的转换时间最长为6us
	digitalWrite(tp_dev.sck,LOW); 	     	    
	delay_us(1);    	   
	digitalWrite(tp_dev.sck,HIGH);		//给1个时钟，清除BUSY
	delay_us(1);    
	digitalWrite(tp_dev.sck,LOW); 	     	    
	for(count=0;count<16;count++)//读出16位数据,只有高12位有效 
	{ 				  
		Num<<=1; 	 
		digitalWrite(tp_dev.sck,LOW);	//下降沿有效  	    	   
		delay_us(1);    
 		digitalWrite(tp_dev.sck,HIGH);
 		if(digitalRead(tp_dev.miso))Num++; 		 
	}  	
	Num>>=4;   	//只有高12位有效.
	digitalWrite(tp_dev.cs,HIGH);		//释放片选	 
	return(Num);   
}

//读取一个坐标值(x或者y)
//连续读取READ_TIMES次数据,对这些数据升序排列,
//然后去掉最低和最高LOST_VAL个数,取平均值 
//xy:指令（CMD_RDX/CMD_RDY）
//返回值:读到的数据
#define READ_TIMES 5 	//读取次数
#define LOST_VAL 1	  	//丢弃值
uint16_t TP_Read_XOY(uint8_t xy)
{
	uint16_t i, j;
	uint16_t buf[READ_TIMES];
	uint16_t sum=0;
	uint16_t temp;
	for(i=0;i<READ_TIMES;i++)buf[i]=TP_Read_AD(xy);		 		    
	for(i=0;i<READ_TIMES-1; i++)//排序
	{
		for(j=i+1;j<READ_TIMES;j++)
		{
			if(buf[i]>buf[j])//升序排列
			{
				temp=buf[i];
				buf[i]=buf[j];
				buf[j]=temp;
			}
		}
	}	  
	sum=0;
	for(i=LOST_VAL;i<READ_TIMES-LOST_VAL;i++)sum+=buf[i];
	temp=sum/(READ_TIMES-2*LOST_VAL);
	return temp;   
} 
//读取x,y坐标
//最小值不能少于100.
//x,y:读取到的坐标值
//返回值:0,失败;1,成功。
uint8_t TP_Read_XY(uint16_t *x,uint16_t *y)
{
	uint16_t xtemp,ytemp;			 	 		  
	xtemp=TP_Read_XOY(tp_dev.CMD_RDX);
	ytemp=TP_Read_XOY(tp_dev.CMD_RDY);	  												   
	//if(xtemp<100||ytemp<100)return 0;//读数失败
	*x=xtemp;
	*y=ytemp;
	return 1;//读数成功
}

//连续2次读取触摸屏IC,且这两次的偏差不能超过
//ERR_RANGE,满足条件,则认为读数正确,否则读数错误.	   
//该函数能大大提高准确度
//x,y:读取到的坐标值
//返回值:0,失败;1,成功。
#define ERR_RANGE 50 //误差范围 
uint8_t TP_Read_XY2(uint16_t *x,uint16_t *y) 
{
	uint16_t x1,y1;
 	uint16_t x2,y2;
 	uint8_t flag;    
    flag=TP_Read_XY(&x1,&y1);   
    if(flag==0)return(0);
    flag=TP_Read_XY(&x2,&y2);	   
    if(flag==0)return(0);   
    if(((x2<=x1&&x1<x2+ERR_RANGE)||(x1<=x2&&x2<x1+ERR_RANGE))//前后两次采样在+-50内
    &&((y2<=y1&&y1<y2+ERR_RANGE)||(y1<=y2&&y2<y1+ERR_RANGE)))
    {
        *x=(x1+x2)/2;
        *y=(y1+y2)/2;
        return 1;
    }else return 0;	  
}

#if 0
//////////////////////////////////////////////////////////////////////////////////		  
//与LCD部分有关的函数  
//画一个触摸点
//用来校准用的
//x,y:坐标
//color:颜色

//画一个大点(2*2的点)		   
//x,y:坐标
//color:颜色
void TP_Draw_Big_Point(uint16_t x,uint16_t y,uint16_t color)
{	    
	POINT_COLOR=color;
	LCD_DrawPoint(x,y);//中心点 
	LCD_DrawPoint(x+1,y);
	LCD_DrawPoint(x,y+1);
	LCD_DrawPoint(x+1,y+1);	 	  	
}
#endif		
//////////////////////////////////////////////////////////////////////////////////		  
//触摸按键扫描
//tp:0,屏幕坐标;1,物理坐标(校准等特殊场合用)
//返回值:当前触屏状态.

uint8_t TP_Scan(uint8_t tp)
{
    uint16_t x,y;
	float x1,y1;
	TP_Read_XY2(&x,&y);//读取物理坐标
	if((x>100)&&(y>100)){
		tp_dev.x[0]=x;
		tp_dev.y[0]=y;
		if(tp){
			return 1;
	    }else {
		  x1 = (float)((tp_dev.x[0]*tp_dev.xfac)-tp_dev.xoff);
		  y1 = (float)((tp_dev.y[0]*tp_dev.yfac)-tp_dev.yoff);
	      tp_dev.x[0]=(int16_t)x1;                 //将结果转换为屏幕坐标
	      tp_dev.y[0]=(int16_t)y1;  
		  return 1;
	    } 
	}else{
		return 0;
	}			   
}
/*
uint8_t TP_Scan(uint8_t tp)
{			   
	int32_t x1,y1;
	if(digitalRead(tp_dev.irq)==0)//有按键按下
	{
		if(tp)TP_Read_XY2(&tp_dev.x[0],&tp_dev.y[0]);//读取物理坐标
		else if(TP_Read_XY2(&tp_dev.x[0],&tp_dev.y[0]))//读取屏幕坐标
		{
		  x1 = (int32_t)(((tp_dev.x[0]-tp_dev.xoff)*tp_dev.xfac)/1000+10);
		  y1 = (int32_t)(((tp_dev.y[0]-tp_dev.yoff)*tp_dev.yfac)/1000+10);
	      tp_dev.x[0]=x1;                 //将结果转换为屏幕坐标
	      tp_dev.y[0]=y1;  
	 	} 
		return 1
	}
	return 0;	
}	  
*/
//////////////////////////////////////////////////////////////////////////	 
//保存在EEPROM里面的地址区间基址,占用13个字节(RANGE:SAVE_ADDR_BASE~SAVE_ADDR_BASE+12)
#define SAVE_ADDR_BASE 40
//保存校准参数										    
#if 0
void TP_Save_Adjdata(void)
{
	int32 temp;			 
	//保存校正结果!
	temp=tp_dev.xfac*100000000;//保存x校正因素      
    AT24CXX_WriteLenByte(SAVE_ADDR_BASE,temp,4);   
	temp=tp_dev.yfac*100000000;//保存y校正因素    
    AT24CXX_WriteLenByte(SAVE_ADDR_BASE+4,temp,4);
	//保存x偏移量
    AT24CXX_WriteLenByte(SAVE_ADDR_BASE+8,tp_dev.xoff,2);		    
	//保存y偏移量
	AT24CXX_WriteLenByte(SAVE_ADDR_BASE+10,tp_dev.yoff,2);	
	//保存触屏类型
	AT24CXX_WriteOneByte(SAVE_ADDR_BASE+12,tp_dev.touchtype);	
	temp=0X0A;//标记校准过了
	AT24CXX_WriteOneByte(SAVE_ADDR_BASE+13,temp); 
}

//得到保存在EEPROM里面的校准值
//返回值：1，成功获取数据
//        0，获取失败，要重新校准
uint8_t TP_Get_Adjdata(void)
{					  
	s32 tempfac;
	tempfac=AT24CXX_ReadOneByte(SAVE_ADDR_BASE+13);//读取标记字,看是否校准过！ 		 
	if(tempfac==0X0A)//触摸屏已经校准过了			   
	{    												 
		tempfac=AT24CXX_ReadLenByte(SAVE_ADDR_BASE,4);		   
		tp_dev.xfac=(float)tempfac/100000000;//得到x校准参数
		tempfac=AT24CXX_ReadLenByte(SAVE_ADDR_BASE+4,4);			          
		tp_dev.yfac=(float)tempfac/100000000;//得到y校准参数
	    //得到x偏移量
		tp_dev.xoff=AT24CXX_ReadLenByte(SAVE_ADDR_BASE+8,2);			   	  
 	    //得到y偏移量
		tp_dev.yoff=AT24CXX_ReadLenByte(SAVE_ADDR_BASE+10,2);				 	  
 		tp_dev.touchtype=AT24CXX_ReadOneByte(SAVE_ADDR_BASE+12);//读取触屏类型标记
		if(tp_dev.touchtype)//X,Y方向与屏幕相反
		{
			tp_dev.CMD_RDX=0X90;
			tp_dev.CMD_RDY=0XD0;	 
		}else				   //X,Y方向与屏幕相同
		{
			tp_dev.CMD_RDX=0XD0;
			tp_dev.CMD_RDY=0X90;	 
		}		 
		return 1;	 
	}
	return 0;
}	 

//提示字符串
const uint8_t* TP_REMIND_MSG_TBL="Please use the stylus click the cross on the screen.The cross will always move until the screen adjustment is completed.";
 					  
//提示校准结果(各个参数)
void TP_Adj_Info_Show(uint16_t x0,uint16_t y0,uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2,uint16_t x3,uint16_t y3,uint16_t fac)
{	  
	POINT_COLOR=RED;
	LCD_ShowString(40,160,lcddev.width,lcddev.height,16,"x1:");
 	LCD_ShowString(40+80,160,lcddev.width,lcddev.height,16,"y1:");
 	LCD_ShowString(40,180,lcddev.width,lcddev.height,16,"x2:");
 	LCD_ShowString(40+80,180,lcddev.width,lcddev.height,16,"y2:");
	LCD_ShowString(40,200,lcddev.width,lcddev.height,16,"x3:");
 	LCD_ShowString(40+80,200,lcddev.width,lcddev.height,16,"y3:");
	LCD_ShowString(40,220,lcddev.width,lcddev.height,16,"x4:");
 	LCD_ShowString(40+80,220,lcddev.width,lcddev.height,16,"y4:");  
 	LCD_ShowString(40,240,lcddev.width,lcddev.height,16,"fac is:");     
	LCD_ShowNum(40+24,160,x0,4,16);		//显示数值
	LCD_ShowNum(40+24+80,160,y0,4,16);	//显示数值
	LCD_ShowNum(40+24,180,x1,4,16);		//显示数值
	LCD_ShowNum(40+24+80,180,y1,4,16);	//显示数值
	LCD_ShowNum(40+24,200,x2,4,16);		//显示数值
	LCD_ShowNum(40+24+80,200,y2,4,16);	//显示数值
	LCD_ShowNum(40+24,220,x3,4,16);		//显示数值
	LCD_ShowNum(40+24+80,220,y3,4,16);	//显示数值
 	LCD_ShowNum(40+56,lcddev.width,fac,3,16); 	//显示数值,该数值必须在95~105范围之内.
}

//触摸屏校准代码
//得到四个校准参数
void TP_Adjust(void)
{								 
	uint16_t pos_temp[4][2];//坐标缓存值
	uint8_t  cnt=0;	
	uint16_t d1,d2;
	uint32_t tem1,tem2;
	double fac; 	
	uint16_t outtime=0;
 	cnt=0;				
	POINT_COLOR=BLUE;
	BACK_COLOR =WHITE;
	LCD_Clear(WHITE);//清屏   
	POINT_COLOR=RED;//红色 
	LCD_Clear(WHITE);//清屏 	   
	POINT_COLOR=BLACK;
	LCD_ShowString(40,40,160,100,16,(uint8_t*)TP_REMIND_MSG_TBL);//显示提示信息
	TP_Drow_Touch_Point(20,20,RED);//画点1 
	tp_dev.sta=0;//消除触发信号 
	tp_dev.xfac=0;//xfac用来标记是否校准过,所以校准之前必须清掉!以免错误	 
	while(1)//如果连续10秒钟没有按下,则自动退出
	{
		tp_dev.scan(1);//扫描物理坐标
		if((tp_dev.sta&0xc0)==TP_CATH_PRES)//按键按下了一次(此时按键松开了.)
		{	
			outtime=0;		
			tp_dev.sta&=~(1<<6);//标记按键已经被处理过了.
						   			   
			pos_temp[cnt][0]=tp_dev.x[0];
			pos_temp[cnt][1]=tp_dev.y[0];
			cnt++;	  
			switch(cnt)
			{			   
				case 1:						 
					TP_Drow_Touch_Point(20,20,WHITE);				//清除点1 
					TP_Drow_Touch_Point(lcddev.width-20,20,RED);	//画点2
					break;
				case 2:
 					TP_Drow_Touch_Point(lcddev.width-20,20,WHITE);	//清除点2
					TP_Drow_Touch_Point(20,lcddev.height-20,RED);	//画点3
					break;
				case 3:
 					TP_Drow_Touch_Point(20,lcddev.height-20,WHITE);			//清除点3
 					TP_Drow_Touch_Point(lcddev.width-20,lcddev.height-20,RED);	//画点4
					break;
				case 4:	 //全部四个点已经得到
	    		    //对边相等
					tem1=abs(pos_temp[0][0]-pos_temp[1][0]);//x1-x2
					tem2=abs(pos_temp[0][1]-pos_temp[1][1]);//y1-y2
					tem1*=tem1;
					tem2*=tem2;
					d1=sqrt(tem1+tem2);//得到1,2的距离
					
					tem1=abs(pos_temp[2][0]-pos_temp[3][0]);//x3-x4
					tem2=abs(pos_temp[2][1]-pos_temp[3][1]);//y3-y4
					tem1*=tem1;
					tem2*=tem2;
					d2=sqrt(tem1+tem2);//得到3,4的距离
					fac=(float)d1/d2;
					if(fac<0.95||fac>1.05||d1==0||d2==0)//不合格
					{
						cnt=0;
 				    	TP_Drow_Touch_Point(lcddev.width-20,lcddev.height-20,WHITE);	//清除点4
   	 					TP_Drow_Touch_Point(20,20,RED);								//画点1
 						TP_Adj_Info_Show(pos_temp[0][0],pos_temp[0][1],pos_temp[1][0],pos_temp[1][1],pos_temp[2][0],pos_temp[2][1],pos_temp[3][0],pos_temp[3][1],fac*100);//显示数据   
 						continue;
					}
					tem1=abs(pos_temp[0][0]-pos_temp[2][0]);//x1-x3
					tem2=abs(pos_temp[0][1]-pos_temp[2][1]);//y1-y3
					tem1*=tem1;
					tem2*=tem2;
					d1=sqrt(tem1+tem2);//得到1,3的距离
					
					tem1=abs(pos_temp[1][0]-pos_temp[3][0]);//x2-x4
					tem2=abs(pos_temp[1][1]-pos_temp[3][1]);//y2-y4
					tem1*=tem1;
					tem2*=tem2;
					d2=sqrt(tem1+tem2);//得到2,4的距离
					fac=(float)d1/d2;
					if(fac<0.95||fac>1.05)//不合格
					{
						cnt=0;
 				    	TP_Drow_Touch_Point(lcddev.width-20,lcddev.height-20,WHITE);	//清除点4
   	 					TP_Drow_Touch_Point(20,20,RED);								//画点1
 						TP_Adj_Info_Show(pos_temp[0][0],pos_temp[0][1],pos_temp[1][0],pos_temp[1][1],pos_temp[2][0],pos_temp[2][1],pos_temp[3][0],pos_temp[3][1],fac*100);//显示数据   
						continue;
					}//正确了
								   
					//对角线相等
					tem1=abs(pos_temp[1][0]-pos_temp[2][0]);//x1-x3
					tem2=abs(pos_temp[1][1]-pos_temp[2][1]);//y1-y3
					tem1*=tem1;
					tem2*=tem2;
					d1=sqrt(tem1+tem2);//得到1,4的距离
	
					tem1=abs(pos_temp[0][0]-pos_temp[3][0]);//x2-x4
					tem2=abs(pos_temp[0][1]-pos_temp[3][1]);//y2-y4
					tem1*=tem1;
					tem2*=tem2;
					d2=sqrt(tem1+tem2);//得到2,3的距离
					fac=(float)d1/d2;
					if(fac<0.95||fac>1.05)//不合格
					{
						cnt=0;
 				    	TP_Drow_Touch_Point(lcddev.width-20,lcddev.height-20,WHITE);	//清除点4
   	 					TP_Drow_Touch_Point(20,20,RED);								//画点1
 						TP_Adj_Info_Show(pos_temp[0][0],pos_temp[0][1],pos_temp[1][0],pos_temp[1][1],pos_temp[2][0],pos_temp[2][1],pos_temp[3][0],pos_temp[3][1],fac*100);//显示数据   
						continue;
					}//正确了
					//计算结果
					tp_dev.xfac=(float)(lcddev.width-40)/(pos_temp[1][0]-pos_temp[0][0]);//得到xfac		 
					tp_dev.xoff=(lcddev.width-tp_dev.xfac*(pos_temp[1][0]+pos_temp[0][0]))/2;//得到xoff
						  
					tp_dev.yfac=(float)(lcddev.height-40)/(pos_temp[2][1]-pos_temp[0][1]);//得到yfac
					tp_dev.yoff=(lcddev.height-tp_dev.yfac*(pos_temp[2][1]+pos_temp[0][1]))/2;//得到yoff  
					if(abs(tp_dev.xfac)>2||abs(tp_dev.yfac)>2)//触屏和预设的相反了.
					{
						cnt=0;
 				    	TP_Drow_Touch_Point(lcddev.width-20,lcddev.height-20,WHITE);	//清除点4
   	 					TP_Drow_Touch_Point(20,20,RED);								//画点1
						LCD_ShowString(40,26,lcddev.width,lcddev.height,16,"TP Need readjust!");
						tp_dev.touchtype=!tp_dev.touchtype;//修改触屏类型.
						if(tp_dev.touchtype)//X,Y方向与屏幕相反
						{
							tp_dev.CMD_RDX=0X90;
							tp_dev.CMD_RDY=0XD0;	 
						}else				   //X,Y方向与屏幕相同
						{
							tp_dev.CMD_RDX=0XD0;
							tp_dev.CMD_RDY=0X90;	 
						}			    
						continue;
					}		
					POINT_COLOR=BLUE;
					LCD_Clear(WHITE);//清屏
					LCD_ShowString(35,110,lcddev.width,lcddev.height,16,"Touch Screen Adjust OK!");//校正完成
					delay_ms(1000);
//					TP_Save_Adjdata();  
 					LCD_Clear(WHITE);//清屏   
					return;//校正完成				 
			}
		}
		delay_ms(10);
		outtime++;
		if(outtime>1000)
		{
//			TP_Get_Adjdata();
			break;
	 	} 
 	}
}
#else
void TP_Adjust(void){};	
#endif

//触摸屏初始化
//返回值:0,没有进行校准
//		 1,进行过校准
uint8_t TP_Init(void)
{
#if OTT_MAX_TOUCH	
	if(lcddev.id == 0X5510) //电容触摸屏
	{
		OTT2001A_Init();
		tp_dev.scan = CTP_Scan; //扫描函数指向电容触摸屏扫描函数
		tp_dev.touchtype |= 0X80; //电容屏
		tp_dev.touchtype |= lcddev.dir&0x01;//横屏还是竖屏
		return 0;
	}else
#endif
	{ 
    pinMode(tp_dev.cs,OUTPUT);
	pinMode(tp_dev.sck,OUTPUT);	
    pinMode(tp_dev.miso,INPUT);
	pinMode(tp_dev.mosi,OUTPUT);
#ifdef 	TOUCH_NIRQ
	pinMode(tp_dev.irq,INPUT_PULLUP);	
#endif
#ifdef TOUCH_BUSY
	pinMode(TOUCH_BUSY,INPUT);	
#endif
	TP_Read_XY(&tp_dev.x[0],&tp_dev.y[0]); //第一次读取初始化
	TP_Read_XY(&tp_dev.x[0],&tp_dev.y[0]); //第一次读取初始化

//		AT24CXX_Init(); //初始化24CXX
//		if(TP_Get_Adjdata())return 0; //已经校准过
//		else   //未校准过？
//		{
//			LCD_Clear(BACK_COLOR); //清屏
//			TP_Adjust(); //屏幕校准
//			TP_Save_Adjdata(); //保存校准数据
//		}
//		TP_Get_Adjdata(); //获取校准值
    }
	return 1;
}
	
