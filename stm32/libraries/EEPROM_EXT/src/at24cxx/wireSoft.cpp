#include <Arduino.h>
#include "wireSoft.h"

//////////////////////////////////////////////////////////////////////////////////	 
//IIC software simulate driver by huaweiwx@sina.com 2017.6.28	   
////////////////////////////////////////////////////////////////////////////////// 

TwoWireSoft::TwoWireSoft(){
}

TwoWireSoft::TwoWireSoft(uint8_t sda,uint8_t scl){
		this->setPins(sda,scl);
}

void TwoWireSoft::setPins(uint8_t sda,uint8_t scl){
		pdev->sda = sda;
		pdev->scl = scl;
}


//初始化IIC
void TwoWireSoft::begin(void)
{			
  pinMode(pdev->sda,OUTPUT_OD);
  pinMode(pdev->scl,OUTPUT);
  digitalWrite(pdev->scl,HIGH);
  digitalWrite(pdev->sda,HIGH);
}

//产生IIC起始信号
void TwoWireSoft::start(void)
{
	pinMode(pdev->sda,OUTPUT_OD);     //sda线输出
	digitalWrite(pdev->sda,HIGH);	  	  
	digitalWrite(pdev->scl,HIGH);
	delayMicroseconds(4);
 	digitalWrite(pdev->sda,LOW);//START:when CLK is high,DATA change form high to low 
	delayMicroseconds(4);
	digitalWrite(pdev->scl,LOW);//钳住I2C总线，准备发送或接收数据 
}	  
//产生IIC停止信号
void TwoWireSoft::stop(void)
{
	pinMode(pdev->sda,OUTPUT_OD);//sda线输出
	digitalWrite(pdev->scl,LOW);
	digitalWrite(pdev->sda,LOW);//STOP:when CLK is high DATA change form low to high
 	delayMicroseconds(4);
	digitalWrite(pdev->scl,HIGH); 
	digitalWrite(pdev->sda,HIGH);//发送I2C总线结束信号
	delayMicroseconds(4);							   	
}
//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
uint8_t TwoWireSoft::waitAck(void)
{
	uint8_t ucErrTime=0;
	pinMode(pdev->sda,INPUT);      //SDA设置为输入  
	digitalWrite(pdev->sda,HIGH);delayMicroseconds(1);	   
	digitalWrite(pdev->scl,HIGH);delayMicroseconds(1);	 
	while(digitalRead(pdev->sda))
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			this->stop();
			return 1;
		}
	}
	digitalWrite(pdev->scl,LOW);//时钟输出0 	   
	return 0;  
} 
//产生ACK应答
void TwoWireSoft::ack(void)
{
	digitalWrite(pdev->scl,LOW);
	pinMode(pdev->sda,OUTPUT_OD);
	digitalWrite(pdev->sda,LOW);
	delayMicroseconds(2);
	digitalWrite(pdev->scl,HIGH);
	delayMicroseconds(2);
	digitalWrite(pdev->scl,LOW);
}
//不产生ACK应答		    
void TwoWireSoft::nAck(void)
{
	digitalWrite(pdev->scl,LOW);
	pinMode(pdev->sda,OUTPUT_OD);
	digitalWrite(pdev->sda,HIGH);
	delayMicroseconds(2);
	digitalWrite(pdev->scl,HIGH);
	delayMicroseconds(2);
	digitalWrite(pdev->scl,LOW);
}					 				     
//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答			  
void TwoWireSoft::sendByte(uint8_t txd)
{                        
    uint8_t t;   
	pinMode(pdev->sda,OUTPUT_OD); 	    
    digitalWrite(pdev->scl,LOW);//拉低时钟开始数据传输
    for(t=0;t<8;t++)
    {              
        digitalWrite(pdev->sda,(txd&0x80)>>7);
        txd<<=1; 	  
		delayMicroseconds(2);   //对TEA5767这三个延时都是必须的
		digitalWrite(pdev->scl,HIGH);
		delayMicroseconds(2); 
		digitalWrite(pdev->scl,LOW);	
		delayMicroseconds(2);
    }	 
} 	    
//读1个字节，ack=1时，发送ACK，ack=0，发送nACK   
uint8_t TwoWireSoft::readByte(unsigned char ack)
{
	unsigned char i,receive=0;
	pinMode(pdev->sda,INPUT);//SDA设置为输入
    for(i=0;i<8;i++ )
	{
        digitalWrite(pdev->scl,LOW); 
        delayMicroseconds(2);
		digitalWrite(pdev->scl,HIGH);
        receive<<=1;
        if(digitalRead(pdev->sda))receive++;   
		delayMicroseconds(1); 
    }					 
    if (!ack)
        this->nAck();//发送nACK
    else
        this->ack(); //发送ACK   
    return receive;
}



























