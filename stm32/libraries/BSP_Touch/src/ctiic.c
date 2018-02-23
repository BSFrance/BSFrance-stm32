#include <Arduino.h>
#if __has_include("bsp.h")
#  include "bsp.h"
#endif
#include "ctiic.h"

#if defined(CT_SCL_PIN) && defined(CT_SDA_PIN)
   	   		   
//IO方向设置
#define CT_SDA_IN   pinMode(CT_SDA_PIN,INPUT)
#define CT_SDA_OUT  pinMode(CT_SDA_PIN,OUTPUT)

//IO操作函数	 
#define CT_IIC_SCL(x)    digitalWrite(CT_SCL_PIN,x) 			 /*SCL*/
#define CT_IIC_SDA(x)    digitalWrite(digitalWrite(CT_SDA_PIN,x) /*SDA*/
#define CT_READ_SDA      digitalRead(CT_SDA_PIN)  			//输入SDA 

//////////////////////////////////////////////////////////////////////////////////

//电容触摸芯片IIC接口初始化
void CT_IIC_Init(void)
{					     
 	pinMode(CT_SCL_PIN,OUTPUT);
 	pinMode(CT_SDA_PIN,OUTPUT);
	digitalWrite(CT_SCL_PIN,HIGH);
	digitalWrite(CT_SDA_PIN,HIGH);	 
}

//产生IIC起始信号
void CT_IIC_Start(void)
{
	CT_SDA_OUT;     //sda线输出
	CT_IIC_SDA(HIGH);	  	  
	CT_IIC_SCL(HIGH);
	delay_us(10);
 	CT_IIC_SDA(LOW);//START:when CLK is high,DATA change form high to low 
	delay_us(10);
	CT_IIC_SCL(LOW);//钳住I2C总线，准备发送或接收数据 
}	  
//产生IIC停止信号
void CT_IIC_Stop(void)
{
	CT_SDA_OUT;//sda线输出
	CT_IIC_SCL(HIGH);
	delay_us(10);
	CT_IIC_SDA(LOW);//STOP:when CLK is high DATA change form low to high
	delay_us(10);
	CT_IIC_SDA(HIGH);//发送I2C总线结束信号
	delay_us(10);
}
//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
uint8_t CT_IIC_Wait_Ack(void)
{
	uint8_t ucErrTime=0;
	CT_SDA_IN;      //SDA设置为输入  
	CT_IIC_SDA(HIGH);delay_us(1);	   
	CT_IIC_SCL(HIGH);delay_us(1);	 
	while(CT_READ_SDA)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			CT_IIC_Stop();
			return 1;
		} 
	}
	CT_IIC_SCL(LOW);//时钟输出0 	   
	return 0;  
} 
//产生ACK应答
void CT_IIC_Ack(void)
{
	CT_IIC_SCL(LOW);
	CT_SDA_OUT();
	CT_IIC_SDA(LOW);
	delay_us(10);
	CT_IIC_SCL(HIGH);
	delay_us(10);
	CT_IIC_SCL(LOW);
}
//不产生ACK应答		    
void CT_IIC_NAck(void)
{
	CT_IIC_SCL(LOW);
	CT_SDA_OUT();
	CT_IIC_SDA(HIGH);
	delay_us(10);
	CT_IIC_SCL(HIGH);
	delay_us(10);
	CT_IIC_SCL(LOW);
}					 				     
//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答			  
void CT_IIC_Send_Byte(uint8_t txd)
{                        
    uint8_t t;   
	CT_SDA_OUT(); 	    
    CT_IIC_SCL(LOW);//拉低时钟开始数据传输
    for(t=0;t<8;t++)
    {              
        CT_IIC_SDA=(txd&0x80)>>7;
        txd<<=1; 	      
		CT_IIC_SCL(HIGH);
		delay_us(10);
		CT_IIC_SCL(LOW);	
		delay_us(10);
    }	 
} 	    
//读1个字节，ack=1时，发送ACK，ack=0，发送nACK   
uint8_t CT_IIC_Read_Byte(unsigned char ack)
{
	uint8_t i,receive=0;
 	CT_SDA_IN();//SDA设置为输入
    for(i=0;i<8;i++ )
	{
        CT_IIC_SCL(LOW); 	    	   
		delay_us(30);
		CT_IIC_SCL(HIGH);  
		receive<<=1;
		if(CT_READ_SDA)receive++;   
	}	  				 
	if (!ack)CT_IIC_NAck();//发送nACK
	else CT_IIC_Ack(); //发送ACK   
 	return receive;
}


#endif //define(CT_SCL_PIN)&&define(CT_SDA_PIN)


























